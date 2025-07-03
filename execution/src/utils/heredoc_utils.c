/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/03 23:19:47 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 17:25:28 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*ft_strjoin_free(char *s1, char *s2, int flag)
{
	char	*tmp;

	tmp = ft_strjoin(s1, s2);
	if (s1 && s2 && flag == 2)
	{
		ft_free(s1);
		s1 = NULL;
		ft_free(s2);
		s2 = NULL;
	}
	else if (s1 && flag == 1)
	{
		ft_free(s1);
		s1 = NULL;
	}
	else if (s2 && !flag)
	{
		ft_free(s2);
		s2 = NULL;
	}
	return (tmp);
}

char	*expand_heredoc(char *line, int flag)
{
	int		new_length;
	char	*result;

	new_length = calculate_new_length(line, flag);
	result = ft_malloc(sizeof(char) * (new_length + 1));
	if (!result)
		return (NULL);
	fill_heredoc(line, flag, result);
	return (result);
}

int	calculate_new_length(char *line, int flag)
{
	int		i;
	int		j;
	char	*tmp;
	char	*tmp2;

	i = 0;
	j = 0;
	while (line[i])
	{
		if (line[i] == '$' && !flag)
		{
			tmp = ft_substr(line, i, to_space(line, i));
			i += to_space(line, i);
			tmp2 = expand_env(tmp, 0);
			j += ft_strlen(tmp2);
			ft_free(tmp2);
			ft_free(tmp);
		}
		else
		{
			j++;
			i++;
		}
	}
	return (j);
}

void	fill_heredoc(char *line, int flag, char *result)
{
	int		i;
	int		j;
	char	*tmp;
	char	*tmp2;

	i = 0;
	j = 0;
	while (line[i])
	{
		if (line[i] == '$' && !flag)
		{
			tmp = ft_substr(line, i, to_space(line, i) - i);
			tmp2 = expand_env(tmp, 0);
			i = to_space(line, i);
			ft_free(tmp);
			ft_strlcpy(result + j, tmp2, ft_strlen(tmp2) + 1);
			j += ft_strlen(tmp2);
			ft_free(tmp2);
		}
		else
			result[j++] = line[i++];
	}
	result[j] = '\0';
}

int	to_space(char *line, int i)
{
	i++;
	while (line[i] && line[i] != ' ' && line[i] != '\t' && line[i] != '\n'
		&& line[i] != '$' && line[i] != '\'' && line[i] != '\"')
		i++;
	return (i);
}
