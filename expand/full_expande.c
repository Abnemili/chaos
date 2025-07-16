#include "minishell.h"

extern t_env	*g_envp;

char	*get_env_value(char *name)
{
	t_env	*temp;

	temp = g_envp;
	if (!name)
		return (NULL);
	while (temp)
	{
		if (strcmp(temp->name, name) == 0)
			return (temp->value);
		temp = temp->next;
	}
	return (NULL);
}

int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

int	copy_var_value(char **res, int *len, int *max, char *val)
{
	int		vlen;
	char	*temp;

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

char	*extract_var_name(char *str, int start, int *end)
{
	int	i;

	i = start;
	if (str[i] == '?')
	{
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

char	*expand_exit_status(int exit_code)
{
	return (ft_itoa(exit_code));
}

extern t_env	*g_envp;

int	process_regular_char(char *content, int *i, t_expand_data *data)
{
	if (!(*(data->res) = realloc_result(*(data->res), data->max, *(data->len)
				+ 2)))
		return (0);
	(*(data->res))[(*(data->len))++] = content[(*i)++];
	return (1);
}

int	process_expansion_loop(char *content, t_expand_data *data)
{
	int	i;

	i = 0;
	while (content[i])
	{
		if (content[i] == '$')
		{
			i++;
			if (!process_dollar_expansion(content, &i, data))
				return (0);
		}
		else if (!process_regular_char(content, &i, data))
			return (0);
	}
	return (1);
}

char	*expand_token_content(char *content, int exit_code, int should_expand)
{
	char			*res;
	int				len;
	int				max;
	t_expand_data	data;

	len = 0;
	max = 1024;
	if (!content || !should_expand)
		return (ft_strdup(content));
	res = malloc(max);
	if (!res)
		return (NULL);
	data = (t_expand_data){&res, &len, &max, exit_code};
	if (!process_expansion_loop(content, &data))
		return (free(res), NULL);
	res[len] = '\0';
	return (res);
}
/* minishell/expand_word.c */
void    handle_word_token(t_elem *curr, int exit_code)
{
    int   should_expand;
    char *exp;

    /* NEVER expand inside a single‑quoted token */
    should_expand = (curr->state != IN_QUOTE);     /*  ✅ new rule  */

    exp = expand_token_content(curr->content, exit_code, should_expand);
    if (exp)
    {
        free(curr->content);
        curr->content = exp;
        curr->type = WORD;
        /* keep curr->state unchanged – it may still be IN_DQUOTE or GENERAL */
    }
}

void	expand_tokens(t_elem *token, int exit_code)
{
	t_elem	*curr;

	curr = token;
	while (curr)
	{
		if (curr->type == QUOTE || curr->type == DQUOTE)
			handle_quoted_token(curr, exit_code);
		else if ((curr->type == WORD || curr->type == ENV)
			&& curr->state != IN_QUOTE)
			handle_word_token(curr, exit_code);
		curr = curr->next;
	}
}


char	*remove_quotes(char *content, enum e_type quote_type)
{
	int		len;
	char	quote_char;

	if (!content)
		return (NULL);
	len = strlen(content);
	if (len < 2)
		return (ft_strdup(content));
	if (quote_type == QUOTE)
		quote_char = '\'';
	else
		quote_char = '"';
	if (content[0] == quote_char && content[len - 1] == quote_char)
		return (ft_strndup(content + 1, len - 2));
	return (ft_strdup(content));
}

void	handle_quoted_token(t_elem *curr, int exit_code)
{
	char	*unq;
	char	*exp;
	int		sh_expand;

	unq = remove_quotes(curr->content, curr->type);
	if (!unq)
		return ;
	sh_expand = (curr->type == DQUOTE);
	exp = expand_token_content(unq, exit_code, sh_expand);
	free(unq);
	if (exp)
	{
		free(curr->content);
		curr->content = exp;
		curr->type = WORD;
	}
}

int	process_dollar_expansion(char *content, int *i, t_expand_data *data)
{
	int		var_end;
	char	*name;
	char	*value;
	int		is_special;

	name = extract_var_name(content, *i, &var_end);
	if (name)
	{
		is_special = handle_special_var(name, data->exit_code, &value);
		if (!copy_var_value(data->res, data->len, data->max, value))
		{
			cleanup_var_expansion(name, value, is_special);
			return (0);
		}
		cleanup_var_expansion(name, value, is_special);
		*i = var_end;
		return (1);
	}
	if (!(*(data->res) = realloc_result(*(data->res), data->max, *(data->len)
				+ 2)))
		return (0);
	(*(data->res))[(*(data->len))++] = '$';
	return (1);
}

char	*realloc_result(char *result, int *max_size, int needed)
{
	char	*new_result;

	if (needed < *max_size)
		return (result);
	*max_size = needed + 100;
	new_result = realloc(result, *max_size);
	if (!new_result)
	{
		free(result);
		return (NULL);
	}
	return (new_result);
}
int	handle_special_var(char *name, int exit_code, char **value)
{
	if (strcmp(name, "?") == 0)
	{
		*value = expand_exit_status(exit_code);
		return (1);
	}
	*value = get_env_value(name);
	return (0);
}