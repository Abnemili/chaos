/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abnemili <abnemili@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 14:26:24 by abnemili          #+#    #+#             */
/*   Updated: 2025/07/24 12:57:44 by abnemili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int is_valid_var_char(char c) {
    return (ft_isalnum(c) || c == '_');
}

char *realloc_result(char *result, int *max_size, int needed) {
    char *new_result;

    if (needed <= *max_size)
        return (result);
    
    *max_size = needed + 100;
    new_result = realloc(result, *max_size);
    if (!new_result) {
        free(result);
        return (NULL);
    }
    return (new_result);
}

int copy_var_value(char **res, int *len, int *max, char *val) {
    int vlen;
    char *temp;

    if (!val)
        val = "";
    
    vlen = strlen(val);
    temp = realloc_result(*res, max, *len + vlen + 1);
    if (!temp)
        return (0);
    
    *res = temp;
    strcpy(*res + *len, val);
    *len += vlen;
    return (1);
}