/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_cmd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:59 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 12:54:06 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* pwd_cmd:
 * Prints the value of the "PWD" environment variable.
 * Searches env for key "PWD" and prints its value if found.
 */
int	pwd_cmd(t_env **env)
{
	t_env	*temp;

	temp = *env;
	while (!str_equals(temp->key, "PWD"))
		temp = temp->next;
	if (temp && temp->value)
		printf("%s\n", temp->value);
	return (0);
}
