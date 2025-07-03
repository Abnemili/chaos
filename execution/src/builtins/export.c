
#include "../../include/expander.h"

int	export_error_display(t_cmd *cmd, char *key, char *value, int *i)
{
	char	*tmp;

	tmp = NULL;
	if (!check_valid_key(key))
	{
		export_not_valid(tmp, i, value, key);
		return (1);
	}
	if (str_index(cmd->args[*i], '=')
		&& cmd->args[*i][str_index(cmd->args[*i], '=') - 1] == '+'
		&& env_key_exists(get_parser()->env, key))
		edit_env(get_parser()->env, key,
			ft_strjoin_free(get_value(key, get_parser()->env), value, 1));
	else if (str_index(cmd->args[*i], '=')
		&& cmd->args[*i][str_index(cmd->args[*i], '=') - 1] == '+')
		add_env(&get_parser()->env, ft_strdup(key), ft_strdup(value));
	else if (env_key_exists(get_parser()->env, key))
		edit_env(get_parser()->env, key, ft_strdup(value));
	else
		add_env(&get_parser()->env, ft_strdup(key), ft_strdup(value));
	return (0);
}

int	export_null(int *i, t_cmd *cmd)
{
	char	*key;

	key = ft_strdup(cmd->args[*i]);
	if (!check_valid_key(key))
	{
		ft_putstr_fd("minishell: export: `", 2);
		if (ft_strlen(key))
			ft_putstr_fd(key, 2);
		else
			ft_putstr_fd("=", 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		*i += 1;
		ft_free(key);
		get_parser()->exit_status = 1;
		return (1);
	}
	if (env_key_exists(get_parser()->env, key))
	{
		if (!get_value(key, get_parser()->env))
			edit_env(get_parser()->env, ft_strdup(key), NULL);
	}
	else
		add_env(&get_parser()->env, ft_strdup(key), NULL);
	return (0);
}

void	export_args_handle(t_cmd *cmd, char *key, char *value)
{
	int	i;

	i = 1;
	while (cmd->args[i])
	{
		if (ft_strchr(cmd->args[i], '='))
		{
			value = ft_strchr(cmd->args[i], '=') + 1;
			key = ft_substr(cmd->args[i], 0, key_len(cmd->args[i]));
			if (export_error_display(cmd, key, value, &i))
				continue ;
		}
		else if (export_null(&i, cmd))
			continue ;
		i++;
		ft_free(key);
	}
	check_for_child(cmd->pid, get_parser()->exit_status);
}

int	ft_export(t_cmd *cmd)
{
	char	*key;
	char	*value;

	key = NULL;
	value = NULL;
	if (cmd->count == 1)
		print_export_list(get_parser()->env);
	else
		export_args_handle(cmd, key, value);
	return (1);
}

void	print_export_list(t_env *env)
{
	t_env	*tmp;
	t_env	*to_free;

	tmp = copy_env_list(env);
	if (!tmp)
		return ;
	sort_env_list(tmp);
	to_free = tmp;
	while (tmp)
	{
		ft_putstr_fd("declare -x ", 1);
		ft_putstr_fd(tmp->key, 1);
		if (tmp->value)
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(tmp->value, 1);
			ft_putchar_fd('\"', 1);
		}
		ft_putchar_fd('\n', 1);
		tmp = tmp->next;
	}
	free_env(to_free);
}
