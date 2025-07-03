/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/26 16:02:54 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/01 16:19:28 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Function to print a single command's details
void print_cmd_debug(t_cmd *cmd, int cmd_num)
{
        int i;
        
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
void print_pipeline_debug(t_data *data)
{
    t_cmd *current;
    int cmd_count = 0;
    
    if (!data || !data->head)
    {
        printf("No commands to display (empty pipeline)\n");
        return;
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
void print_pipeline_compact(t_data *data)
{
    t_cmd *current;
    // int cmd_num = 1;
    int i;
    
    if (!data || !data->head)
    {
        printf("Empty pipeline\n");
        return;
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
void cleanup_resources(t_data *data, t_lexer *lexer, char *input)
{
    if (data->elem)
        free_token_list(data->elem);
    if (data->head)
        free_cmd_list(data->head);
    if (lexer)
        free_lexer(lexer);
    if (input)
        free(input);
}

// Process input and return status (0 = error, 1 = success)
int process_input(char *input, int *last_exit_code)
{
    t_data data;
    t_lexer *lexer;


    init_data(&data, input); // init data
    if (check_empty_line(&data))
        return (1); // Empty line is not an error


    lexer = init_lexer(input); // init lexer
    if (!lexer)
    {
        printf("Error: lexer initialization failed\n");
        return (0);
    }

    // Tokenize and merge the tokens quote 
    data.elem = init_tokens(lexer); // tokenising
    merge_adjacent_word_tokens(&data.elem);

    if (!parse_input(data.elem, input, lexer))
    {
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    expand_tokens(data.elem, *last_exit_code); // probably 
    
    if (!parse_pipeline(&data))
    {
        printf("Error: pipeline parsing failed\n");
        printf("DETECTED HERE IN PASE_PIPELINE MIAN.C \n");
        cleanup_resources(&data, lexer, NULL);
        return (0);
    }
    
    // Debug output (remove in production)
    printf("\n--- DEBUG: Parsed Commands ---\n");
    print_pipeline_debug(&data);

    // TODO: Execute commands and update last_exit_code
    // *last_exit_code = execute_pipeline(&data);
    *last_exit_code = 0; // Mock exit status

    // Cleanup
    cleanup_resources(&data, lexer, NULL);
    return (1);
}
