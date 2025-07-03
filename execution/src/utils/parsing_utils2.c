/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 19:22:46 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 18:18:27 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

int	copy_quote(char *command, int start, char *result, int *l)
{
	char	quote;
	int		tmp;

	quote = command[start];
	tmp = start;
	if (find_expand(command, start) || heredoc_arg(command, start))
		result[(*l)++] = command[start++];
	else
		start++;
	while (command[start] && command[start] != quote)
		result[(*l)++] = command[start++];
	if (command[start] == quote)
	{
		if (find_expand(command, start) || heredoc_arg(command, tmp))
			result[(*l)++] = command[start++];
		else
			start++;
	}
	return (start);
}

int	should_expand(char *command, int start)
{
	if (isspecial(command, start))
		return (1);
	if (find_expand(command, start))
		return (1);
	if (heredoc_arg(command, start))
		return (1);
	return (0);
}

char	*copy_segment(char *command, int start, int end, int *expand_check)
{
	int		l;
	char	*result;

	l = 0;
	result = (char *)ft_malloc(sizeof(char) * (end - start + 1));
	while (start < end)
	{
		if (command[start] == '\"' || command[start] == '\'')
		{
			if (should_expand(command, start))
				*expand_check = 1;
			start = copy_quote(command, start, result, &l);
		}
		else
			result[l++] = command[start++];
	}
	result[l] = '\0';
	return (result);
}

int	ft_isspace(char charac)
{
	if (charac == ' ' || charac == '\t' || charac == '\n'
		|| charac == '\v' || charac == '\f'
		|| charac == '\r')
		return (1);
	return (0);
}

void	ambiguos_redirect(char *arg)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	get_parser()->exit_status = 1;
}
