/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:43 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/01 16:05:29 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Purpose: Prints all exported environment variables.
Input: Pointer to the head of a t_env linked list.
Behavior:
Converts the linked list to a char ** array containing only exported variables.
Prints each variable on its own line in the format "KEY=VALUE".
Frees the allocated array.
Return Value: 0 on success, 1 if memory allocation fails.
*/

int	env_cmd(t_command *cmd, t_env **env)
{
	int		i;
	char	**envp;
	//char	*path;
	i = 0;
	envp = struct_to_envp(*env, 1);
	if(cmd->args[1])
	{
		if (chdir(cmd->args[1]) == -1)
		{
			exec_error_custom("minishell: env: ", "no such file or directory", 127);
		}
	}
	if (!envp)
	{
		return (1);
	}
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	free_array(envp);
	return (0);
}
