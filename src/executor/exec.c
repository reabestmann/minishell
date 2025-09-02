/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:50:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/03 15:42:59 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


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
			//free_array(paths);
			return (path);
		}
		free(path);
		i++;
	}
	//free_array(paths);
	return (NULL);
}

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

/*Modify run_command or create a new function for piped commands.
This current code only handles a single command — no pipes.
Later, you’ll want a new function that loops over a linked list of commands, creates pipes, and forks multiple times — similar to what we discussed earlier.*/
/*will want to take t_cammand *cmd instead of args later or have two functions... */
void	run_command(t_command *cmds, char **envp)
{
	pid_t	pid;
	int	status;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return ;
	}
	if (pid == 0)
	{
		child_signal_setup();
		/*Check if input/output files need to be redirected (your t_command struct has those fields).
Call helper functions that do open(), dup2(), close().
After that, call execute().*/
		/*call apply_redirections(cmd); which is where the above will take place*/
		execute(cmds->args, envp);
		exit(1);
	}
	else
	{
		parent_signal_setup();
		if (waitpid(pid, &status, 0) == -1)
		{
			write(2, "waitpid error\n", 14);
			return;
		}
		init_signals();
	}
}
