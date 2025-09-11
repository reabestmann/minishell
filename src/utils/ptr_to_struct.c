/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ptr_to_struct.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:42 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/11 13:14:08 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* add_nodes(t_env *current, char *equals, char *env_str)
   - Purpose: Creates a new t_env node from a single environment string ("KEY=VALUE")
              and links it to the end of the existing list.
   - Parameters:
       * current: pointer to the last node of the list; new node is attached here.
                  If NULL, the new node will be the first node.
       * equals: pointer to the '=' character in env_str.
       * env_str: full environment string to split into key and value.
   - Returns: Pointer to the newly created node, or NULL on allocation failure.
   - Notes: Allocates memory for key and value separately. Sets exported = 1
            since these come from the original environment. Caller must handle failures.
*/
static t_env *add_nodes(t_env *current, char *equals, char *env_str)
{
	t_env *new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return(NULL);
	new_node->key = ft_substr(env_str, 0, equals - env_str);
	if(!new_node->key)
	{
		free(new_node);
		return(NULL);
	}
	new_node->value = ft_strdup(equals + 1);
	if(!new_node->value)
	{
		free(new_node->key);
		free(new_node);
		return(NULL);
	}
	new_node->next = NULL;
	new_node->exported = 1;
	if(current)
		current->next = new_node;
	return(new_node);
}


/* envp_to_struct(char **envp)
   - Purpose: Converts a char ** environment array (from main) into a linked list of t_env nodes.
   - Parameters:
       * envp: NULL-terminated array of strings ("KEY=VALUE") to convert.
   - Returns: Pointer to the head of the linked list, or NULL on failure.
   - Notes: Iterates through envp, using add_nodes() to append each valid entry.
            Frees any allocated nodes on failure. Only strings containing '=' are added.
            This linked list will have exported = 1 for all nodes.
*/
t_env *envp_to_struct(char **envp)
{
	char *equals;
	int i;
	t_env *head;
	t_env *current;

	head = NULL;
	current = NULL;
	i = 0;		
	while(envp[i])
	{
		equals = ft_strchr(envp[i], '=');
		if(equals)
		{
			current = add_nodes(current, equals, envp[i]);
			if (!head)
				head = current;
			if(!current)
			{
				free_env_struct(head);
				return(NULL);
			}
		}
		i++;
	}
	return(head);
}
