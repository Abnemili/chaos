
#include "../../include/builtins.h"

void	ft_pwd(pid_t pid)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	add_alloc(pwd);
	ft_putendl_fd(pwd, 1);
	ft_free(pwd);
	check_for_child(pid, 0);
}
