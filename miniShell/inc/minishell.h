#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>

# include "def_struct.h"
# include "../src/libft/libft.h"

# define NC		"\033[0m"
# define CYAN	"\033[1;36m"
# define GREEN	"\033[1;32m"
# define RED	"\033[1;31m"
# define BOLD	"\033[1m"

/* Builtins */
void	ft_cd(char **args, t_env *env);
void	ft_echo(char **args);
void	ft_env(t_env *env, char **args);
void	ft_export(t_env **env, char **args);
void	ft_pwd(void);
void	ft_unset(t_env **env, char **args);
void	ft_exit(char **args, t_exec *exec);

/* Function for envlst linked list */
t_env	*get_env_variables(char **envp);
t_env	*ft_envlst_new(char *str);
void	ft_envlstadd_back(t_env **env, t_env *new_env);
void	ft_envlst_clear(t_env **env);
void	ft_envlst_clearone(t_env **root, t_env *env);
t_env	*ft_envlst_search(t_env *env, char *searched);
int		ft_envlst_size(t_env *env);

/* Parsing */
char	*parse_operator(char *cmd);
bool	parse_line(char *line);
bool	check_empty_line(char *line);
void	chg_qte_state(int *state, int c);
char	**args_split(char *cmd);
int		*tokenise_line(char **cmd);
void	expand_env_var(char **cmd, int *cmd_type);
void	expand_doll(char **cmd, int *tokens, t_env *env);
void	remove_quote(char **cmd);
void	remove_quote_in_tok(char **cmd);
bool	check_tokens(int *cmd_type);

void	chg_qte_state(int *state, int c);

# define E_OPERATOR_PARSE "minishell:\
	syntax error near unexpected token `%c%c'\n"

/* Signals */
void	handle_sig(int state);

/* Prompt */
void	get_prompt(t_env *env);

/* Utils */
bool	is_pipe(char c);
bool	is_redirection(char c);
bool	is_space(char c);
char	**env_lst_to_array_name(t_env *env);
char	**env_lst_to_array_value(t_env *env);

#endif
