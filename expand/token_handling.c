/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 11:41:15 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/26 10:06:25 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *remove_quotes(char *content, enum e_type quote_type) {
    int len;
    char quote_char;

    if (!content)
        return (NULL);
    
    len = strlen(content);
    if (len < 2)
        return (ft_strdup(content));
    
    if (quote_type == QUOTE)
        quote_char = '\'';
    else if (quote_type == DQUOTE)
        quote_char = '"';
    else
        return (ft_strdup(content));
    
    if (content[0] == quote_char && content[len - 1] == quote_char)
        return (ft_strndup(content + 1, len - 2));
    
    return (ft_strdup(content));
}

void handle_quoted_token(t_elem *curr, int exit_code, t_env *env_list) {
    char *unquoted;
    char *expanded;
    int should_expand;

    if (!curr || !curr->content)
        return;
    
    unquoted = remove_quotes(curr->content, curr->type);
    if (!unquoted)
        return;
    
    should_expand = (curr->type == DQUOTE);
    expanded = expand_token_content(unquoted, exit_code, should_expand, env_list);
    free(unquoted);
    
    if (expanded) {
        free(curr->content);
        curr->content = expanded;
        curr->type = WORD;
    }
}

void handle_word_token(t_elem *curr, int exit_code, t_env *env_list) {
    int should_expand;
    char *exp;

    if (!curr || !curr->content)
        return;
    
    should_expand = (curr->state != IN_QUOTE);
    exp = expand_token_content(curr->content, exit_code, should_expand, env_list);
    if (exp) {
        free(curr->content);
        curr->content = exp;
        curr->type = WORD;
    }
}

void expand_tokens(t_elem *token, int exit_code, t_env *env_list) {
    t_elem *curr;

    if (!token)
        return;
    
    curr = token;
    while (curr) {
        if (curr->type == QUOTE || curr->type == DQUOTE)
            handle_quoted_token(curr, exit_code, env_list);
         else if ((curr->type == WORD || curr->type == ENV || curr->type == EXIT_STATUS)
                && curr->state != IN_QUOTE)
            handle_word_token(curr, exit_code, env_list);
        curr = curr->next;
    }
}