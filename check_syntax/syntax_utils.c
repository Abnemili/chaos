/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 14:17:58 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/12 20:14:05 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
int	is_quote(enum e_type type)
{
	return (type == QUOTE || type == DQUOTE);
}

int	is_redirection(enum e_type type)
{
	return (type == REDIR_IN || type == REDIR_OUT || 
			type == DREDIR_OUT || type == HERE_DOC);
}

int	is_empty(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || 
			c == '\r' || c == '\f' || c == '\v');
}

t_elem	*skip_whitespace(t_elem *token)
{
	while (token && token->type == WHITE_SPACE)
		token = token->next;
	return (token);
}