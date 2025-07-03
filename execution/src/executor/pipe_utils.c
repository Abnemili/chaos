
#include "../../include/executor.h"

void	exit_status(char *failed, int status)
{
	ft_printf("%s: %s\n", failed, strerror(status));
	free_parser(get_parser());
	exit(1);
}

void	one_cmd_norm(t_cmd *cmd, t_parser *parser, pid_t pid)
{
	if (pid == -1)
		exit_status("fork", -1);
	if (pid == 0)
	{
		if (cmd->in_fd != 0)
			dup2(cmd->in_fd, 0);
		if (cmd->out_fd != 1)
			dup2(cmd->out_fd, 1);
		test_redirection_fd(cmd);
		error_display(cmd);
		if (check_builtins(parser, cmd, pid))
		{
			free_parser(parser);
			exit(0);
		}
		execve(cmd->args[0], cmd->args, convert_path_to_array(parser->env));
		perror("execve");
		exit(255);
	}
}

void	exec_one_command(t_cmd *cmd, t_parser *parser)
{
	pid_t	pid;
	int		status;

	g_sigchild = 1;
	cmd->pid = 1;
	if (before_child(cmd))
		return ;
	pid = fork();
	signal(SIGQUIT, sigquit_handler);
	one_cmd_norm(cmd, parser, pid);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		parser->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		parser->exit_status = WTERMSIG(status) + 128;
	g_sigchild = 0;
	signal(SIGQUIT, sigquit_handler);
}
