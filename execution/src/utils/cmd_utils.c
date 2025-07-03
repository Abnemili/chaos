/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 22:01:37 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/25 17:30:00 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

int	skip_whitespace(char *command, int i)
{
	while (ft_isspace(command[i]))
		i++;
	return (i);
}

int	handle_special_characters(char *command, int i, int *l)
{
	(*l)++;
	if (command[i + 1] == command[i] && command[i] != '|')
		i += 2;
	else
		i++;
	return (i);
}

int	handle_quotes(char *command, int i, int *l, int *tmp)
{
	if (!i || (command[i - 1] != '\'' && command[i - 1] != '\"'))
		(*l)++;
	while (command[i])
	{
		if (command[i] == '\"' || command[i] == '\'')
		{
			*tmp = command[i];
			i++;
			while (command[i] && command[i] != *tmp)
				i++;
			if (command[i] == *tmp)
				i++;
		}
		while (command[i] && command[i] != '|' && command[i] != '<'
			&& command[i] != '>' && command[i] != ' ' && command[i] != '\t'
			&& command[i] != '\n' && command[i] != '\"' && command[i] != '\'')
			i++;
		if (command[i] != '\'' && command[i] != '\"')
			break ;
	}
	return (i);
}

int	handle_regular_characters(char *command, int i, int *l, int *tmp)
{
	if (!i || command[i - 1] != *tmp)
		(*l)++;
	while (command[i])
	{
		while (command[i] && command[i] != '|' && command[i] != '<'
			&& command[i] != '>' && command[i] != ' ' && command[i] != '\t'
			&& command[i] != '\n' && command[i] != '\"' && command[i] != '\'')
			i++;
		if (command[i] == '\"' || command[i] == '\'')
		{
			*tmp = command[i];
			i++;
			while (command[i] && command[i] != *tmp)
				i++;
			if (command[i] == *tmp)
				i++;
		}
		if (command[i] == '|' || command[i] == '<' || command[i] == '>'
			|| command[i] == ' ' || command[i] == '\t' || command[i] == '\n')
			break ;
	}
	return (i);
}

int	unclosed_quotes(char *command)
{
	int	i;
	int	sq;
	int	dq;

	i = 0;
	sq = 0;
	dq = 0;
	while (command[i])
	{
		if (command[i] == '\'' && !dq)
			sq = !sq;
		if (command[i] == '\"' && !sq)
			dq = !dq;
		i++;
	}
	if (sq || dq)
		return (1);
	return (0);
}
