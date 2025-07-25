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

static char *ft_create_tmpfile(void)
{
    char    *tmp_name;
    int     fd;
    int     i;
    const char *base_dir;

    // Try standard temporary directories first
    const char *tmp_dirs[] = {"/tmp", "/var/tmp", ".", NULL};
    
    i = 0;
    while (tmp_dirs[i])
    {
        base_dir = tmp_dirs[i];
        tmp_name = ft_strjoin(base_dir, "/.minishell_heredoc_XXXXXX");
        if (!tmp_name)
            return NULL;
        
        // Simple attempt to create unique filename
        fd = open(tmp_name, O_CREAT | O_EXCL | O_RDWR, 0600);
        if (fd != -1)
        {
            close(fd);
            return tmp_name;
        }
        free(tmp_name);
        i++;
    }
    return NULL;
}
int handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd)
{
    int tmp_fd;
    char *tmp_name;
    char *delimiter;
    char *line;
    char *expanded_line;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;  // Move past HERE_DOC token
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    delimiter = (*current)->content;

    // Create unique temporary file for heredoc content
    tmp_name = ft_create_tmpfile();
    if (!tmp_name)
    {
        data->file_error = 1;
        return (0);
    }

    tmp_fd = open(tmp_name, O_WRONLY | O_TRUNC);
    if (tmp_fd == -1)
    {
        perror("open heredoc temp file");
        free(tmp_name);
        data->file_error = 1;
        return (0);
    }

    while (1)
    {
        line = readline("> ");
        if (!line)
        {
            // EOF or Ctrl-D
            break;
        }

        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }

        // Expand variables in heredoc line before writing
        expanded_line = expand_token_content(line, data->exit_status, 1, data->env_list);
        free(line);
        if (!expanded_line)
        {
            close(tmp_fd);
            unlink(tmp_name);
            free(tmp_name);
            return (0);
        }

        write(tmp_fd, expanded_line, strlen(expanded_line));
        write(tmp_fd, "\n", 1);
        free(expanded_line);
    }

    close(tmp_fd);

    // Reopen for reading to assign to cmd->in_file
    tmp_fd = open(tmp_name, O_RDONLY);
    if (tmp_fd == -1)
    {
        perror("open heredoc temp file");
        unlink(tmp_name);
        free(tmp_name);
        data->file_error = 1;
        return (0);
    }

    // Assign file descriptor to command input, closing previous if needed
    if (cmd->in_file != STDIN_FILENO)
        close(cmd->in_file);
    cmd->in_file = tmp_fd;

    // Store the temp filename for later cleanup
    if (cmd->heredoc_tmp)
        free(cmd->heredoc_tmp);
    cmd->heredoc_tmp = tmp_name;

    *current = (*current)->next;
    return (1);
}
