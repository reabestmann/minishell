/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:43 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/11 13:30:48 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int env_cmd(t_env **env)
{
	int i;
	char **temp;

	i = 0;
	temp = struct_to_envp(*env, 1);
    if (!temp)
        return (1);
	while(temp[i])
	{
		printf("%s\n", temp[i]);
		i++;
	}
	free_array(temp);
	return (0);
}