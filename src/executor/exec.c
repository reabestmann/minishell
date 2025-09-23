/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:50:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/23 16:59:08 by aabelkis         ###   ########.fr       */
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
/* run_builtin: 
	checks if 'cmd' matches any builtin name.
	Calls str_equals for each of the 7 builtin commands.
	Calls the function directly and returns its result.
	-> 0 = success, >0 = Error.
	returns -1 if command is not a builtin.
*/

int	run_builtin(t_command *cmd, t_env **env)
{
	if (str_equals(cmd->args[0], "cd"))
		return (cd_cmd(cmd, env));
	trim_quotes_for_execution(cmd->args);
	if (str_equals(cmd->args[0], "echo"))
		return (echo_cmd(cmd));
	if (str_equals(cmd->args[0], "pwd"))
		return (pwd_cmd(env));
	if (str_equals(cmd->args[0], "export"))
		return (export_cmd(cmd, env));
	if (str_equals(cmd->args[0], "unset"))
		return (unset_cmd(cmd, env));
	if (str_equals(cmd->args[0], "env"))
		return (env_cmd(env));
	if (str_equals(cmd->args[0], "exit"))
		return (exit_cmd(cmd));
	return (-1);
}

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
	trim_quotes_for_execution(args);
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
static int	fork_process(t_command *cmds, t_env **env)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
		error("minishell: fork");
	if (pid == 0)
		run_child(cmds, env);
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
	if (!cmds || !cmds->args || !cmds->args[0])
		return (0);
	if (has_dollar(cmds->args))
		dollar_expansion(cmds, env, status);
	/*//I want to print all my commands here so that I can see how they look
	  // --- DEBUG: print all args after expansion ---
    for (int i = 0; cmds->args[i]; i++)
        printf("arg[%d] = %s\n", i, cmds->args[i]);
    // ---------------------------------------------*/

	if (!cmds->in_child && !cmds->infile && !cmds->outfile && !cmds->next)
	{
		if (cmds->modifies_shell)
			return (run_builtin(cmds, env));
		return (fork_process(cmds, env));
	}
	else
		return (run_pipeline(cmds, env));
}
