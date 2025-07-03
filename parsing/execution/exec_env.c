/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 14:40:47 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 13:50:56 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern t_env	*g_envp;

t_env			*g_envp = NULL;

t_env	*create_env_node(const char *env)
{
	t_env	*node;
	char	*sep;

	node = malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	sep = strchr(env, '=');
	if (!sep)
		return (free(node), NULL);
	node->name = ft_strndup(env, sep - env);
	if (!node->name)
		return (free(node), NULL);
	node->value = strdup(sep + 1);
	if (!node->value)
		return (free(node->name), free(node), NULL);
	node->next = NULL;
	return (node);
}

void	add_env_back(t_env **lst, t_env *new_node)
{
	t_env	*temp;

	if (!lst || !new_node)
		return ;
	if (!*lst)
	{
		*lst = new_node;
		return ;
	}
	temp = *lst;
	while (temp->next)
		temp = temp->next;
	temp->next = new_node;
}

void	init_env_list(char **envp)
{
	int		i;
	t_env	*node;

	i = 0;
	while (envp[i])
	{
		node = create_env_node(envp[i]);
		if (node)
			add_env_back(&g_envp, node);
		i++;
	}
}

void	free_env_list(t_env *env)
{
	t_env	*tmp;

	while (env)
	{
		tmp = env;
		env = env->next;
		free(tmp->name);
		free(tmp->value);
		free(tmp);
	}
}
