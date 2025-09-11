/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:54 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/11 17:00:36 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* now_free(t_env *new_node)
   - Purpose: Frees a partially allocated t_env node if allocation of key or value fails.
   - Parameters:
       * new_node: pointer to the node being checked.
   - Returns: 1 if memory was freed due to failure, 0 if everything is fine.
   - Notes: Used internally by add_nodes() to safely handle allocation failures.
*/
static int now_free(t_env *new_node)
{
	if(!new_node->key)
	{
		free(new_node);
		return(1);
	}
	else if(!new_node->value)
	{
		free(new_node->key);
		free(new_node);
		return(1);
	}
	return(0);
}

/* add_nodes(t_env *current, char *equals, char *env_str)
   - Purpose: Creates a new t_env node from a "KEY=VALUE" string and appends it to the list.
   - Parameters:
       * current: pointer to the last node in the list; NULL if list is empty.
       * equals: pointer to '=' character in env_str.
       * env_str: full environment string to split into key and value.
   - Returns: pointer to the new node, or NULL on allocation failure.
   - Notes: Allocates memory for key and value. Sets exported = 1. Uses now_free() to handle allocation errors.
*/
static t_env *add_nodes(t_env *current, char *equals, char *env_str)
{
	t_env *new_node;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return(NULL);
	ft_memset(new_node, 0, sizeof(t_env));
	new_node->exported = 1;
	new_node->key = ft_substr(env_str, 0, equals - env_str);
	if(now_free(new_node) == 1)
		return(NULL);
	if (equals != 0)
		new_node->value = ft_strdup(equals + 1);
	else
		new_node->value = NULL;
	if(now_free(new_node) == 1)
		return(NULL);
	new_node->next = NULL;
	if(current)
		current->next = new_node;
	return(new_node);
}

static int update_var(t_command *cmd, t_env **env)
{
	int i;
	t_env *temp;

	i = 0;
	temp = *env;
	while (cmd->args[1] != temp->key || temp == NULL || temp->next == NULL)
		temp = temp->next;
	if (temp->next == NULL || temp == NULL)
		add_node(temp, );
	
	/*look to see if var exists - if it does update key
	else send to add_node to add to end - return 1 on failure*/
	return(0);
}

static int list_exported()
{
	/*sort all that are listed as exported by alphabetical and print*/
	return(0);
}

int export_cmd(t_command *cmd, t_env **env)
{
    if (!cmd || !env)
        return (1);
	if (update_var() == 1)
		return(0);
	else
		list_exported();
    printf("export is not available yet\n");
    return (0);
}