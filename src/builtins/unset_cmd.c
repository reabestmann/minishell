/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:03:05 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/11 15:27:58 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* find_and_delete_node(t_env **env, t_env *crnt, t_env **prev, char *key)
   - Purpose: Checks a single t_env node against a key and deletes it if it matches.
   - Variables:
     * env: pointer to the head of the linked list.
     * crnt: the current node being checked.
     * prev: pointer to the previous node, used for relinking.
     * key: the environment variable key to match.
     * next: stores crnt->next to continue iteration safely after deletion.
   - Behavior:
     * Compares crnt->key to key.
     * If it matches, removes crnt from the list, updates pointers, and frees memory.
     * If not, updates prev to point to crnt.
   - Returns: pointer to the next node for continuing iteration.
*/
t_env *find_and_delete_node(t_env **env, t_env *crnt, t_env **prev, char *key)
{
	t_env *next;
	
	next = crnt->next;
	if(ft_strncmp(crnt->key, key, ft_strlen(key)) == 0  && 
		ft_strlen(crnt->key) == ft_strlen(key)) // otherwise circle throuhg until find it
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
	return(next);
}


/* unset_cmd(t_command *cmd, t_env **env)
   - Purpose: Removes one or more environment variables from the t_env linked list.
   - Variables:
     * cmd: command structure containing the arguments to unset.
     * env: pointer to the head of the linked list.
     * current: node currently being checked.
     * previous: previous node for relinking the list.
     * i: index for iterating through cmd->args.
   - Behavior:
     * Iterates over all arguments after 'unset' in cmd->args.
     * For each argument, iterates through the linked list and calls find_and_delete_node to remove matching nodes.
     * Supports multiple args and multiple nodes with the same key.
   - Returns: 0 on success, 1 if cmd or env is NULL.
*/
int unset_cmd(t_command *cmd, t_env **env)
{
	t_env *current;
	t_env *previous;
	int i;

	if (!cmd || !env)
		return (1);
	i = 1;
	while(cmd->args[i])
	{
		current = *env;
		previous = NULL;
		while(current != NULL)
			current = find_and_delete_node(env, current, &previous, cmd->args[i]);
		i++;
	}
	return (0);
}