
#ifndef UTILS_H
# define UTILS_H

# include "minishell.h"

typedef struct s_token			t_token;
typedef struct s_cmd			t_cmd;
typedef struct s_env			t_env;
typedef struct s_redirection	t_redirection;
typedef struct s_parser			t_parser;
typedef struct s_expand			t_expand;
typedef struct s_alloc			t_alloc;
typedef enum s_type				t_type;
typedef enum perms				t_perms;

int								handle_regular_characters(char *command, int i,
									int *l, int *tmp);
void							check_init(size_t *i, int *j,
									int *k, int *digit);
int								handle_quotes(char *command, int i, int *l,
									int *tmp);
int								handle_special_characters(char *command, int i,
									int *l);
int								skip_whitespace(char *command, int i);
int								skip_quotes(char *command, int i);
int								skip_special_char(char *command, int i);
int								skip_everything_else(char *command, int i);
char							*copy_segment(char *command, int start, int end,
									int *expand_check);
void							free_tokens(t_token **head);
void							free_char_array(char **array);
void							free_cmd(t_cmd *cmd);
void							free_redirections(t_redirection *redirections);
void							free_parser(t_parser *parser);
char							*set_cwd(void);
int								*get_lastpid(void);
int								ft_isspace(char charac);
char							**further_split(char *s, char *charset);
t_env							*check_env(void);
void							env_update(t_env *env);
int								ft_atoi_shlvl(char *shlvl);
void							check_shlvl(t_env *env);
void							update_last_cmd(t_parser *parser);
void							*ft_malloc(size_t size);
void							add_alloc(void *allocated);
void							garbage_removal(void);
void							ft_free(void *ptr);
void							heredoc_signals(void);
int								undeflow_check(size_t min_limit, size_t test,
									int digit, int last);
int								overflow_check(size_t test, int digit);

#endif