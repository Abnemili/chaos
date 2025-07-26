#include "minishell.h"

// // Struct to handle heredoc state
// typedef struct s_heredoc_state {
//     volatile int interrupted;
//     // struct sigaction old_sa;
// } t_heredoc_state;



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
	cmd->heredoc_fd = -1;  // Initialize heredoc_fd
	cmd->heredoc_tmp = NULL;  // Initialize heredoc_tmp
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