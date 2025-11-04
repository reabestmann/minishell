/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ptr_to_struct.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:42 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 14:00:17 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/* node_value:
**   Duplicates the value part of an env string (after '='). 
**   Frees the node if allocation fails and returns NULL.*/
static char	*node_value(t_env *node, char *equals)
{
	char	*value;

	value = ft_strdup(equals + 1);
	if (!value)
	{
		free(node->key);
		free(node);
		return (NULL);
	}
	return (value);
}

/* now_free(t_env *new_node)
   - Purpose: Frees a partially allocated t_env node 
	if allocation of key or value fails.
   - Parameters:
       * new_node: pointer to the node being checked.
   - Returns: 1 if memory was freed due to failure, 0 if everything is fine.
   - Notes: Used internally by add_nodes() to safely handle allocation failures.
*/
static t_env	*add_nodes(t_env *current, char *equals, char *env_str)
{
	t_env	*new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = ft_substr(env_str, 0, equals - env_str);
	if (!new_node->key)
	{
		free(new_node);
		return (NULL);
	}
	new_node->value = node_value(new_node, equals);
	if (!new_node->value)
		return (NULL);
	new_node->next = NULL;
	new_node->exported = 1;
	if (current)
		current->next = new_node;
	return (new_node);
}

/* envp_to_struct:
**   Converts a char **envp array into a linked list of t_env nodes.
**   Only strings containing '=' are added.
**   Uses add_nodes() to create nodes and links them.
**   Frees all allocated nodes on failure and returns NULL.
**   Returns head of the linked list on success.*/
t_env	*envp_to_struct(char **envp)
{
	char	*equals;
	int		i;
	t_env	*head;
	t_env	*current;

	head = NULL;
	current = NULL;
	i = 0;
	while (envp[i])
	{
		equals = ft_strchr(envp[i], '=');
		if (equals)
		{
			current = add_nodes(current, equals, envp[i]);
			if (!head)
				head = current;
			if (!current)
			{
				free_env_struct(head);
				return (NULL);
			}
		}
		i++;
	}
	return (head);
}
