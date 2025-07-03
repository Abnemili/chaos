/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cringe_norm2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/31 15:13:05 by amouhand          #+#    #+#             */
/*   Updated: 2024/09/01 10:40:26 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

void	check_init(size_t *i, int *j, int *k, int *digit)
{
	*i = 0;
	*j = 0;
	*k = 1;
	*digit = 0;
}

int	str_index(const char *str, int ch)
{
	int	index;

	index = 0;
	while (str[index])
	{
		if (str[index] == ch)
			return (index);
		index++;
	}
	return (-1);
}
