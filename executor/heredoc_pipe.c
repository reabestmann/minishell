/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:58:45 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/06 15:57:12 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* heredoc_child_process:
   Handles heredoc reading in child process.
   Exits with status 0 on success, 130 on SIGINT.
*/
void	heredoc_child_process(char *delim, int write_fd, int status)
{
	struct sigaction	sa;
	int					result;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = heredoc_sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		error("error with sigint_handler");
	result = write_heredoc(delim, write_fd, status);
	close(write_fd);
	exit(0);
}

static int	heredoc_interactive(t_command *cmd, int i, int hd_pipe[2])
{
	pid_t	pid;
	int		status;

	status = 0;
	pid = fork();
	if (pid == -1)
		error("pipe heredoc");
	if (pid == 0)
		heredoc_child_process(cmd->heredoc_delim[i], hd_pipe[1], status);
	close(hd_pipe[1]);
	waitpid(pid, &status, 0);
	return (get_exit_status(status));
}

static int	heredoc_noninteractive(t_command *cmd, int i, int hd_pipe[2])
{
	int	res;
	int	status;

	status = 0;
	res = write_heredoc(cmd->heredoc_delim[i], hd_pipe[1], status);
	close(hd_pipe[1]);
	if (res == -1)
	{
		close(hd_pipe[0]);
		return (-1);
	}
	return (0);
}

/* apply_heredocs:
 Sets up pipes for each heredoc, writes contents (forks only if interactive), 
 and stores read ends in command struct.*/
int	apply_heredocs(t_command *cmd)
{
	int		hd_pipe[2];
	int		i;

	if (!cmd->heredoc_count)
		return (0);
	i = -1;
	while (++i < cmd->heredoc_count)
	{
		if (pipe(hd_pipe) == -1)
			error("pipe heredoc");
		if (isatty(STDIN_FILENO))
		{
			if (heredoc_interactive(cmd, i, hd_pipe) != 0)
				return (-1);
		}
		else
		{
			if (heredoc_noninteractive(cmd, i, hd_pipe) != 0)
				return (-1);
		}
		cmd->heredoc_fds[i] = hd_pipe[0];
	}
	return (0);
}
