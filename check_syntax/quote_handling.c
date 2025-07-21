/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 14:17:58 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 14:25:30 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	update_quote_state(enum e_type type, enum e_state *state)
{
	if (type == QUOTE && *state != IN_DQUOTE)
		*state = (*state == IN_QUOTE) ? GENERAL : IN_QUOTE;
	else if (type == DQUOTE && *state != IN_QUOTE)
		*state = (*state == IN_DQUOTE) ? GENERAL : IN_DQUOTE;
	return (1);
}

int	check_unclosed_quotes_in_input(const char *input)
{
	int		i;
	char	in_quote;

	i = 0;
	in_quote = 0;
	while (input[i])
	{
		if (input[i] == '\'' && in_quote != '"')
		{
			if (in_quote == '\'')
				in_quote = 0;
			else
				in_quote = '\'';
		}
		else if (input[i] == '"' && in_quote != '\'')
		{
			if (in_quote == '"')
				in_quote = 0;
			else
				in_quote = '"';
		}
		i++;
	}
	if (in_quote != 0)
	{
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
		return (0);
	}
	return (1);
}