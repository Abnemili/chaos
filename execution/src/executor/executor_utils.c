
#include "../../include/executor.h"

int	check_builtins(t_parser *parser, t_cmd *cmd, pid_t pid)
{
	t_cmd	*tmp;

	tmp = cmd;
	if (!ft_strcmp(tmp->args[0], "echo"))
		return (ft_echo(tmp, pid), 1);
	else if (!ft_strcmp(tmp->args[0], "env"))
	{
		if (tmp->count == 1)
			return (ft_env(parser->env, pid), 1);
		else if (tmp->count > 1)
			ft_putendl_fd("env: too many arguments", 2);
	}
	else if (!ft_strcmp(tmp->args[0], "exit"))
		return (ft_exit(tmp, pid), 1);
	else if (!ft_strcmp(cmd->args[0], "unset"))
		return (ft_unset(cmd, pid), 1);
	else if (!ft_strcmp(cmd->args[0], "cd"))
		return (ft_cd(cmd, pid), 1);
	else if (!ft_strcmp(cmd->args[0], "pwd"))
		return (ft_pwd(pid), 1);
	else if (!ft_strcmp(cmd->args[0], "export"))
		return (ft_export(cmd), 1);
	return (0);
}

int	before_child(t_cmd *cmd)
{
	if (!ft_strcmp(cmd->args[0], "exit"))
		return (ft_exit(cmd, 1), 1);
	else if (!ft_strcmp(cmd->args[0], "unset"))
		return (ft_unset(cmd, 1), 1);
	else if (!ft_strcmp(cmd->args[0], "cd"))
		return (ft_cd(cmd, 1), 1);
	else if (!ft_strcmp(cmd->args[0], "export"))
		return (ft_export(cmd), 1);
	return (0);
}
