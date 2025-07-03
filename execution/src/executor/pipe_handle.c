
#include "../../include/executor.h"

void	close_fds(int cmd_count, int *fd)
{
	int	i;

	i = 0;
	while (i < 2 * (cmd_count - 1))
	{
		close(fd[i]);
		i++;
	}
}

void	execute_cmd(t_cmd **cmd, int cmd_count, int *fd, int i)
{
	if (cmd[i]->pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		child_process(cmd, fd, i, cmd_count);
		test_redirection_fd(cmd[i]);
		error_display(cmd[i]);
		close_fds(cmd_count, fd);
		if (check_builtins(get_parser(), cmd[i], cmd[i]->pid))
			exit(0);
		execve(cmd[i]->args[0], cmd[i]->args,
			convert_path_to_array(get_parser()->env));
		perror("execve");
		free_parser(get_parser());
		exit(255);
	}
}

void	child_process(t_cmd **cmd, int *fd, int i, int cmd_count)
{
	if (!i)
		dup2(cmd[i]->in_fd, 0);
	if (i == cmd_count - 1)
		dup2(cmd[i]->out_fd, 1);
	if (i != 0)
	{
		if (!cmd[i]->in_fd)
			cmd[i]->in_fd = fd[(i - 1) * 2];
		dup2(cmd[i]->in_fd, 0);
	}
	if (i != cmd_count - 1)
	{
		if (cmd[i]->out_fd == 1)
			cmd[i]->out_fd = fd[i * 2 + 1];
		dup2(cmd[i]->out_fd, 1);
	}
}

void	forking_accordingly(t_cmd **cmd, int cmd_count, int *fd)
{
	int	i;
	int	*last;

	i = 0;
	g_sigchild = 1;
	while (i < cmd_count)
	{
		last = get_lastpid();
		signal(SIGQUIT, sigquit_handler);
		cmd[i]->pid = fork();
		if (cmd[i]->pid < 0)
			exit_status("fork failed", 1);
		if (!cmd[i + 1] && cmd[i]->pid)
			*last = cmd[i]->pid;
		execute_cmd(cmd, cmd_count, fd, i);
		i++;
	}
}

void	create_pipe(t_cmd **cmd, int cmd_count)
{
	int	*fd;
	int	i;
	int	status;

	i = 0;
	status = 0;
	fd = ft_malloc(sizeof(int) * (2 * (cmd_count - 1)));
	while (i < cmd_count - 1)
	{
		if (pipe(fd + i * 2) < 0)
			exit_status("pipe failed", 1);
		i++;
	}
	forking_accordingly(cmd, cmd_count, fd);
	close_fds(cmd_count, fd);
	i = 0;
	while (i < cmd_count)
	{
		pipe_utils(cmd[i], status);
		i++;
	}
	ft_free(fd);
	g_sigchild = 0;
	signal(SIGQUIT, sigquit_handler);
}
