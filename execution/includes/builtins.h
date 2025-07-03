
#ifndef BUILTINS_H
# define BUILTINS_H

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

void							ft_echo(t_cmd *cmd, pid_t pid);
void							ft_env(t_env *env, pid_t pid);
void							ft_pwd(pid_t pid);
void							ft_unset(t_cmd *cmd, pid_t pid);
int								ft_exit(t_cmd *cmd, pid_t pid);
int								ft_cd(t_cmd *cmd, pid_t pid);
int								ft_export(t_cmd *cmd);

void							add_env(t_env **head, char *key, char *value);
void							print_export_list(t_env *env);
void							sort_env_list(t_env *env);
int								env_key_exists(t_env *env, char *key);
int								check_valid_key(char *key);
t_env							*copy_env_list(t_env *env);
void							error_display(t_cmd *cmd);
int								check_for_child(pid_t pid, int exit_stat);
char							*cd_error(t_cmd *cmd, pid_t pid);
int								export_error_display(t_cmd *cmd, char *key,
									char *value, int *i);
int								export_null(int *i, t_cmd *cmd);
void							export_args_handle(t_cmd *cmd, char *key,
									char *value);
void							exit_norm(t_cmd *cmd, int *exit_status,
									pid_t pid);
int								change_dir(char *path, pid_t pid);
void							change_dir_norm(char *old_pwd);
void							redic_error(t_redirection *redirection);
void							export_not_valid(char *tmp, int *i,
									char *value, char *key);
long long						num(t_cmd *cmd, size_t min_limit, int last);
void							num_norm(t_cmd *cmd, int *i, int *sign);
int								check_for_doubles(char *args);

#endif