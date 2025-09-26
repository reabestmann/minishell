/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:13:11 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/26 16:52:58 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* apply_pipes:
   Handles piping for a child process.
   prev_fd: read end from previous command (or -1 if none)
   pipe_fd: current pipe
   cmd: current command node
*/
static void	apply_pipes(int prev_fd, int pipe_fd[2], t_command *cmd)
{
	if (prev_fd != -1)
		fd_check(prev_fd, STDIN_FILENO, "pipe read");
	if (cmd->next)
		fd_check(pipe_fd[1], STDOUT_FILENO, "pipe write");
}

/* child_exec:
   Executed in the forked child process.
   - Sets up stdin/stdout according to pipes and redirections
   - Runs a builtin (if allowed) or executes an external command
   - Exits the child when done
*/
void	run_child(t_command *cmd, t_env **env)
{
	char	**envp;

	cmd->in_child = 1;
	child_signal_setup();
	apply_redirections(cmd);
	if (!cmd->args || !cmd->args[0])
		exit(0);
	if (run_builtin(cmd, env) == -1)
	{
		envp = struct_to_envp(*env, 1);
		execute(cmd->args, envp);
	}
	exit(0);
}

/* parent_process:
   Executed in the parent after forking a child.
   - Closes old pipe ends no longer needed
   - Keeps read end of current pipe for next command in the pipeline
   - Returns the fd to pass to the next child, or -1 if last command
*/
static int	parent_process(t_command *cmd, int prev_fd, int pipe_fd[2])
{
	parent_signal_setup();
	if (prev_fd != -1)
		close(prev_fd);
	if (cmd->next)
	{
		close(pipe_fd[1]);
		return (pipe_fd[0]);
	}
	else
	{
		if (pipe_fd[0] != -1)
			close(pipe_fd[0]);
		if (pipe_fd[1] != -1)
			close(pipe_fd[1]);
		return (-1);
	}
}

/* init_pipes:
    helper function that initializes the pipe fds for the current command.
    - If the cmd is not the last, create a new pipe.
    - Else, set both ends to -1, signaling "no pipe"*/
static pid_t	init_pipes(int pipe_fd[2], t_command *cmd)
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

/* run_pipeline:
   Executes a linked list of commands connected by pipes.
   - Forks each command in a child process
   - Pipes output to input of the next child
   - Builtins modifying the shell still run in 
   		parent if standalone (handled elsewhere)
   - Waits for all children at the end
*/
int	run_pipeline(t_command *cmds, t_env **env)
{
	t_command	*cmd;
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;
	int			status;

	cmd = cmds;
	prev_fd = -1;
	status = 0;
	while (cmd)
	{
		pid = init_pipes(pipe_fd, cmd);
		if (pid == 0)
		{
			apply_pipes(prev_fd, pipe_fd, cmd);
			run_child(cmd, env);
		}
		else
			prev_fd = parent_process(cmd, prev_fd, pipe_fd);
		cmd = cmd->next;
	}
	while (wait(&status) > 0)
		status = get_exit_status(status);
	return (status);
}
