/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 20:04:59 by abnemili          #+#    #+#             */
/*   Updated: 2025/06/13 10:17:37 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int handle_heredoc(t_data *data, t_elem **current, t_cmd *cmd)
{
    (void)data;
    int     pipe_fd[2];
    char    *line;
    char    *delimiter;

    *current = (*current)->next;
    skip_whitespace_ptr(current);
    if (!*current)
        return (0);
    delimiter = (*current)->content;
    if (pipe(pipe_fd) == -1) // open the arr discriptor
        return (0);
    while (1)
    {
        write (STDOUT_FILENO, "> ", 2);
        line = get_next_line(STDIN_FILENO);
       if (!line)
            break;
        if (line[ft_strlen(line) - 1] == '\n')
            line[ft_strlen(line) - 1 ] = '\0'; //  set the null , to detect the dilimiter
        if (strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(pipe_fd[1], line, ft_strlen(line));
        write (pipe_fd[1], "\n", 1);
        free(line);
    }
    close(pipe_fd[1]);
    if (cmd->in_file != STDIN_FILENO)
        close(cmd->in_file);
    cmd->in_file = pipe_fd[0];
    *current = (*current)->next;
    return (1);
}
