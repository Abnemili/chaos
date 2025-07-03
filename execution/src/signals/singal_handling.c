
#include "../../include/signals.h"

void	sigint_handler(int signo)
{
	get_parser()->exit_status = 130;
	if (signo == SIGINT && g_sigchild)
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		g_sigchild = 0;
	}
	else
	{
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	sigquit_handler(int signo)
{
	get_parser()->exit_status = 131;
	if (signo == SIGQUIT && g_sigchild)
	{
		printf("Quit (core dumped)\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		g_sigchild = 0;
	}
	else
	{
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
}
