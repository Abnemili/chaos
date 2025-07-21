/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 20:23:16 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/11 13:32:22 by abnemili         ###   ########.fr       */
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

int main(int ac, char **av, char **env)
{
    char *input;
    int last_exit_code = 0;

    (void)av;
    (void)ac;

    // Initialize environment list
    init_env_list(env);
    // done
    while (1)
    {
        input = readline("minishell $> ");
        if (!input)
        {
            printf("exit\n");
            break;
        }

        // Process the input
        if (process_input(input, &last_exit_code))
            add_history(input);
        free(input);
    }
    return (0);
}
