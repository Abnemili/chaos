
#include "../../include/executor.h"

int	*get_lastpid(void)
{
	static int	pid;

	return (&pid);
}

void	error_display_norm(t_cmd *cmd)
{
	if (cmd->access == NOT_AVAIL)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free_parser(get_parser());
		exit(127);
	}
	else if (cmd->access == NOT_EXEC)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": Permission denied\n", 2);
		free_parser(get_parser());
		exit(126);
	}
}

void	error_display(t_cmd *cmd)
{
	if (cmd->access != IS_EXEC)
	{
		if (cmd->access == NULL_CMD)
		{
			free_parser(get_parser()),
			exit(0);
		}
		if (cmd->access == CMD_NOT_FOUND)
		{
			ft_putstr_fd(cmd->args[0], 2);
			ft_putstr_fd(" : command not found\n", 2);
			free_parser(get_parser());
			exit(127);
		}
		else if (cmd->access == IS_A_DIR)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->args[0], 2);
			ft_putstr_fd(": Is a directory\n", 2);
			free_parser(get_parser());
			exit(126);
		}
		error_display_norm(cmd);
	}
}

void	pipe_utils(t_cmd *cmd, int status)
{
	if (cmd->pid != *get_lastpid())
		waitpid(cmd->pid, &status, 0);
	else if (cmd->pid == *get_lastpid())
	{
		waitpid(cmd->pid, &status, 0);
		if (WIFEXITED(status))
			get_parser()->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			get_parser()->exit_status = WTERMSIG(status) + 128;
	}
}
