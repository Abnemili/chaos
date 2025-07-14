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


int handle_redirection_in(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_RDONLY);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->in_file != STDIN_FILENO)
        close(cmd->in_file);
    cmd->in_file = fd;

    *current = (*current)->next;
    return (1);
}

// Handles '>' redirection (truncate output)
int handle_redirection_out(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->out_file != STDOUT_FILENO)
        close(cmd->out_file);
    cmd->out_file = fd;

    *current = (*current)->next;
    return (1);
}

// Handles '>>' redirection (append to output)
int handle_redirection_append(t_data *data, t_elem **current, t_cmd *cmd)
{
    int fd;

    if (!data || !current || !*current || !cmd)
        return (0);

    *current = (*current)->next;
    skip_whitespace_ptr(current);

    if (!*current || (*current)->type != WORD)
        return (0);

    fd = open((*current)->content, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror((*current)->content);
        data->file_error = 1;
        *current = (*current)->next;
        return (0);
    }

    if (cmd->out_file != STDOUT_FILENO)
        close(cmd->out_file);
    cmd->out_file = fd;

    *current = (*current)->next;
    return (1);
}


void	skip_whitespace_ptr(t_elem **current)
{
	if (!current)
		return;
	while (*current && (*current)->type == WHITE_SPACE)
		*current = (*current)->next;
}

int	count_command_args(t_elem *start)
{
	int	count;
	t_elem	*current;

	count = 0;
	current = start;
	while (current && current->type != PIPE_LINE)
	{
		if (current->type == WORD || current->type == ENV)
		{
			if (!is_redirection_target(current, start))
				count++;
		}
		current = current->next;
	}
	return (count);
}

// Fixed memory allocation with proper cleanup
int	allocate_cmd_args(t_cmd *cmd, int arg_count)
{
	if (!cmd)
		return (0);
	if (arg_count <= 0)
	{
		cmd->full_cmd = NULL;
		return (1);
	}
	cmd->full_cmd = malloc(sizeof(char *) * (arg_count + 1));
	if (!cmd->full_cmd)
		return (0);
	ft_memset(cmd->full_cmd, 0, sizeof(char *) * (arg_count + 1));
	return (1);
}


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

int	is_redirection_target(t_elem *elem, t_elem *start)
{
	t_elem	*current;
	t_elem	*prev;

	if (!elem || !start)
		return (0);
	current = start;
	prev = NULL;
	while (current)
	{
		if (current == elem)
			break;
		if (current->type != WHITE_SPACE)
			prev = current;
		current = current->next;
	}
	if (!current || !prev)
		return (0);
	return (prev->type == REDIR_IN || prev->type == REDIR_OUT ||
		prev->type == DREDIR_OUT || prev->type == HERE_DOC);
}
int	parse_pipeline(t_data *data)
{
	t_elem	*current;
	t_cmd	*current_cmd;
	t_cmd	*last_cmd;

	if (!data || !data->elem)
		return (0);
	current = data->elem;
	data->head = NULL;
	last_cmd = NULL;
	while (current)
	{
		skip_whitespace_ptr(&current);
		if (!current)
			break;
		if (current->type == PIPE_LINE)
		{
			current = current->next;
			skip_whitespace_ptr(&current);
			if (!current)
				return (0);
			continue;
		}
		current_cmd = parse_command(data, &current);
		if (!current_cmd)
		{
			free_cmd_list(data->head);
			return (0);
		}
		if (!data->head)
			data->head = current_cmd;
		else
			last_cmd->next = current_cmd;
		last_cmd = current_cmd;
	}
	return (data->head != NULL);
}

// Fixed memory cleanup on failure
t_cmd	*parse_command(t_data *data, t_elem **current)
{
	t_cmd	*cmd;

	if (!data || !current)
		return (NULL);
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	cmd->in_file = STDIN_FILENO;
	cmd->out_file = STDOUT_FILENO;
	cmd->full_cmd = NULL;
	cmd->next = NULL;
	if (!parse_arguments(data, current, cmd))
	{
		free_cmd(cmd);
		return (NULL);
	}
	return (cmd);
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
		if ((*current)->type == WORD || (*current)->type == ENV)
		{
			if (!process_word_token(data, current, cmd, &arg_index))
				return (0);
		}
		else if (!process_redirection(data, current, cmd))
			return (0);
	}
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