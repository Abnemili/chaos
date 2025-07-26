
#include "minishell.h"

int	process_word_token(t_data *data, t_elem **current, t_cmd *cmd, int *arg_index)
{
	if (!data || !current || !*current || !cmd || !arg_index)
		return (0);
	if (cmd->full_cmd && !is_redirection_target(*current, data->elem))
	{
		cmd->full_cmd[*arg_index] = ft_strdup((*current)->content);
		if (!cmd->full_cmd[*arg_index])
			return (0);
		(*arg_index)++;
	}
	*current = (*current)->next;
	return (1);
}

// Simplified redirection processing
int	process_redirection(t_data *data, t_elem **current, t_cmd *cmd)
{
	if (!data || !current || !*current || !cmd)
		return (0);
	if ((*current)->type == REDIR_IN)
		return (handle_redirection_in(data, current, cmd));
	if ((*current)->type == REDIR_OUT)
		return (handle_redirection_out(data, current, cmd));
	if ((*current)->type == DREDIR_OUT)
		return (handle_redirection_append(data, current, cmd));
	if ((*current)->type == HERE_DOC)
		return (handle_heredoc(data, current, cmd));
	*current = (*current)->next;
	return (1);
}

// Fixed argument parsing with better error handling
int	parse_arguments(t_data *data, t_elem **current, t_cmd *cmd)
{
	int	arg_count;
	int	arg_index;

	if (!data || !current || !cmd)
		return (0);
	arg_count = count_command_args(*current);
	if (!allocate_cmd_args(cmd, arg_count))
		return (0);
	arg_index = 0;
	while (*current && (*current)->type != PIPE_LINE)
	{
		skip_whitespace_ptr(current);
		if (!*current || (*current)->type == PIPE_LINE)
			break;
		// Handle both WORD and ENV tokens as arguments
		// 
		else if ((*current)->type == WORD || (*current)->type == ENV)
{
	char *merged = ft_strdup((*current)->content);
	if (!merged)
		return (0);

	t_elem *start = *current;
	t_elem *next = (*current)->next;

	while (next && (next->type == WORD || next->type == ENV))
	{
		// Check if there is whitespace between current and next
		t_elem *tmp = start->next;
		int separated_by_whitespace = 0;
		while (tmp && tmp != next)
		{
			if (tmp->type == WHITE_SPACE)
			{
				separated_by_whitespace = 1;
				break;
			}
			tmp = tmp->next;
		}
		if (separated_by_whitespace)
			break;

		char *tmp_str = ft_strjoin(merged, next->content);
		free(merged);
		if (!tmp_str)
			return (0);
		merged = tmp_str;

		start = next;
		next = next->next;
	}

	if (cmd->full_cmd)
		cmd->full_cmd[arg_index++] = merged;

	*current = start->next;
}

		else if (!process_redirection(data, current, cmd))
			return (0);
	}
	return (1);
}
