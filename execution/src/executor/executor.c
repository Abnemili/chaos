
#include "../../include/executor.h"

int	execute_commands(t_parser *parser)
{
	if (count_all_commands(parser->cmd) == 1)
	{
		update_last_cmd(get_parser());
		exec_one_command(parser->cmd[0], parser);
	}
	else
	{
		update_last_cmd(get_parser());
		create_pipe(parser->cmd, count_all_commands(parser->cmd));
	}
	return (0);
}
