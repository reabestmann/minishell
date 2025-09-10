/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:59 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 21:00:51 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int pwd_cmd(t_env **env)
{
	t_env *temp;
	//char *cwd;

	temp = *env;

	while (!str_equals(temp->key, "PWD"))
		temp = temp->next;
	if (temp && temp->value)
		printf("%s\n", temp->value);
	else //do we actually want to do this? 
	{
		error("env struct PWD faulty");
		/*cwd = getcwd(NULL, 0);
		if (!cwd)
			return (1);
		printf("%s\n", cwd);
		free(cwd);*/
	}
	return(0);
}