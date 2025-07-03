/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/31 18:15:19 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/31 18:15:47 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	overflow_check(size_t test, int digit)
{
	if (test > LLONG_MAX / 10 || (test == LLONG_MAX / 10
			&& (digit > LLONG_MAX % 10)))
		return (2);
	return (0);
}

int	undeflow_check(size_t min_limit, size_t test, int digit, int last)
{
	if (test > min_limit || (test == min_limit && digit > last))
		return (2);
	return (0);
}
