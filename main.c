/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:23:16 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/26 09:45:59 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int	parse_input(t_elem *token, char *input, t_lexer *lexer)
{
	(void)lexer;
	
	if (!check_unclosed_quotes_in_input(input))
		return (0);
	if (!check_syntax(token))
		return (0);
	return (1);
}


// Updated to take env_list parameter and include syntax checking
int process_input(char *input, int *last_exit_code, t_env **env_list)
{
    t_data data = {0};
    t_lexer *lexer = NULL;
    data.env_list = *env_list;

    if (!input || !*input)
        return (1);
    
    lexer = init_lexer(input);
    if (!lexer)
        return (0);
    
    data.elem = init_tokens(lexer);
    if (!data.elem)
    {
        free(lexer);
        return (0);
    }
    
    merge_adjacent_word_tokens(&data.elem);
    
    // ADDED: Syntax checking before expansion and execution
    if (!parse_input(data.elem, input, lexer))
    {
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    
    // Expand tokens after syntax validation
    expand_tokens(data.elem, *last_exit_code, data.env_list);
    
    if (!parse_pipeline(&data))
    {
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    
    // Set the exit status pointer in data for signal handlers
    data.exit_status = *last_exit_code;
    
    // Updated: Pass env_list to execute_pipeline
    *last_exit_code = execute_pipeline(&data, env_list);
    cleanup_resources(&data, lexer, NULL);
    return (1);
}

int main(int argc, char **argv, char **envp)
{
    char *input;
    int last_exit_code = 0;
    t_env *env_list;  // Added: Local environment list
    
    (void)argc;
    (void)argv;
    
    // Updated: Initialize environment and store the returned list
    env_list = init_env_list(envp);
    if (!env_list)
    {
        fprintf(stderr, "minishell: failed to initialize environment\n");
        return (1);
    }
    
    // Initialize signal handling
    handle_signals(&last_exit_code);
    
    while (1)
    {
        input = readline("minishell$ ");
        if (!input)
        {
            printf("exit\n");
            break;
        }
        if (*input)
        {
            add_history(input);
            // Updated: Pass env_list to process_input
            process_input(input, &last_exit_code, &env_list);
        }
        free(input);
    }
    
    // Updated: Free the local env_list instead of global g_envp
    free_env_list(env_list);
    return (last_exit_code);
}