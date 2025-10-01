/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:50:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/01 15:35:10 by aabelkis         ###   ########.fr       */
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
	3. Helper functions: 
		find_path(): searches PATH to locate an executable.
		execute(): resolves path + calls execve().
		str_equals() + is_builtin(): helpers to detect builtins. (in utils.c) 
*/

/* find_path: 
	searches envp for PATH, splits it into directories.
	Joins each directory with 'cmd' and checks if it's an executable.
	Returns the full path if found, otherwise NULL.
*/
char	*find_path(char *cmd, char **envp)
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

/* check_executable:
 * Validates that the resolved path is runnable.
 * - NULL path: 
 * 	'/' or '.' → "No such file or directory" (127),
 *  else → "command not found" (127).
 * - stat() fail → "No such file or directory" (127).
 * - Directory → "Is a directory" (126).
 * - No exec permission → "Permission denied" (126).
 * Frees path before exiting when needed.
*/
static void	check_executable(char **args, char *path)
{
	struct stat	sb;

	if (!path)
	{
		if (args[0][0] == '/' || args[0][0] == '.')
			exec_error_custom(args[0], "No such file or directory", 127);
		else
			exec_error_custom(args[0], "command not found", 127);
	}
	if (stat(path, &sb) == -1)
	{
		free(path);
		exec_error(args[0], 127);
	}
	if (S_ISDIR(sb.st_mode))
	{
		free(path);
		exec_error_custom(args[0], "Is a directory", 126);
	}
	if (access(path, X_OK) == -1)
	{
		free(path);
		exec_error(args[0], 126);
	}
}

/* execute: 
  1. Return early if args empty.
   2. Trim quotes in args.
   3. Resolve command path:
        '/' or './' → direct,
        else → PATH lookup.
   4. Run check_executable() on the path.
   5. Call execve() with args/envp.
   6. On execve fail, free path and exit(1).
*/
void	execute(char **args, char **envp)
{
	char	*path;

	if (!args || !args[0])
		return ;
	//trim_quotes_for_execution(args);
	//check here for if its  a command?
	trim_empty_args(args);
	if (!args[0] || args[0][0] == '\0')
		return ;
	if (args[0][0] == '/' || (args[0][0] == '.' && args[0][1] == '/'))
		path = ft_strdup(args[0]);
	else
		path = find_path(args[0], envp);
	check_executable(args, path);
	execve(path, args, envp);
	free(path);
	exec_error(args[0], 1);
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
*/
int	run_command(t_command *cmds, t_env **env, int status)
{
	if (!cmds)
		return (0);
	if (has_dollar(cmds->args))
		dollar_expansion(cmds, env, status);
	if (collect_heredocs(cmds, status) < 0)
		return (1);
	if (!cmds->in_child)
	{
		if (cmds->next)
			return (run_pipeline(cmds, env, status));
		else if ((cmds->args && cmds->args[0])
			|| cmds->infile || cmds->outfile || cmds->heredoc_delim)
		{
			if (cmds->modifies_shell && cmds->args && cmds->args[0])
				return (run_builtin(cmds, env, status));
			return (fork_process(cmds, env, status));
		}
		else
			return (0);
	}
	else
		return (run_pipeline(cmds, env, status));
}
