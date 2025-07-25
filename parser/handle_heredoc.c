/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 20:04:59 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/25 14:15:29 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *simple_itoa(int n)
{
    char    *result;
    int     len = 1;
    int     tmp = n;
    int     i;
    
    /* Calculate length */
    while (tmp /= 10)
        len++;
    
    result = malloc(len + 1);    
    if (!result)
        return NULL;
    
    result[len] = '\0';    
    i = len - 1;
    while (i >= 0)
    {
        result[i--] = '0' + (n % 10);
        n /= 10;
    }    
    
    return result;
}    

static char *generate_heredoc_filename(void)
{
    char    *filename;
    char    *pid_str;
    char    *counter_str;
    static int counter = 0;
    
    pid_str = simple_itoa(getpid());
    counter_str = simple_itoa(counter++);
    if (!pid_str || !counter_str)
    {
        free(pid_str);
        free(counter_str);
        return NULL;
    }    
    
    filename = ft_strjoin("/tmp/.minishell_heredoc_", pid_str);
    free(pid_str);
    if (!filename)
    {
        free(counter_str);
        return NULL;
    }    
    
    pid_str = ft_strjoin(filename, "_");
    free(filename);
    filename = pid_str;
    if (!filename)
    {
        free(counter_str);
        return NULL;
    }    
    
    pid_str = ft_strjoin(filename, counter_str);
    free(filename);
    free(counter_str);
    
    return pid_str;
}    

int create_heredoc_file(char *filename)
{
    int fd = open(filename, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1)
        return -1;
    close(fd);    
    return 0;
}    


static int fill_heredoc(t_data *data, const char *filename, const char *delimiter)
{
    int     fd;
    char    *line;
    char    *expanded_line;
    int     result = 1;

    // Ignore signals during heredoc input
    ignore_signals();

    fd = open(filename, O_WRONLY | O_TRUNC);
    if (fd == -1)
    {
        // Restore signals
        handle_signals(&data->exit_status);
        return 0;
    }

    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            // User pressed Ctrl+D or input stream ended
            if (errno == EINTR)  // If interrupted (not guaranteed with signal())
                data->exit_status = 130;
            result = 0;
            break;
        }

        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }

        expanded_line = expand_token_content(line, data->exit_status, 1, data->env_list);
        free(line);
        if (!expanded_line)
        {
            result = 0;
            break;
        }

        if (write(fd, expanded_line, ft_strlen(expanded_line)) == -1 ||
            write(fd, "\n", 1) == -1)
        {
            free(expanded_line);
            result = 0;
            break;
        }
        free(expanded_line);
    }

    close(fd);

    // Restore signal handlers after heredoc
    handle_signals(&data->exit_status);

    if (data->exit_status == 130)
        return 0;

    return result;
}


int handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd)
{
    char    *filename;
    char    *delimiter;
    int     fill_result;
    
    if (!data || !current || !*current || !cmd)
        return 0;
    
    // Clean up any existing heredoc
    if (cmd->heredoc_tmp)
    {
        unlink(cmd->heredoc_tmp);
        free(cmd->heredoc_tmp);
        cmd->heredoc_tmp = NULL;
    }
    
    // Initialize heredoc_fd if not set
    if (cmd->heredoc_fd == 0)
        cmd->heredoc_fd = -1;
    
    // Get delimiter
    *current = (*current)->next;
    skip_whitespace_ptr(current);
    if (!*current || (*current)->type != WORD)
        return 0;
    delimiter = (*current)->content;
    
    // Create heredoc file
    filename = generate_heredoc_filename();
    if (!filename || create_heredoc_file(filename) == -1)
    {
        free(filename);
        data->file_error = 1;
        return 0;
    }
    
    // Fill heredoc content
    fill_result = fill_heredoc(data, filename, delimiter);
    if (!fill_result)
    {
        unlink(filename);
        free(filename);
        // Don't set file_error for interruption, just return failure
        if (data->exit_status == 130)
            return 0;
        data->file_error = 1;
        return 0;
    }
    
    // Open for reading
    cmd->heredoc_fd = open(filename, O_RDONLY);
    if (cmd->heredoc_fd == -1)
    {
        unlink(filename);
        free(filename);
        data->file_error = 1;
        return 0;
    }
    
    // Set as input
    if (cmd->in_file != STDIN_FILENO)
        close(cmd->in_file);
    cmd->in_file = cmd->heredoc_fd;
    cmd->heredoc_tmp = filename;
    
    *current = (*current)->next;
    return 1;
}

void cleanup_heredoc(t_cmd *cmd)
{
    if (!cmd)
        return;
    
    if (cmd->heredoc_tmp)
    {
        unlink(cmd->heredoc_tmp);
        free(cmd->heredoc_tmp);
        cmd->heredoc_tmp = NULL;
    }
    
    if (cmd->heredoc_fd != -1)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1;
    }
}