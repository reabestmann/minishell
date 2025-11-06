/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 16:13:11 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/06 18:19:21 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* apply_pipes:
   Handles piping for a child process.
   Sets up stdin/stdout using dup2 for the pipeline,
	and closes unused pipe ends in this child.
*/
static void	apply_pipes(int prev_fd, int pipe_fd[2], t_command *cmd)
{
	if (prev_fd != -1 && !cmd->heredoc_count)
		fd_check(prev_fd, STDIN_FILENO, "pipe read");
	if (cmd->next)
	{
		fd_check(pipe_fd[1], STDOUT_FILENO, "pipe write");
		if (pipe_fd[0] != -1)
			close(pipe_fd[0]);
	}
}

/* mini_tee:
	Mini version of the Unix 'tee' command for pipelines.
	Reads from standard input, writes each line to both:
	STDOUT & outfile.
 	Used for commands that have both an outfile
	 and a next command in the pipeline.
 */
static void	mini_tee(t_command *cmd, int out_fd)
{
	char	*line;

	if (cmd->append == 1)
		out_fd = open(cmd->outfile, O_CREAT | O_WRONLY | O_APPEND, 0644);
	else if (cmd->append == 2)
		out_fd = open(cmd->outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (out_fd < 0)
		error("open fd");
	line = get_next_line(STDIN_FILENO);
	while (line)
	{
		write(STDOUT_FILENO, line, ft_strlen(line));
		write(out_fd, line, ft_strlen(line));
		free(line);
		line = get_next_line(STDIN_FILENO);
	}
	close(out_fd);
	exit(0);
}

/*runs child - signal set up, apply redirections, etc - 
now also does dollar_expansion */
void	run_child(t_command *cmd, t_env **env, int status)
{
	char	**envp;

	cmd->in_child = 1;
	child_signal_setup();
	reset_terminal_mode();
	apply_redirections(cmd, env, status);
	if (!cmd->args || !cmd->args[0])
		exit(EXIT_SUCCESS);
	if (cmd->args && cmd->args[0])
		dollar_expansion(cmd, env, status);
	if (cmd->outfile && cmd->next)
		mini_tee(cmd, -1);
	if (run_builtin(cmd, env, status) == -1)
	{
		if (cmd->args && cmd-> args[0])
			update_last_command(env, cmd->args[0]);
		envp = struct_to_envp(*env, 1);
		execute(cmd, &status, envp);
	}
	exit(EXIT_SUCCESS);
}

/* parent_process:
Executed in the parent after forking a child.
- Closes old pipe ends no longer needed
*/
static int	parent_process(t_command *cmd, int prev_fd, int pipe_fd[2])
{
	parent_signal_setup();
	if (prev_fd != -1)
		close(prev_fd);
	if (cmd->next)
	{
		if (pipe_fd[1] != -1)
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

/* run_pipeline:
Executes commands connected by pipes,
forks each command in a child process,
pipes output to input of the next child
- Waits for all children at the end
*/
int	run_pipeline(t_command *cmds, t_env **env, int status)
{
	t_command	*cmd;
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;
	pid_t		last_pid;

	init_vars(&cmd, &prev_fd, &last_pid, cmds);
	while (cmd)
	{
		pid = init_pipes(pipe_fd, cmd);
		if (pid == 0)
		{
			apply_pipes(prev_fd, pipe_fd, cmd);
			run_child(cmd, env, status);
		}
		else
		{
			prev_fd = parent_process(cmd, prev_fd, pipe_fd);
			last_pid = pid;
		}
		cmd = cmd->next;
	}
	if (prev_fd != -1)
		close(prev_fd);
	return (wait_for_last(last_pid));
}
