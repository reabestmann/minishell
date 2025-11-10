/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:51:17 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 12:49:36 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* setting_value:
 * Extracts value from a "KEY=VALUE" string into new_node->value.
 * If '=' is missing, sets value to empty string. Returns 0 on success,
 * 1 if allocation fails or node/key is NULL. Frees key/node on failure.
 */
int	setting_value(char **equals, t_env **new_node)
{
	if (*new_node == NULL || (*new_node)->key == NULL)
		return (1);
	if (*equals)
	{
		if (*equals + 1)
			(*new_node)->value = ft_strdup(*equals + 1);
		else
			(*new_node)->value = ft_strdup("");
	}
	else
		(*new_node)->value = NULL;
	return (0);
}

/* free_keys:
 * Frees two allocated key strings. Used after comparison to avoid leaks.
 */
void	free_keys(char *key_one, char *key_two)
{
	free(key_one);
	free(key_two);
}

/* setting_vars:
 * Initializes a new t_env node: zeroes memory, sets exported = 1,
 * finds '=' in path and stores in *equals. Must be called after malloc.
 */
void	setting_vars(char **path, char **equals, t_env **new_node)
{
	*equals = ft_strchr(*path, '=');
	if (!*new_node)
		return ;
	ft_memset(*new_node, 0, sizeof(t_env));
	(*new_node)->exported = 1;
}

/* check_match_and_free:
 * Frees key and returns 0 if match==2, 1 if match==1, -1 otherwise.
 * Used in update_var to propagate result while freeing key.
 */
int	check_match_and_free(int match, char *key)
{
	if (match == 1)
	{
		free(key);
		return (1);
	}
	if (match == 2)
	{
		free(key);
		return (0);
	}
	return (-1);
}

/* check_match:
 * Returns 0 if match==2, 1 if match==1, -1 otherwise.
 * Used to decide return value after checking a node in update_var.
 */
int	check_match(int match)
{
	if (match == 1)
	{
		return (1);
	}
	if (match == 2)
	{
		return (0);
	}
	return (-1);
}
