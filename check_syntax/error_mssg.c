/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_error.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 23:35:03 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 14:18:01 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_redirection_symbol(enum e_type type)
{
	if (type == REDIR_IN)
		return ("<");
	else if (type == REDIR_OUT)
		return (">");
	else if (type == DREDIR_OUT)
		return (">>");
	else if (type == HERE_DOC)
		return ("<<");
	return ("unknown");
}

char	*get_sredir_error(t_elem *curr)
{
	t_elem	*next;
	t_elem	*third;

	next = skip_whitespace(curr->next);
	if (!next || next->type != curr->type)
		return ("newline");
	
	third = skip_whitespace(next->next);
	if (third && third->type == curr->type)
		return (curr->type == REDIR_OUT ? ">" : "<");
	return ("newline");
}

char	*get_dredir_error(t_elem *curr)
{
	t_elem	*next;

	next = skip_whitespace(curr->next);
	if (!next || !is_redirection(next->type))
		return ("newline");
	
	if (next->type == curr->type)
		return (curr->type == DREDIR_OUT ? ">>" : "<<");
	if (next->type == REDIR_OUT)
		return (">");
	if (next->type == REDIR_IN)
		return ("<");
	return ("newline");
}

char	*get_error_token(t_elem *curr)
{
	if (curr->type == REDIR_IN || curr->type == REDIR_OUT)
		return (get_sredir_error(curr));
	if (curr->type == HERE_DOC || curr->type == DREDIR_OUT)
		return (get_dredir_error(curr));
	return ("newline");
}