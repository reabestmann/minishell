/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:50:53 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/02 13:47:49 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif
/* External library headers */
# include <stdlib.h>     // malloc, free, exit, getenv
# include <unistd.h>     // write, read, fork, execve, 
						//access, dup, dup2, pipe, chdir
# include <stdio.h>      // printf, perror
# include <string.h>     // strerror, strcmp, etc.
# include <fcntl.h>      // open, O_RDONLY, O_WRONLY
# include <errno.h>      // errno
# include <signal.h>     // signal, sigaction, sigemptyset, sigaddset, kill
# include <sys/types.h>  // wait, waitpid, opendir, etc.
# include <sys/wait.h>   // wait, waitpid, wait3, wait4
# include <dirent.h>     // opendir, readdir, closedir
# include <sys/stat.h>   // stat, lstat, fstat
# include <termios.h>    // tcgetattr, tcsetattr
# include <termcap.h>    // tgetent, tgetstr, tputs, etc.
# include <curses.h>     // needed for termcap
# include <readline/readline.h>  // readline
# include <readline/history.h>   // add_history, rl_* functions

/* Our own headers */
# include "libft.h"
# include "structs.h"

#define HEREDOC_TMP ".heredoc_tmp"

extern volatile sig_atomic_t	g_sigint_received;

/* parser/
 * lexer.c */
t_token		*lexer(const char *input);
/*expand.c*/
char		*append_normal_text(char *text, char *result);
/* parse.c */
int			handle_input(char *input, t_env **env, int status);
t_command	*parser(t_token *tokens);
/* parse_utils.c */
void		update_quotes(char c, int *in_quote, int *in_dquote);
int			skip_spaces(const char *str, int i);
void		set_cmd_flags(t_command *cmd);
int			has_dollar(char **args);
/* quote_state.c */
void		set_state_str(const char *str, char *state);
char		*copy_squote(int *i, char *arg, char *result);
char		*set_state(char *arg, int *i, char *result, char *state);
/* syntax.c */
int			syntax_valid(t_token *tokens);
/* expand.c */
char	*expand_arg_keep_quotes(char *arg, t_env *head, int last_status);
void		dollar_expansion(t_command *cmd, t_env **head, int last_status);
/* expand_utils.c */
char		*append_key_value(t_env *head, char *arg, int *i, char *result);
int			is_valid_key(char *arg, int i, int *len);
char		*append_result(t_env *head, char *key, char *result,
				int last_status);
char		*find_env_value(t_env *head, char *key);
char		*append_normal_text(char *text, char *result);
/* executor/ 
 * exec.c */
void		execute(char **args, char **envp);
int			run_command(t_command *cmds, t_env **env, int status);
int			run_builtin(t_command *cmd, t_env **env, int status);
void		trim_empty_args(char **args);
char		*find_path(char *cmd, char **envp);
/* pipes.c */
void		run_child(t_command *cmd, t_env **env, int status);
int			run_pipeline(t_command *cmds, t_env **env, int status);
/* redirections.c */
void		parse_redirection(t_command *cmd, t_token **cpy);
void		apply_redirections(t_command *cmd, t_env **env, int last_status);
void		fd_check(int fd, int std_fd, char *file);
/* heredoc.c */
int			apply_heredocs(t_command *cmd, int last_status);
int			collect_heredocs(t_command *cmds, int status);
/*redir_heredoc1.c */
char		*get_trimmed_delimiter(const char *delimiter);
int			delimiter_was_quoted(const char *delimiter);
void		add_heredoc(t_command *cmd, const char *delimiter);
/*redir_heredoc2.c*/
char		*expand_for_heredoc(char *line, int last_status);
/*redir_utils.c*/
void		fd_check(int fd, int std_fd, char *file);
int			dol_q_expansion(char *line, int *i, int last_status, char **result);
/* ptr_to_struct.c */
t_env		*envp_to_struct(char **envp);
/* struct_to_ptr.c */
char		**struct_to_envp(t_env *head, int export_only);
/* builtins/
 * cd_cmd.c */
int	is_quoted(char *arg);
int			too_many_args(t_command *cmd);
int			cd_cmd(t_command *cmd, t_env **env);
/*cd_cmd_utils.c*/
char		*get_env_value(t_env **env, const char *key);
void		add_pwd_node(t_env **paths, char *pwd, char *old_or_new);
void		update_oldpwd(t_env **env, char *oldpwd);
void		update_newpwd(t_env **env, char *new_pwd);
char		*expand_home(t_env **env, char *target_dir);
/* echo_cmd.c */
int			echo_cmd(t_command *cmd);
/* pwd_cmd.c */
int			pwd_cmd(t_env **env);
/* env_cmd.c */
int			env_cmd(t_command *cmd, t_env **env);
/* exit_cmd.c */
int			exit_cmd(t_command *cmd, t_env **env, int last_status);
/* export_cmd.c */
int			export_cmd(t_command *cmd, t_env **env);
/*export utils*/
int			setting_value(char **equals, t_env **new_node);
void		free_keys(char *key_one, char *key_two);
void		setting_vars(char **path, char **equals, t_env **new_node);
/*export_cmd_keys.c*/
int			is_valid_export_key(char *arg, int *len);
int			setting_key(char **path, char **equals, t_env **new_node);
char		*find_key(char *path, int *key_len);
int			validate_and_get_key(char *path, int *key_len, char **key);
int			key_info(char **envp, int j, char **key_one, char **key_two);
/*export_cmd_bubble_sort*/
void		ft_bubble_sort(char **envp);
/* unset_cmd.c */
int			unset_cmd(t_command *cmd, t_env **env);
/* utils/
 * free.c */
void		free_array(char **array);
void		free_commands(t_command *cmds);
void		free_tokens(t_token *tokens);
void		free_env_struct(t_env *head);
void		free_val(int *last_status, char **val);
/* utils.c */
char		*get_next_line(int fd);
int			get_exit_status(int status);
int			str_equals(const char *str, const char *target);
void		*handle_malloc(size_t bytes);
/* error.c */
void		error(const char *msg);
void		exec_error_custom(const char *cmd, const char *msg, int status);
void		exec_error(const char *msg, int status);
/* trim_quotes */
void		trim_quotes_for_execution(char **args);
char		*remove_quotes(char *arg);
/* signals/
 * parent_child_setup.c */
void		child_signal_setup(void);
void		parent_signal_setup(void);
/* signal_handler.c */
void		disable_ctrl_echo(void);
void		enable_ctrl_echo(void);
void		init_signals(void);

#endif
