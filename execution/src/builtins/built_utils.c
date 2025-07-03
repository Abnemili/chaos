
#include "../../include/builtins.h"

int	check_for_child(pid_t pid, int exit_stat)
{
	if (pid == 0)
	{
		get_parser()->exit_status = exit_stat;
		free_parser(get_parser());
		exit(exit_stat);
	}
	else
		get_parser()->exit_status = exit_stat;
	return (0);
}

t_env	*copy_env_list(t_env *env)
{
	t_env	*tmp;
	t_env	*new;
	t_env	*head;

	tmp = env;
	head = NULL;
	while (tmp)
	{
		new = ft_malloc(sizeof(t_env));
		if (!new)
			return (NULL);
		new->key = ft_strdup(tmp->key);
		new->value = ft_strdup(tmp->value);
		new->next = NULL;
		if (!head)
			head = new;
		else
		{
			new->next = head;
			head = new;
		}
		tmp = tmp->next;
	}
	return (head);
}

void	sort_env_list(t_env *env)
{
	t_env	*tmp;
	t_env	*tmp2;
	char	*temp;

	tmp = env;
	while (tmp)
	{
		tmp2 = tmp->next;
		while (tmp2)
		{
			if (ft_strcmp(tmp->key, tmp2->key) > 0)
			{
				temp = tmp->key;
				tmp->key = tmp2->key;
				tmp2->key = temp;
				temp = tmp->value;
				tmp->value = tmp2->value;
				tmp2->value = temp;
			}
			tmp2 = tmp2->next;
		}
		tmp = tmp->next;
	}
}

int	env_key_exists(t_env *env, char *key)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (!ft_strcmp(tmp->key, key))
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

int	check_valid_key(char *key)
{
	int	i;

	i = 0;
	if (!ft_isalpha(key[i]) && key[i] != '_')
		return (0);
	i++;
	while (key[i])
	{
		if (!ft_isalnum(key[i]) && key[i] != '_')
			return (0);
		i++;
	}
	return (1);
}
