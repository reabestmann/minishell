/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:13:11 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/27 22:45:31 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* init_pipes:
	helper function that initializes the pipe fds for the current command.
	- If the cmd is not the last, create a new pipe.
	- Else, set both ends to -1, signaling "no pipe"
*/
pid_t	init_pipes(int pipe_fd[2], t_command *cmd)
{
	pid_t	pid;

	if (cmd->next)
	{
		if (pipe(pipe_fd) == -1)
			error("pipe");
	}
	else
	{
		pipe_fd[0] = -1;
		pipe_fd[1] = -1;
	}
	pid = fork();
	if (pid < 0)
		error("fork");
	return (pid);
}

/* wait_for_last:
	Tracks the PID of the last command in the pipeline.
	Waits for all children to finish (avoiding zombie processes)
	sets last_status only for the last command,
	returns it, becoming the pipelines final exit code.
*/
int	wait_for_last(pid_t last_pid)
{
	int		wstatus;
	pid_t	wpid;
	int		last_status;

	last_status = 0;
	wpid = wait(&wstatus);
	while (wpid > 0)
	{
		if (wpid == last_pid)
			last_status = get_exit_status(wstatus);
		wpid = wait(&wstatus);
	}
	return (last_status);
}

/* init_vars:
	Helper that initiates variables of run_pipeline
*/
void	init_vars(t_command **cmd, int *prev_fd,
	pid_t *last_pid, t_command *cmds)
{
	*cmd = cmds;
	*prev_fd = -1;
	*last_pid = 0;
}
