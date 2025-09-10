/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:50:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/04 18:56:49 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*  roadmap:
	1. run_command(): decides wether to fork or not
		- standalone builtin that modifies the shell runs directly
		- for the rest, we fork.                         
	2.  else → fork_process() is called
		(fork + execve child, parent waits).
		if builtin, run_builtin is called (calls function directly)
	4. Helper functions: 
		find_path(): searches PATH to locate an executable.
		execute(): resolves path + calls execve().
		str_equals() + is_builtin(): helpers to detect builtins. (in exec_utils.c) 
*/

/* find_path: 
	searches envp for PATH, splits it into directories.
	Joins each directory with 'cmd' and checks if it's an executable.
	Returns the full path if found, otherwise NULL.
*/
static char	*find_path(char *cmd, char **envp)
{
	char	**paths;
	char	*part_path;
	char	*path;
	int		i;

	i = 0;
	while (!ft_strnstr(envp[i], "PATH", 4))
		i++;
	paths = ft_split(envp[i] + 5, ':');
	i = 0;
	while (paths[i])
	{
		part_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(part_path, cmd);
		free(part_path);
		if (access(path, F_OK) == 0 && access(path, X_OK) == 0)
		{
			free_array(paths);
			return (path);
		}
		free(path);
		i++;
	}
	free_array(paths);
	return (NULL);
}
/* execute: 
	calls find_path() to resolve the command's location.
	If not found, prints "command not found" 
	and exits 127 (command not found).
	If found, runs execve() with args/envp, exits on failure.
*/
void	execute(char **args, char **envp)
{
	char	*path;

	path = find_path(args[0], envp);
	if (!path)
	{
		ft_putstr_fd("minishell: command not found: ", 2);
		ft_putstr_fd(args[0], 2);
		ft_putstr_fd("\n", 2);
		exit(127);
	}
	if (execve(path, args, envp) == -1)
	{
		perror("execve");
		free(path);
		exit(1);
	}
}
/* fork_process: 
	creates a child process with fork().
	In the child: sets signals, (TODO redirections), then execve().
	In the parent: waits for the child and resets signals.
*/
static void	fork_process(t_command *cmds, t_env **env)
{
	pid_t	pid;
	int		status;
	char	**envp;

	pid = fork();
	if (pid < 0)
		error("minishell: fork");
	if (pid == 0)
	{
		cmds->in_child = 1;
		child_signal_setup();
		apply_redirections(cmds); 
		if (!cmds->modifies_shell)
		{
			if (run_builtin(cmds, env) == -1)
			{
				envp = struct_to_envp(*env);
				execute(cmds->args, envp);
			}
		}
		exit(1);
	}
	else
	{
		parent_signal_setup();
		if (waitpid(pid, &status, 0) == -1)
			error("waitpid");
		init_signals();
	}
}
/* run_command: 
	main entry point to run a command node.
	If args is empty, does nothing.  
	If cmd modifies shell and is a standalone, 
		run_builtin will run it in parent;
	otherwise runs via fork/execve.
*/
void	run_command(t_command *cmds, t_env **env)
{
	if (!cmds || !cmds->args || !cmds->args[0])
		return ;
	if (cmds->modifies_shell && !cmds->in_child && !cmds->infile && !cmds->outfile && !cmds->next)
	{
		run_builtin(cmds, env);
		return ;
	}
	fork_process(cmds, env);
}
