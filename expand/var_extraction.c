#include "minishell.h"

char *extract_var_name(char *str, int start, int *end) 
{
    int i;

    if (!str || !end)
        return (NULL);
    
    i = start;
    if (str[i] == '?') {
        *end = i + 1;
        return (ft_strdup("?"));
    }
    
    while (str[i] && is_valid_var_char(str[i]))
        i++;
    
    *end = i;
    if (i == start)
        return (NULL);
    
    return (ft_strndup(str + start, i - start));
}

char *expand_exit_status(int exit_code) {
    return (ft_itoa(exit_code));
}

int handle_special_var(char *name, int exit_code, char **value, t_env *env_list) {
    if (!name || !value)
        return (0);
    
    if (strcmp(name, "?") == 0) {
        *value = expand_exit_status(exit_code);
        return (1);
    }
    
    *value = get_env_value(env_list, name);
    return (0);
}
