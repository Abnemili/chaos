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

static char *generate_heredoc_filename(void)
{
    char    *filename;
    char    *base;
    char    pid_str[16];
    char    counter_str[16];
    static int counter = 0;
    
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    snprintf(counter_str, sizeof(counter_str), "%d", counter++);
    
    base = ft_strjoin("/tmp/.minishell_heredoc_", pid_str);
    if (!base)
        return NULL;
    
    filename = ft_strjoin(base, "_");
    free(base);
    if (!filename)
        return NULL;
    
    base = ft_strjoin(filename, counter_str);
    free(filename);
    return base;
}

char *create_heredoc_file(void)
{
    char    *filename;
    int     fd;
    
    filename = generate_heredoc_filename();
    if (!filename)
        return NULL;
    
    fd = open(filename, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1)
    {
        free(filename);
        return NULL;
    }
    close(fd);
    return filename;
}

static int fill_heredoc(t_data *data, const char *filename, const char *delimiter)
{
    int     fd;
    char    *line;
    char    *expanded_line;
    
    fd = open(filename, O_WRONLY | O_TRUNC);
    if (fd == -1)
        return 0;
    
    while (1)
    {
        line = readline("> ");
        if (!line)
            break;
        
        if (ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        
        expanded_line = expand_token_content(line, data->exit_status, 1, data->env_list);
        free(line);
        if (!expanded_line)
        {
            close(fd);
            return 0;
        }
        
        write(fd, expanded_line, ft_strlen(expanded_line));
        write(fd, "\n", 1);
        free(expanded_line);
    }
    
    close(fd);
    return 1;
}

int handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd)
{
    char    *filename;
    char    *delimiter;
    
    if (!data || !current || !*current || !cmd)
        return 0;
    
    // Clean up any existing heredoc
    cleanup_heredoc(cmd);
    
    // Get delimiter
    *current = (*current)->next;
    skip_whitespace_ptr(current);
    if (!*current || (*current)->type != WORD)
        return 0;
    delimiter = (*current)->content;
    
    // Create and fill heredoc file
    filename = create_heredoc_file();
    if (!filename || !fill_heredoc(data, filename, delimiter))
    {
        if (filename)
            free(filename);
        data->file_error = 1;
        return 0;
    }
    
    // Open for reading
    cmd->heredoc_fd = open(filename, O_RDONLY);
    if (cmd->heredoc_fd == -1)
    {
        free(filename);
        data->file_error = 1;
        return 0;
    }
    
    // Set as input and store filename
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
    
    if (cmd->heredoc_fd != -1 && cmd->heredoc_fd != STDIN_FILENO)
    {
        close(cmd->heredoc_fd);
        cmd->heredoc_fd = -1;
    }
    
    if (cmd->in_file == cmd->heredoc_fd)
        cmd->in_file = STDIN_FILENO;
}