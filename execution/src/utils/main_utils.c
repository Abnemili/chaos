/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amouhand <amouhand@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/12 13:13:38 by ybouaoud          #+#    #+#             */
/*   Updated: 2024/09/05 11:01:55 by amouhand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils.h"

int	print_error(char *msg, int code)
{
	write(2, msg, strlen(msg));
	return (code);
}

t_parser	*readfrom(char **env)
{
	t_parser	*parser;

	parser = init_parser(env);
	if (!parser)
		return (NULL);
	env_update(parser->env);
	while (1)
	{
		parser_subfree(parser);
		maintain_parser(parser);
		if (!read_input(parser))
			continue ;
		if (!parse_input(parser))
			continue ;
		if (!tokenize_input(parser))
			continue ;
		if (execute_commands(parser))
			continue ;
	}
	free_parser(parser);
	return (parser);
}

void	free_parser(t_parser *parser)
{
	garbage_removal();
	free(parser);
	parser = NULL;
	rl_clear_history();
}

void	parser_subfree(t_parser *parser)
{
	int	i;

	i = 0;
	if (parser)
	{
		if (parser->prompt)
			ft_free(parser->prompt);
		if (parser->line)
			ft_free(parser->line);
		if (parser->result)
			free_char_array(parser->result);
		if (parser->head)
			free_tokens(&parser->head);
		if (parser->cmd)
		{
			while (parser->cmd[i])
				free_cmd(parser->cmd[i++]);
			ft_free(parser->cmd);
		}
		maintain_parser(parser);
	}
}

void	maintain_parser(t_parser *parser)
{
	if (parser)
	{
		parser->prompt = NULL;
		parser->line = NULL;
		parser->result = NULL;
		parser->head = NULL;
		parser->cmd = NULL;
		parser->expand = NULL;
		parser->heredoc_abort = 0;
	}
}
