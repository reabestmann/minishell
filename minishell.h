/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:50:53 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 15:05:00 by aabelkis         ###   ########.fr       */
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

# define HEREDOC_TMP ".heredoc_tmp"

extern volatile sig_atomic_t	g_sigint_received;

/* shell_utils.c*/
void		read_line(char **input);
void		init_main_vars(int *params, char **argv, t_env **env, char **envp);
void		update_last_command(t_env **env, char *last_cmd);
void		update_shlvl(t_env **env);
/* parser/
 * lexer.c */
t_token		*lexer(const char *input);
/*lexer_token_utils*/
t_token		*create_token(const char *str, t_token_type type);
void		append_token(t_token **head, t_token *new_token);
int			handle_redir_err(const char *str, t_token_type *type);
int			handle_redir_both(const char *str, t_token_type *type);
int			handle_redir_out(const char *str, t_token_type *type);
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
/* EXPAND FUNCTIONS */
char		*expand_one_arg(char *arg, int *i, t_env *head, int last_status);
char		*handle_normal_txt(int *i, char *arg, char *result);
char		*expand_arg_keep_quotes(char *arg, t_env *head,
				int last_status, int *had_unquoted);
void		dollar_expansion(t_command *cmd, t_env **head, int last_status);
/*expand_split_whitespace.c */
char		**ft_split_whitespace(const char *str);
/*expand_basics.c*/
char		*append_char(char c, char *result);
/*expand_split_replace*/
int			replace_arg_with_splits(t_command *cmd, int idx, char **parts);
/* expand_utils.c */
char		*append_key_value(t_env *head, char *arg, int *i, char *result);
int			is_valid_key(char *arg, int i, int *len);
char		*append_result(t_env *head, char *key, char *result,
				int last_status);
char		*find_env_value(t_env *head, char *key);
char		*append_normal_text(char *text, char *result);
/* executor/ 
 * exec.c */
void		execute(t_command *cmds, int *status, char **envp);
int			run_command(t_command *cmds, t_env **env, int status);
/* path.c */
char		*find_path(char *cmd, char **envp);
void		check_executable(char **args, char *path);
/*path_utils.c*/
int			is_special_dir(char *arg);
/* exec_utils.c */
int			run_builtin(t_command *cmd, t_env **env, int status);
int			prepare_builtin_exec(t_command *cmds, t_env **env, int status);
void		trim_empty_args(char **args);
/* pipes.c */
void		run_child(t_command *cmd, t_env **env, int status);
int			run_pipeline(t_command *cmds, t_env **env, int status);
/* pipe_utils.c */
void		init_vars(t_command **cmd, int *prev_fd,
				pid_t *last_pid, t_command *cmds);
int			wait_for_last(pid_t last_pid);
pid_t		init_pipes(int pipe_fd[2], t_command *cmd);
/* redirections.c */
void		parse_redirection(t_command *cmd, t_token **cpy);
void		apply_redirections(t_command *cmd, t_env **env, int last_status);
void		fd_check(int fd, int std_fd, char *file);
/* HEREDOC FUNCTIONS */
void		free_heredocs(t_command *cmd);
int			apply_heredocs(t_command *cmd, int last_status);
int			collect_heredocs(t_command *cmds, int status);
void		print_heredoc_prompt(void);
int			is_delimiter_line(char *line, char *trimmed_delim);
void		process_and_write_line(char *line, int expand,
				int status, int write_fd);
void		handle_redir_file(char *file, int append_mode, int fd_type);
void		handle_infile(char **filename);
int			handle_sigint_in_heredoc(char *line, char *trimmed_delim);
char		*get_trimmed_delimiter(const char *delimiter);
int			delimiter_was_quoted(const char *delimiter);
void		add_heredoc(t_command *cmd, const char *delimiter);
void		merge_fd_into_pipe(int src_fd, int dest_fd);
char		*expand_for_heredoc(char *line, int last_status);
char		*append_normal_text(char *text, char *result);
/* REDIRECTION FUNCTIONS */
void		fd_check(int fd, int std_fd, char *file);
int			dol_q_expansion(char *line, int *i, int last_status, char **result);
void		handle_redir_file(char *file, int append_mode, int fd_type);
void		handle_infile(char **filename);
int			is_last_redir(t_token *current);
char		*expand_arg_keep_quotes_simple(char *arg, t_env *head,
				int last_status);
void		set_redirection(t_command *cmd, t_token *token,
				int append_type, int last);
void		set_fd_type(t_command *cmd, t_token *cpy);
int			is_append_type(int type);
int			is_output_redir(int type);
void		handle_input_redir(t_command *cmd, t_token *next);
/* ENV FUNCTIONS */
t_env		*envp_to_struct(char **envp);
char		**struct_to_envp(t_env *head, int export_only);
/* BUILTIN FUNCTIONS/
 * cd_cmd.c */
char		*get_env_value(t_env **env, const char *key);
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
int			update_var(char *path, t_env **env);
int			export_cmd(t_command *cmd, t_env **env);
/*export utils*/
int			setting_value(char **equals, t_env **new_node);
void		free_keys(char *key_one, char *key_two);
void		setting_vars(char **path, char **equals, t_env **new_node);
int			check_match_and_free(int match, char *key);
int			check_match(int match);
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
/*unset_cmd_utils.c*/
void		remove_env_var(t_env **env, char *key);
/* UTILITY FUNCTIONS/
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
void		update_last_command(t_env **env, char *last_cmd);
int			args_len(char **args);
int			is_quoted(char *arg);
/* error.c */
void		error(const char *msg);
void		exec_error_custom(const char *cmd, const char *msg, int status);
void		exec_error_custom_simple(const char *cmd,
				const char *msg, int status);
void		exec_error(const char *msg, int status);
/* trim_quotes */
void		trim_quotes_for_execution(char **args);
char		*remove_quotes(char *arg);
/* SIGNAL FUNCTIONS/
 * parent_child_setup.c */
void		child_signal_setup(void);
void		parent_signal_setup(void);
/* signal_handler.c */
void		disable_ctrl_echo(void);
void		enable_ctrl_echo(void);
void		init_signals(void);

#endif
