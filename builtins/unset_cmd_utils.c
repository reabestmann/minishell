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
 * Checks if the current node's key matches `key` and deletes it if so.
 * Updates linked list pointers accordingly.
 * env: pointer to head of the list.
 * crnt: current node to check.
 * prev: pointer to previous node (for relinking).
 * key: environment variable name to match.
 * Returns: next node in the list for iteration.
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
 * Removes all nodes with a matching key from the environment list.
 * env: pointer to head of the list.
 * key: environment variable name to remove.
 * Notes: Iterates through the list and calls find_and_delete_node for each node.
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
