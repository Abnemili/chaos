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

static void reverse_str(char *str, int len)
{
    char *end = str + len - 1;
    while (str < end) {
        char tmp = *str;
        *str++ = *end;
        *end-- = tmp;
    }
}
static char *generate_heredoc_filename(void)
{
    char    *filename;
    int     pid;
    static int counter = 0;
    
    // Calculate needed size (base + pid + _ + counter + null)
    pid = getpid();
    filename = malloc(30 + 10 + 1 + 10 + 1); // Extra space for safety
    if (!filename)
        return NULL;
    
    // Manually format the string
    char *ptr = filename;
    
    // Add base path
    const char *base = "/tmp/.minishell_heredoc_";
    while (*base)
        *ptr++ = *base++;
    
    // Add PID
    int n = pid;
    char *start = ptr;
    do {
        *ptr++ = (n % 10) + '0';
        n /= 10;
    } while (n > 0);
    reverse_str(start, ptr - start);
    
    // Add separator
    *ptr++ = '_';
    
    // Add counter
    n = counter++;
    start = ptr;
    do {
        *ptr++ = (n % 10) + '0';
        n /= 10;
    } while (n > 0);
    reverse_str(start, ptr - start);
    
    // Null terminate
    *ptr = '\0';
    
    return filename;
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