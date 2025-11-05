/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:50:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/30 19:59:08 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* execute: 
   Trim quotes in args, resolve command path,
	run check_executable() on the path,
	all execve() with args/envp.
*/
void	execute(t_command *cmds, int *status, char **envp)
{
	char	*path;

	if (!cmds->args || !cmds->args[0] || cmds->expand_empty)
	{
		*status = 0;
		return ;
	}
	if (cmds->args[0][0] == '\0')
		exec_error_custom("''", "command not found", 127);
	trim_empty_args(cmds->args);
	if (cmds->args[0][0] == '/'
		|| (cmds->args[0][0] == '.' && cmds->args[0][1] == '/'))
		path = ft_strdup(cmds->args[0]);
	else
		path = find_path(cmds->args[0], envp);
	check_executable(cmds->args, path);
	execve(path, cmds->args, envp);
	free(path);
	exec_error(cmds->args[0], 1);
}

/* fork_process: 
	creates a child process with fork().
	In the child: sets signals, (TODO redirections), then execve().
	In the parent: waits for the child and resets signals.
*/
static int	fork_process(t_command *cmds, t_env **env, int status)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		error("minishell: fork");
	if (pid == 0)
		run_child(cmds, env, status);
	parent_signal_setup();
	if (waitpid(pid, &status, 0) == -1)
		error("waitpid");
	init_signals();
	return (get_exit_status(status));
}

/* run_command: 
	main entry point to run a command node.
	If args is empty, does nothing.  
	If cmd modifies shell and is a standalone, 
		run_builtin will run it in parent;
		otherwise runs via fork/execve.
	if cmd is not a standalone, run_pipeline runs it.
	now also checks for empty strings and returns error 
		via custom error function
*/
int	run_command(t_command *cmds, t_env **env, int status)
{
	if (!cmds)
		return (0);
	if (has_dollar(cmds->args))
		dollar_expansion(cmds, env, status);
	if (!cmds->in_child)
	{
		if (cmds->next)
			return (run_pipeline(cmds, env, status));
		if ((cmds->args && cmds->args[0]) || cmds->infile
			|| cmds->outfile || cmds->errfile || cmds->heredoc_delim)
		{
			if (cmds->modifies_shell && cmds->args && cmds->args[0])
				return (prepare_builtin_exec(cmds, env, status));
			return (fork_process(cmds, env, status));
		}
		else
			return (0);
	}
	else
		return (run_pipeline(cmds, env, status));
}
