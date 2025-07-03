/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 18:27:56 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 19:17:58 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*quotes_in_heredoc(char *arg)
{
	char	*result;
	int		new_length;

	new_length = calculate_length(arg);
	result = ft_malloc(sizeof(char) * (new_length + 1));
	if (!result)
		return (NULL);
	fill_result(arg, result);
	return (result);
}

int	calculate_length(char *arg)
{
	int	i;
	int	j;
	int	tmp;

	i = 0;
	j = 0;
	while (arg[i])
	{
		if (arg[i] == '\"' || arg[i] == '\'')
		{
			j++;
			tmp = arg[i++];
			while (arg[i] && arg[i] != tmp)
				i++;
			if (arg[i] == tmp)
				j++;
		}
		i++;
	}
	return (i - j);
}

void	fill_result(char *arg, char *result)
{
	int	i;
	int	j;
	int	tmp;

	i = 0;
	j = 0;
	while (arg[i])
	{
		if (arg[i] == '\"' || arg[i] == '\'')
		{
			tmp = arg[i++];
			while (arg[i] && arg[i] != tmp)
				result[j++] = arg[i++];
			if (arg[i] == tmp)
				i++;
		}
		else
			result[j++] = arg[i++];
	}
	result[j] = '\0';
}

unsigned long	hash_djb2(const char *str)
{
	unsigned long	hash;
	int				c;

	hash = 5381;
	c = *str++;
	while (c)
	{
		c = *str++;
		hash = ((hash << 5) + hash) + c;
	}
	return (hash);
}

char	*generate_temp_filename(char *arg)
{
	char	*name;

	name = ft_itoa(hash_djb2(arg));
	name = ft_strjoin_free("/tmp/", name, 0);
	name = ft_strjoin_free(name, ".tmp", 1);
	return (name);
}
