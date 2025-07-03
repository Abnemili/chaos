/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 14:56:25 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 18:17:43 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

int	skip_quotes(char *command, int i)
{
	int	tmp;

	while (command[i])
	{
		if (command[i] == '\"' || command[i] == '\'')
		{
			tmp = command[i];
			i++;
			while (command[i] && command[i] != tmp)
				i++;
			if (command[i] == tmp)
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

int	skip_special_char(char *command, int i)
{
	if (command[i + 1] == command[i] && command[i] != '|')
		i += 2;
	else
		i++;
	return (i);
}

int	skip_everything_else(char *command, int i)
{
	int	tmp;

	while (command[i])
	{
		while (command[i] && command[i] != '|' && command[i] != '<'
			&& command[i] != '>' && command[i] != ' ' && command[i] != '\t'
			&& command[i] != '\n' && command[i] != '\"' && command[i] != '\'')
			i++;
		if (command[i] == '\"' || command[i] == '\'')
		{
			tmp = command[i];
			i++;
			while (command[i] && command[i] != tmp)
				i++;
			if (command[i] == tmp)
				i++;
		}
		if (command[i] == '|' || command[i] == '<' || command[i] == '>'
			|| command[i] == ' ' || command[i] == '\t' || command[i] == '\n')
			break ;
	}
	return (i);
}

int	find_expand(char *command, int i)
{
	int	j;

	j = i;
	if (!command[j])
		return (0);
	while (j >= 0 && command[j])
		j--;
	j++;
	while (command[j])
	{
		if (command[j] == '$')
			return (1);
		j++;
	}
	return (0);
}

int	heredoc_arg(char *command, int i)
{
	int	j;

	j = i - 1;
	if (!i || !j)
		return (0);
	while (j >= 0 && (command[j] != ' '
			&& command[j] != '\t' && command[j] != '<'))
		j--;
	while (j >= 0 && (command[j] == ' ' || command[j] == '\t'))
		j--;
	if (j > 0 && command[j] == '<' && command[j - 1] == '<')
		return (1);
	return (0);
}
