/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 21:45:44 by ybouaoud          #+#    #+#             */
/*   Updated: 2024/09/05 22:09:24 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

int	ft_atoi_shlvl(char *shlvl)
{
	int	i;
	int	nbr;

	i = 0;
	nbr = 0;
	while (shlvl[i])
	{
		if (!ft_isdigit(shlvl[i]))
			return (0);
		nbr = nbr * 10 + (shlvl[i] - '0');
		i++;
	}
	return (nbr);
}

void	check_shlvl(t_env *env)
{
	int	shlvl;

	shlvl = ft_atoi_shlvl(get_value("SHLVL", env));
	edit_env(env, "SHLVL", ft_itoa(shlvl + 1));
}

void	update_last_cmd(t_parser *parser)
{
	int	count;

	count = count_char_array(parser->cmd[0]->args);
	if (count_all_commands(parser->cmd) == 1
		&& count)
	{
		edit_env(parser->env, "_",
			ft_strdup(parser->cmd[0]->args[count - 1]));
	}
	else
		edit_env(parser->env, "_", NULL);
}

void	env_update(t_env *env)
{
	if (!get_value("OLDPWD", env))
		add_env(&env, "OLDPWD", NULL);
	check_shlvl(env);
}

t_env	*check_env(void)
{
	t_env	*env;

	env = (t_env *)ft_malloc(sizeof(t_env));
	env->key = ft_strdup("PWD");
	env->value = ft_strdup(getcwd(NULL, 0));
	env->next = (t_env *)ft_malloc(sizeof(t_env));
	env->next->key = ft_strdup("SHLVL");
	env->next->value = ft_strdup("0");
	env->next->next = (t_env *)ft_malloc(sizeof(t_env));
	env->next->next->key = ft_strdup("_");
	env->next->next->value = ft_strdup("");
	return (env);
}
