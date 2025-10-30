/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/21 14:45:16 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/30 20:19:07 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* check_executable:
 * Validates that the resolved path is runnable.
 * - NULL path: 
 * 	'/' or '.' → "No such file or directory" (127),
 *  else → "command not found" (127).
 * - stat() fail → "No such file or directory" (127).
 * - Directory → "Is a directory" (126).
 * - No exec permission → "Permission denied" (126).
 * Frees path before exiting when needed.
 * also returns simple error - which doesnt include mini in 
 * front for command not found
*/
static int	is_special_dir(char *arg)
{
	return ((ft_strncmp(arg, ".", 2) == 0)
		|| (ft_strncmp(arg, "..", 3) == 0)
		|| (ft_strncmp(arg, "~", 2) == 0));
}

static void	exec_error_and_free(char *arg, char *path, char *msg, int code)
{
	free(path);
	exec_error_custom(arg, msg, code);
}

void	check_executable(char **args, char *path)
{
	struct stat	sb;

	if (!path)
		exec_error_custom(args[0], "command not found", 127);
	if (stat(path, &sb) == -1)
		exec_error_and_free(args[0], path, "command not found", 127);
	if (S_ISDIR(sb.st_mode))
	{
		if (is_special_dir(args[0]))
			exec_error_and_free(args[0], path, "command not found", 127);
		else
			exec_error_and_free(args[0], path, "Is a directory", 126);
	}
	if (access(path, X_OK) == -1)
		exec_error_and_free(args[0], path, "Permission denied", 126);
}
/*void	check_executable(char **args, char *path)
{
	struct stat	sb;

	if (!path)
		exec_error_custom(args[0], "command not found", 127);
	if (stat(path, &sb) == -1)
	{
		free(path);
		exec_error_custom(args[0], "command not found", 127);
	}
	if (S_ISDIR(sb.st_mode))
	{
		if ((ft_strncmp(args[0], ".", 2) == 0)
			|| (ft_strncmp(args[0], "..", 3) == 0)
			|| (ft_strncmp(args[0], "~", 2) == 0))
		{
			free(path);
			exec_error_custom(args[0], "command not found", 127);
		}
		else
		{
			free(path);
			exec_error_custom(args[0], "Is a directory", 126);
		}
	}
	if (access(path, X_OK) == -1)
	{
		free(path);
		exec_error_custom(args[0], "Permission denied", 126);
	}
}*/

static char	*try_cwd(const char *cmd)
{
	char	*cwd;
	char	*part_path;
	char	*path;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		return (NULL);
	part_path = ft_strjoin(cwd, "/");
	path = ft_strjoin(part_path, cmd);
	free(part_path);
	free(cwd);
	if (!access(path, F_OK))
		return (path);
	free(path);
	return (NULL);
}

/*Tries to find the command in PATH directories */
static char	*search_path_dirs(const char *cmd, const char *path_env)
{
	char	**paths;
	char	*part_path;
	char	*path;
	int		i;

	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
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

// Main find_path function
char	*find_path(char *cmd, char **envp)
{
	const char	*path_env;
	int			i;

	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0 && access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	i = 0;
	path_env = NULL;
	while (envp[i])
	{
		if (ft_strnstr(envp[i], "PATH=", 5))
		{
			path_env = envp[i] + 5;
			break ;
		}
		i++;
	}
	if (!envp[i])
		return (try_cwd(cmd));
	return (search_path_dirs(cmd, path_env));
}
