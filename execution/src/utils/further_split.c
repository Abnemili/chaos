/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   further_split.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 15:03:59 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 18:19:08 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/expander.h"

int	is_charset(char c, char *charset)
{
	int	i;

	i = 0;
	while (charset[i])
	{
		if (c == charset[i])
			return (1);
		i++;
	}
	return (0);
}

int	count_words(char *s, char *charset)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		if (is_charset(s[i], charset))
			i++;
		else
		{
			count++;
			while (s[i] && !is_charset(s[i], charset))
				i++;
		}
	}
	return (count);
}

char	**further_split(char *s, char *charset)
{
	char	**res;
	int		count;
	int		i;
	int		j;

	count = count_words(s, charset);
	res = (char **)ft_malloc(sizeof(char *) * (count + 1));
	if (!res)
		return (NULL);
	res[count] = NULL;
	init_vars(&i, &j, &count);
	while (s[i])
	{
		if (is_charset(s[i], charset))
			i++;
		else
		{
			j = i;
			while (s[i] && !is_charset(s[i], charset))
				i++;
			res[count] = ft_substr(s, j, i - j);
			count++;
		}
	}
	return (res);
}
