/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pointer_to_struct.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:42 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 16:42:50 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*3. add_nodes(t_env *current, char *equals, char *env_str)
   - Purpose: Creates a new t_env node from a single environment string and links it to the current node.
   - Variables:
     * current: last node in the existing list; new node is attached here.
     * equals: pointer to '=' character in env_str.
     * env_str: full environment string ("KEY=VALUE").
     * new_node: newly allocated node being created.
   - Called from: envp_to_struct().
   - Calls: malloc(), ft_substr(), ft_strdup().*/

t_env *add_nodes(t_env *current, char *equals, char *env_str)
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
	if(current)
		current->next = new_node;
	return(new_node);
}

/*4. envp_to_struct(char **envp)
   - Purpose: Converts a char ** (environment array) into a linked list of t_env nodes.
   - Variables:
     * envp: array of strings to convert.
     * i: index for iterating envp.
     * head: pointer to first node of linked list.
     * current: last node in the list; used for appending.
     * equals: pointer to '=' character in current env string.
   - Called from: Anywhere you need a linked list representation of envp.
   - Calls: add_nodes(), free_env_struct(), ft_strchr().*/

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