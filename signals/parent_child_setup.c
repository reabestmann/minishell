/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent_child_setup.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 15:29:42 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/17 15:41:55 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

//returns 1 if sigint was received, 0 otherwise
int	handle_sigint_in_heredoc(char *line, char *trimmed_delim)
{
	if (g_sigint_received)
	{
		free(line);
		free(trimmed_delim);
		return (1);
	}
	return (0);
}

/* wait_for_heredoc_child:
   Waits for child process and returns -1 if interrupted (Ctrl+C),
   0 otherwise.
*/
int	wait_for_heredoc_child(pid_t pid, int hd_pipe[2])
{
	int	status;

	close(hd_pipe[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		close(hd_pipe[0]);
		return (-1);
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		close(hd_pipe[0]);
		return (-1);
	}
	return (0);
}

/* heredoc_child_process:
   Handles heredoc reading in child process.
   Exits with status 0 on success, 130 on SIGINT.
*/
void	heredoc_child_process(char *delim, int write_fd, int status)
{
	struct sigaction	sa;
	int					result;

	enable_ctrl_echo();
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = heredoc_sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		error("error with sigint_handler");
	result = write_heredoc(delim, write_fd, status);
	close(write_fd);
	if (result == -1)
		exit(130);
	exit(0);
}

/*here I set up sigaction struct so default actions occur instead of what we 
	had in main and then execute SIGINT, SIGQUIT and SIGPIPE signals in child*/
void	child_signal_setup(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		error("error with sigint_handler");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		error("error with sigquit");
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
		error("error with sigpipe");
}

/*here I set up sigaction struct so that signals are ignored instead 
	of what we had in main (for parent process)*/
void	parent_signal_setup(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		error("error with sigint_handler");
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		error("error with sigquit");
}
