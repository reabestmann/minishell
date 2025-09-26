/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:51:17 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 18:15:04 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* setting_value(equals, new_node)
   - Purpose: Extracts the value part from a "KEY=VALUE" string and assigns it 
   to the node.
   - Parameters:
       * equals: pointer to '=' character in path; NULL if no value.
       * new_node: pointer to the t_env node being initialized.
   - Returns: 0 on success, 1 if allocation fails or node/key is NULL.
   - Notes: Uses ft_strdup to allocate value; sets value = NULL 
   		if no '=' present. 
   Frees key and node on failure.
*/
int	setting_value(char **equals, t_env **new_node)
{
	if (*new_node == NULL || (*new_node)->key == NULL)
		return (1);
	if (*equals)
	{
		(*new_node)->value = ft_strdup(*equals + 1);
		if (!(*new_node)->value)
		{
			free((*new_node)->key);
			free(*new_node);
			return (1);
		}
	}
	else
	{
		(*new_node)->value = ft_strdup("");
		if (!(*new_node)->value)
		{
			free((*new_node)->key);
			free(*new_node);
			return (1);
		}
	}
	return (0);
}

/* free_keys(key_one, key_two)
   - Purpose: Frees memory allocated for two keys.
   - Parameters:
       * key_one: first key to free.
       * key_two: second key to free.
   - Returns: void.
   - Notes: Used after comparison in alphabatize to avoid memory leaks.
*/
void	free_keys(char *key_one, char *key_two)
{
	free(key_one);
	free(key_two);
}

/* setting_vars(path, equals, new_node)
   - Purpose: Initializes a new t_env node by zeroing its memory and 
   		setting the exported flag.
   - Parameters:
       * path: pointer to the environment string (e.g., "KEY=VALUE").
       * equals: pointer to store the location of '=' in path.
       * new_node: pointer to the t_env node to initialize.
   - Returns: void.
   - Notes: Sets *equals to point to '=' if present; sets exported = 1. 
   	Must be called after malloc.
*/

void	setting_vars(char **path, char **equals, t_env **new_node)
{
	*equals = ft_strchr(*path, '=');
	if (!*new_node)
		return ;
	ft_memset(*new_node, 0, sizeof(t_env));
	(*new_node)->exported = 1;
}
