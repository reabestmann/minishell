/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:43 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/17 16:03:30 by aabelkis         ###   ########.fr       */
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

int	env_cmd(t_env **env)
{
	int		i;
	char	**temp;

	i = 0;
	temp = struct_to_envp(*env, 1);
	if (!temp)
	{
		return (1);
	}
	while (temp[i])
	{
		printf("%s\n", temp[i]);
		i++;
	}
	free_array(temp);
	return (0);
}
