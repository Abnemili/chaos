
#include "../../include/executor.h"

char	**convert_path_to_array(t_env *env)
{
	char	**paths;
	t_env	*tmp;
	int		i;

	tmp = env;
	i = get_size(tmp);
	paths = (char **)ft_malloc(sizeof(char *) * (i + 1));
	if (!paths)
		return (NULL);
	tmp = env;
	i = 0;
	convert_path_to_arr_norm(tmp, paths, &i);
	paths[i] = NULL;
	return (paths);
}

void	redic_error(t_redirection *redirection)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(redirection->arg, 2);
	if (!access(redirection->arg, F_OK) && access(redirection->arg, X_OK) == -1)
		ft_putstr_fd(": Permission denied\n", 2);
	else
		ft_putstr_fd(": No such file or directory\n", 2);
}

char	*get_valid_path(t_cmd *cmd, pid_t pid)
{
	char	*path;

	path = cd_error(cmd, pid);
	if (!ft_strcmp(cmd->args[1], "~") || cmd->count == 1)
	{
		path = get_value("HOME", get_parser()->env);
		if (!path)
		{
			ft_putstr_fd("cd: Error: HOME not set\n", 2);
			get_parser()->exit_status = 1;
			check_for_child(pid, 1);
			return (NULL);
		}
	}
	return (path);
}

void	update_pwd(t_env *env)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	add_alloc(pwd);
	if (get_value("PWD", env))
	{
		ft_free(get_value("PWD", env));
		edit_env(env, "PWD", ft_strdup(pwd));
	}
	ft_free(pwd);
}

int	key_len(char *str)
{
	int	i;

	i = 0;
	while (str[i] && (str[i] != '=' && !(str[i] == '+' && str[i + 1] == '=')))
		i++;
	return (i);
}
