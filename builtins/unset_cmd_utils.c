/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_cmd_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:05:20 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:05:53 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* find_and_delete_node:
 * Deletes current node's key if it matches a given key.
 * Updates linked list pointers accordingly.
 */
t_env	*find_and_delete_node(t_env **env, t_env *crnt, t_env **prev, char *key)
{
	t_env	*next;

	next = crnt->next;
	if (ft_strncmp(crnt->key, key, ft_strlen(key)) == 0
		&& ft_strlen(crnt->key) == ft_strlen(key))
	{
		if (*prev == NULL)
			*env = next;
		else
			(*prev)->next = next;
		free(crnt->value);
		free(crnt->key);
		free(crnt);
	}
	else
		*prev = crnt;
	return (next);
}

/* remove_env_var:
	Removes all nodes with a matching key from env.
	Iterates through env calling find_and_delete_node.
 */
void	remove_env_var(t_env **env, char *key)
{
	t_env	*current;
	t_env	*previous;

	current = *env;
	previous = NULL;
	while (current)
		current = find_and_delete_node(env, current, &previous, key);
}
