/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_add_val.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 13:46:09 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/11 13:49:51 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* handle_append_value:
 * Appends a new value to an existing env variable.
 * Used for syntax VAR+=value.
 * Returns 1 on malloc failure, 0 on success.
 */
static int	handle_append_value(t_env *temp, char *equals)
{
	char	*to_add;
	char	*joined;

	if (equals)
		to_add = ft_strdup(equals + 1);
	else
		to_add = ft_strdup("");
	if (!to_add)
		return (1);
	if (!temp->value)
		temp->value = to_add;
	else
	{
		joined = ft_strjoin(temp->value, to_add);
		free(temp->value);
		free(to_add);
		if (!joined)
			return (1);
		temp->value = joined;
	}
	return (0);
}

/* handle_assign_value:
 * Replaces an env variable's value with a new one.
 * Used for syntax VAR=value or VAR (no value).
 * Returns 1 on malloc failure, 0 on success.
 */
int	set_value(t_env *temp, char *equals)
{
    if (!temp || !temp->key)
        return (1);
    if (temp->value)
	    free(temp->value);
	if (equals)
	{
		if (*(equals + 1))
			temp->value = ft_strdup(equals + 1);
		else
			temp->value = ft_strdup("");
	}
	else
		temp->value = NULL;
	return (0);
}

/* found_match:
	Checks if temp node matches key and updates its value.
	0=no match, 1=malloc failure, 2=match updated.
	Frees key on success; sets exported=1.
 */
int	found_match(char *key, t_env *temp, int key_len, char *path)
{
	char	*equals;
	int		append;
	int		result;

	equals = ft_strchr(path, '=');
	append = 0;
	if (equals && equals > path && *(equals - 1) == '+')
		append = 1;
	if (ft_strncmp(key, temp->key, key_len) == 0 && temp->key[key_len] == '\0')
	{
		result = 0;
		if (append)
			result = handle_append_value(temp, equals);
		else
			result = set_value(temp, equals);
		if (result != 0)
			return (1);
		temp->exported = 1;
		return (2);
	}
	return (0);
}