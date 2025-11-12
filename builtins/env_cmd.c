/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:43 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:03:54 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* handle_env_chdir:
 * Changes the current working directory to the given path.
 * If chdir fails, prints a custom error message and exits with code 127.
 */
static void	handle_env_chdir(char *path)
{
	if (chdir(path) == -1)
		exec_error_custom("minishell: env: ", "no such file or directory", 127);
}

/* env_cmd:
	Implements the `env` shell builtin.
	If a path argument is provided, attempts to change directory first.
	Converts the t_env linked list to an array and prints all 
 	environment variables. Frees allocated memory before returning.
	Returns 0 on success, 1 on allocation failure.
 */
int	env_cmd(t_command *cmd, t_env **env)
{
	int		i;
	char	**envp;

	i = 0;
	envp = struct_to_envp(*env, 1);
	if (cmd->args[1])
	{
		handle_env_chdir(cmd->args[1]);
	}
	if (!envp)
	{
		return (1);
	}
	while (envp[i])
	{
		if (ft_strchr(envp[i], '='))
			printf("%s\n", envp[i]);
		i++;
	}
	free_array(envp);
	return (0);
}
