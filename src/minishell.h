/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:50:53 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/03 15:41:23 by rbestman         ###   ########.fr       */
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

char	**split_input(char *str);
void	execute(char **args, char **envp);
void	free_array(char **array);
void	run_command(char **args, char **envp);

#endif
