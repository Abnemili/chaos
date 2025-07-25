/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_token.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/03 14:35:36 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/24 14:09:39 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int process_regular_char(char *content, int *i, t_expand_data *data) {
    char *temp;
    
    if (!content || !i || !data || !data->res || !data->len || !data->max)
        return (0);
    
    temp = realloc_result(*(data->res), data->max, *(data->len) + 2);
    if (!temp)
        return (0);
    
    *(data->res) = temp;
    (*(data->res))[*(data->len)] = content[*i];
    (*(data->len))++;
    (*i)++;
    return (1);
}

int process_dollar_expansion(char *content, int *i, t_expand_data *data) {
    int var_end;
    char *name;
    char *value;
    int is_special;

    if (!content || !i || !data)
        return (0);

    name = extract_var_name(content, *i, &var_end);
    if (name) {
        is_special = handle_special_var(name, data->exit_code, &value, data->env_list);
        if (!copy_var_value(data->res, data->len, data->max, value)) {
            cleanup_var_expansion(name, value, is_special);
            return (0);
        }
        cleanup_var_expansion(name, value, is_special);
        *i = var_end;
        return (1);
    }
    
    if (!process_regular_char("$", &(int){0}, data))
        return (0);
    return (1);
}

int process_expansion_loop(char *content, t_expand_data *data) {
    int i;

    if (!content || !data)
        return (0);
    
    i = 0;
    while (content[i]) {
        if (content[i] == '$') {
            i++;
            if (!process_dollar_expansion(content, &i, data))
                return (0);
        }
        else if (!process_regular_char(content, &i, data))
            return (0);
    }
    return (1);
}

char *expand_token_content(char *content, int exit_code, int should_expand, t_env *env_list) {
    char *res;
    int len;
    int max;
    t_expand_data data;

    if (!content)
        return (NULL);
    
    if (!should_expand)
        return (ft_strdup(content));
    
    len = 0;
    max = 1024;
    res = malloc(max);
    if (!res)
        return (NULL);
    
    data.res = &res;
    data.len = &len;
    data.max = &max;
    data.exit_code = exit_code;
    data.env_list = env_list;
    
    if (!process_expansion_loop(content, &data)) {
        free(res);
        return (NULL);
    }
    
    res[len] = '\0';
    return (res);
}