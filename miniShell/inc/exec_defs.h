#ifndef EXEC_DEFS_H
# define EXEC_DEFS_H

# include "def_struct.h"
# include <stdbool.h>

/* Redirections */
int		heredoc_init(int hd_arr[16][2], char ***cmds, int **toks);
void	redirection_hdl(t_exec *exec, int cmd_nb);
int		check_heredoc_use(int fd, t_exec *exec, int cmd_nb);
void	close_heredocs(int hd_arr[16][2]);
bool	stdin_state(void);
# define HD_EOF "minishell: warning: here-document delimited by end-of-file \
(wanted `%s')\n"

/* Execution */
void	main_exec(char *raw_cmd, t_env **env);
char	*get_cmd_path(char **cmd_arg, int *tokens, t_env *env);
char	**get_cmd_args(char **cmd_arg, int *tokens, t_env *env, char *cmd);
bool	check_dir(char *cmd);

char	**transform_envp(t_env *env);
int		cmd_counter(char **args, int *tokens);
char	**get_nxt_cmd(char **cmd_split, int *tokens);

void	sec_close(int fd);
void	sec_full_close(int pipe_fd[2][2], int cmd_nb);
void	free_exec(t_exec *exec, char *path, char **args);

bool	is_builtin(char **cmds, int *toks);
bool	isbuiltin_env(char **cmd, int *toks);
void	start_builtin_env(char **cmd, int *toks, t_exec *exec);
void	init_builtin_exec(char **cmds, int *toks, t_exec *exec, int cmd);

#endif
