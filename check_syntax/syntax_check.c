/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:56:55 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/24 12:13:46 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_initial_syntax(t_elem *curr)
{
	curr = skip_whitespace(curr);
	if (!curr)
		return (1);
	if (curr->type == PIPE_LINE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int	process_token(t_elem *curr, enum e_state *state, t_elem **prev_significant)
{
	if (curr->type == WHITE_SPACE)
		return (1);
	update_quote_state(curr->type, state);
	if (*state == GENERAL)
	{
		if (curr->type == PIPE_LINE)
		{
			if (!validate_pipe(*prev_significant))
				return (0);
		}
		else if (is_redirection(curr->type))
		{
			if (!validate_redirection(curr))
				return (0);
		}
	}
	if (curr->type != WHITE_SPACE)
		*prev_significant = curr;
	return (1);
}

int	check_final_syntax(enum e_state state, t_elem *prev_significant)
{
	if (state != GENERAL)
	{
		if (state == IN_QUOTE)
			ft_putstr_fd("minishell: unexpected EOF while looking for matching `''\n",
				2);
		else
			ft_putstr_fd("minishell: unexpected EOF while looking for matching `\"'\n",
				2);
		return (0);
	}
	if (prev_significant && prev_significant->type == PIPE_LINE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int	check_syntax(t_elem *token)
{
	t_elem			*curr;
	t_elem			*prev_significant;
	enum e_state	state;

	curr = token;
	prev_significant = NULL;
	state = GENERAL;
	if (!check_initial_syntax(curr))
		return (0);
	while (curr)
	{
		if (!process_token(curr, &state, &prev_significant))
			return (0);
		curr = curr->next;
	}
	return (check_final_syntax(state, prev_significant));
}
