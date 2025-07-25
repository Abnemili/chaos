/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 16:02:54 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/24 14:34:29 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Function to print a single command's details
void	print_cmd_debug(t_cmd *cmd, int cmd_num)
{
	int	i;

	printf("=== Command %d ===\n", cmd_num);
	printf("Input FD: %d\n", cmd->in_file);
	printf("Output FD: %d\n", cmd->out_file);
	if (cmd->full_cmd)
	{
		printf("Arguments: ");
		i = 0;
		while (cmd->full_cmd[i])
		{
			printf("[%s]", cmd->full_cmd[i]);
			if (cmd->full_cmd[i + 1])
				printf(", ");
			i++;
		}
		printf("\n");
	}
	else
	{
		printf("Arguments: (none)\n");
	}
	printf("\n");
}

// Function to print the entire command pipeline
void	print_pipeline_debug(t_data *data)
{
	t_cmd	*current;
	int		cmd_count;

	cmd_count = 0;
	if (!data || !data->head)
	{
		printf("No commands to display (empty pipeline)\n");
		return ;
	}
	printf("\n==================== PIPELINE DEBUG ====================\n");
	current = data->head;
	while (current)
	{
		print_cmd_debug(current, cmd_count + 1);
		current = current->next;
		cmd_count++;
	}
	printf("Total commands in pipeline: %d\n", cmd_count);
	printf("========================================================\n\n");
}

// Alternative compact version for quick debugging
void	print_pipeline_compact(t_data *data)
{
	t_cmd	*current;
	int		i;

	// int cmd_num = 1;
	if (!data || !data->head)
	{
		printf("Empty pipeline\n");
		return ;
	}
	printf("Pipeline: ");
	current = data->head;
	while (current)
	{
		printf("[");
		if (current->full_cmd && current->full_cmd[0])
		{
			i = 0;
			while (current->full_cmd[i])
			{
				printf("%s", current->full_cmd[i]);
				if (current->full_cmd[i + 1])
					printf(" ");
				i++;
			}
		}
		else
		{
			printf("empty");
		}
		// Show redirections
		if (current->in_file != STDIN_FILENO)
			printf(" <fd:%d", current->in_file);
		if (current->out_file != STDOUT_FILENO)
			printf(" >fd:%d", current->out_file);
		printf("]");
		if (current->next)
			printf(" | ");
		current = current->next;
		// cmd_num++;
	}
	printf("\n");
}

// Helper function to cleanup resources


// Process input and return status (0 = error, 1 = success)
int process_input(char *input, int *last_exit_code, t_env **env_list)
{
    t_data data = {0};
    t_lexer *lexer = NULL;

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
    // Updated: Pass env_list to expand_tokens
    expand_tokens(data.elem, *last_exit_code, *env_list);
    
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