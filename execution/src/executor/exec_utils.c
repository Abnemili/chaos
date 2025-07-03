
#include "../../include/executor.h"

int	count_all_commands(t_cmd **cmds)
{
	int	i;

	i = 0;
	while (cmds[i])
		i++;
	return (i);
}

int	get_size(t_env *env)
{
	int		i;
	t_env	*tmp;

	i = 0;
	tmp = env;
	while (tmp)
	{
		i++;
		tmp = tmp->next;
	}
	return (i);
}

void	convert_path_to_arr_norm(t_env *tmp, char **paths, int *i)
{
	char	*temp;

	while (tmp)
	{
		if (tmp->value)
		{
			temp = ft_strjoin(tmp->key, "=");
			temp = ft_strjoin_free(temp, tmp->value, 1);
			paths[(*i)] = ft_strdup(temp);
			ft_free(temp);
			*i += 1;
		}
		tmp = tmp->next;
	}
}

int	test_redirection_fd(t_cmd *cmd)
{
	int				ret;
	t_redirection	*redirection;

	ret = 0;
	redirection = cmd->redirections;
	while (redirection)
	{
		if (redirection->fd == -1)
		{
			redic_error(redirection);
			ret = 1;
		}
		redirection = redirection->next;
		if (ret)
		{
			free_parser(get_parser());
			exit(1);
		}
	}
	return (0);
}

void	export_not_valid(char *tmp, int *i, char *value, char *key)
{
	tmp = ft_strjoin("=", value);
	ft_putstr_fd("minishell: export: `", 2);
	if (ft_strlen(key))
		ft_putstr_fd(key, 2);
	else
		ft_putstr_fd(tmp, 2);
	ft_free(tmp);
	ft_putstr_fd("': not a valid identifier\n", 2);
	*i += 1;
	get_parser()->exit_status = 1;
}
