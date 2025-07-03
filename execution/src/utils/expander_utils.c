/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 17:20:01 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 17:20:40 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/expander.h"

char	*joining(char *to_free, char *before, char *expand, char *after)
{
	char	*result;

	ft_free(to_free);
	result = NULL;
	if (before)
		result = ft_strjoin_free(result, before, 0);
	if (expand)
		result = ft_strjoin_free(result, expand, 2);
	if (after)
		result = ft_strjoin_free(result, after, 2);
	return (result);
}

void	free_expand_list(void)
{
	t_expand	*expand;
	t_expand	*tmp;
	t_expand	*tmp2;

	expand = get_parser()->expand;
	tmp2 = expand;
	while (tmp2)
	{
		tmp = tmp2;
		tmp2 = tmp2->next;
		if (tmp)
		{
			if (tmp->content)
			{
				ft_free(tmp->content);
				tmp->content = NULL;
			}
			ft_free(tmp);
			tmp = NULL;
		}
	}
	get_parser()->expand = NULL;
}

int	space_available(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (ft_isspace(s[i]))
			return (1);
		i++;
	}
	return (0);
}
