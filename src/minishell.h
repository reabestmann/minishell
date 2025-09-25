/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:50:53 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/22 15:41:26 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* External library headers */
# include <stdlib.h>     // malloc, free, exit, getenv
# include <unistd.h>     // write, read, fork, execve, access, dup, dup2, pipe, chdir
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

/* parser/
 * lexer.c */
t_token	*lexer(const char *input);
/* parse.c */
int    handle_input(char *input, t_env **env, int status);
t_command	*parser(t_token *tokens);
/* parse_utils.c */
void	update_quotes(char c, int *in_quote, int *in_dquote);
int	skip_spaces(const char *str, int i);
void	set_cmd_flags(t_command *cmd);
int has_dollar(char **args);
/* syntax.c */
int	syntax_valid(t_token *tokens);
/* expand.c */
void	dollar_expansion(t_command *cmd, t_env **head, int last_status);
/* executor/ 
 * exec.c */
void	execute(char **args, char **envp);
int	run_command(t_command *cmds, t_env **env, int status);
int	run_builtin(t_command *cmd, t_env **env);
/* pipes.c */
void	run_child(t_command *cmd, t_env **env);
int	run_pipeline(t_command *cmds, t_env **env);
/* redirections.c */
void	parse_redirection(t_command *cmd, t_token **cpy);
void    apply_redirections(t_command *cmd);
void	fd_check(int fd, int std_fd, char *file);
/* ptr_to_struct.c */
t_env	*envp_to_struct(char **envp);
/* struct_to_ptr.c */
char **struct_to_envp(t_env *head, int export_only);
/* builtins/
 * cd_cmd.c */
int	cd_cmd(t_command *cmd, t_env **env);
/* echo_cmd.c */
int	echo_cmd(t_command *cmd);
/* pwd_cmd.c */
int	pwd_cmd(t_env **env);
/* env_cmd.c */
int	env_cmd(t_env **env);
/* exit_cmd.c */
int	exit_cmd(t_command *cmd, t_env **env);
/* export_cmd.c */
int	export_cmd(t_command *cmd, t_env **env);
/* unset_cmd.c */
int	unset_cmd(t_command *cmd, t_env **env);
/* utils/
 * free.c */
void    free_array(char **array);
void    free_commands(t_command *cmds);
void    free_tokens(t_token *tokens);
void	free_env_struct(t_env *head);
/* utils.c */
int     get_exit_status(int status);
int     str_equals(const char *str, const char *target);
void	*handle_malloc(size_t bytes);
void	error(const char *msg);
void	trim_quotes_for_execution(char **args);
/* signals/
 * parent_child_setup.c */
void	child_signal_setup(void);
void    parent_signal_setup(void);
/* signal_handler.c */
void    disable_ctrl_echo(void);
void    enable_ctrl_echo(void);
void    init_signals(void);

#endif
