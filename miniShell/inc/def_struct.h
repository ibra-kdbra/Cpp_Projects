#ifndef DEF_STRUCT_H
# define DEF_STRUCT_H

typedef struct s_env
{
	char			*var_name;
	char			*var_value;
	struct s_env	*next;
	struct s_env	*prev;
}	t_env;

extern int	g_exit_status;

typedef struct s_exec
{
	char	***cmds;
	int		**toks;
	char	**cmds_old;
	int		*tok_old;
	int		heredoc_fd[16][2];
	int		pipe_fd[2][2];
	int		cmd_cnt;
	int		*cpid;
	t_env	**env;
}	t_exec;

typedef enum e_token
{
	CMD_NAME = 1,
	CMD_ARG,
	R_IN_SIG,
	R_IN_FILE,
	HEREDOC_SIG,
	HEREDOC_LIM,
	R_OUT_SIG,
	R_OUT_FILE,
	R_APP_SIG,
	R_APP_FILE,
	PIPE,
	EMPTY,
}	t_token;

typedef enum e_sig_state
{
	SIG_DEFAULT = 1,
	SIG_HEREDOC,
	SIG_CHILD,
	SIG_PARENT
}	t_sig_state;

#endif
