/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_stuff.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/22 22:25:56 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 18:17:11 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

void	free_tokens(t_token **head)
{
	t_token	*tmp;
	t_token	*current;

	if (!head || !*head)
		return ;
	current = *head;
	while (current)
	{
		tmp = current;
		current = current->next;
		if (tmp->value)
		{
			ft_free(tmp->value);
			tmp->value = NULL;
		}
		ft_free(tmp);
		tmp = NULL;
	}
	*head = NULL;
}

void	free_char_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		if (array[i])
		{
			ft_free(array[i]);
			array[i] = NULL;
		}
		i++;
	}
	ft_free(array);
	array = NULL;
}

void	free_cmd(t_cmd *cmd)
{
	if (!cmd)
		return ;
	if (cmd->args)
		free_char_array(cmd->args);
	if (cmd->redirections)
		free_redirections(cmd->redirections);
	ft_free(cmd);
	cmd = NULL;
}

void	free_redirections(t_redirection *redirections)
{
	t_redirection	*tmp;

	while (redirections)
	{
		tmp = redirections;
		redirections = redirections->next;
		ft_free(tmp->arg);
		ft_free(tmp);
		tmp = NULL;
	}
	redirections = NULL;
}
