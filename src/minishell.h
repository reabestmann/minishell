/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:50:53 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/02 16:24:01 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

// 🧠 Standard Library (Memory, Strings, Numbers)
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

// 🧁 Your own headers (if needed later)
//# include "structs.h"    // Optional: if you want to keep data structs separate
# include "libft.h"      // If you use your own libft
# include "structs.h"

/*main*/
/* parser/
 * split.c */
void    update_quotes(char c, int *in_quote, int *in_dquote);
char	**split_input(char *str);
/* pipe_split.c */
int	skip_space(char *str, int i);
char    **split_pipe(char *str);
/* parse.c */
void    handle_input(char *input, char **envp);
t_token *lexer(const char *input);
/* executor/ 
 * exec.c */
void	execute(char **args, char **envp);
void	run_command(t_command *cmds, char **envp);
void    child_signal_setup(void);
/* redirections.c */
void    rewire(t_command cmd, int pipe_fd[2], t_pipe_mode mode);
void    rewire_input(t_command cmd);
void    rewire_output(t_command cmd);
void    rewire_pipes(int pipe_fd[2], t_pipe_mode mode);
/* utils/
 * free.c */
void    free_array(char **array);
void    free_commands(t_command *cmds);
void    free_tokens(t_token *tokens);
void	*handle_malloc(size_t bytes);
void	error(const char *msg);
/*signal*/
/*parent_child_setup.c*/
void    parent_signal_setup(void);
/*signal_handler.c*/
void    disable_ctrl_echo(void);
void    enable_ctrl_echo(void);
void    init_signals(void);

//delete later
t_command parse_input(char *input);

#endif
