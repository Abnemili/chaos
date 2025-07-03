/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cringe_norm.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/05 00:56:10 by amouhand          #+#    #+#             */
/*   Updated: 2024/08/30 19:31:35 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

void	init_vars(int *i, int *j, int *tmp)
{
	*i = 0;
	*j = 0;
	*tmp = 0;
}

void	increment_vars(int *i, int *j, int *k, int *l)
{
	if (i)
		(*i)++;
	if (j)
		(*j)++;
	if (k)
		(*k)++;
	if (l)
		(*l)++;
}

void	set_check(char *arg, int *i, int *flag)
{
	if (arg[(*i)] == '\'')
		(*flag) = 1;
	else
		(*flag) = 2;
}

void	set_and_increment(int *i, int *flag)
{
	*flag = 0;
	(*i)++;
}

void	heredoc_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
}
