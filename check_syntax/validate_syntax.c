/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 10:25:16 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/24 14:05:53 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	validate_pipe(t_elem *prev_significant)
{
	if (!prev_significant || prev_significant->type == PIPE_LINE
		|| is_redirection(prev_significant->type))
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int	validate_redirection(t_elem *curr)
{
	t_elem	*next;

	next = skip_whitespace(curr->next);
	if (!next)
	{
		printf("minishell: syntax error near unexpected token `newline'\n");
		return (0);
	}
	if (next->type == PIPE_LINE || is_redirection(next->type))
	{
		printf("minishell: syntax error near unexpected token `%s'\n",
			get_redirection_symbol(next->type));
		return (0);
	}
	return (1);
}
