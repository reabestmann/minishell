/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd_keys.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:47:56 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:03:25 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* is_valid_export_key:
 * Validates a variable name for export 
 	(letters/nums/_, cannot start with digit).
 * Stops at '=' or '+='. Stores key length in *len. Returns 1 if valid, else 0.
 */
int	is_valid_export_key(char *arg, int *len)
{
	int	i;

	i = 0;
	if (!arg || arg[0] == '\0')
		return (0);
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
		return (0);
	i++;
	while (arg[i] != '\0' && arg[i] != '=')
	{
		if (arg[i] == '+' && arg[i + 1] == '=')
			break ;
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	*len = i;
	return (1);
}

/* setting_key:
 * Extracts and stores the key from a VAR=VALUE string into new_node->key.
 * Uses '=' position if present. Returns 0 on success, 1 on allocation error.
 */
int	setting_key(char **path, char **equals, t_env **new_node)
{
	if (*equals)
		(*new_node)->key = ft_substr(*path, 0, *equals - *path);
	else
		(*new_node)->key = ft_substr(*path, 0, ft_strlen(*path));
	if (!(*new_node)->key)
	{
		free(*new_node);
		return (1);
	}
	return (0);
}

/* find_key:
 * Extracts key substring from VAR=VALUE (or whole string if no '=').
 * Stores key length in *key_len. Returns allocated key or NULL on failure.
 */
char	*find_key(char *path, int *key_len)
{
	char	*key;
	char	*equals;

	equals = ft_strchr(path, '=');
	if (equals)
		*key_len = equals - path;
	else
		*key_len = ft_strlen(path);
	key = ft_substr(path, 0, *key_len);
	if (!key)
		return (NULL);
	return (key);
}

/* validate_and_get_key:
 * Validates export key syntax and extracts key.
 * Prints error for invalid identifier.
 * On success: allocates key (caller frees), returns 0.
 * Returns 1/2 on validation or allocation failure.
 */
int	validate_and_get_key(char *path, int *key_len, char **key)
{
	if (!is_valid_export_key(path, key_len))
	{
		ft_putstr_fd("mini: export: `", 2);
		ft_putstr_fd(path, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		if (path[0] == '-')
			return (2);
		return (1);
	}
	*key = find_key(path, key_len);
	if (!*key)
		return (1);
	return (0);
}

/* key_info:
 * Extracts keys of envp[j] and envp[j+1], returns length of shorter key.
 * Used for export sorting/compare. Caller frees keys.
 */
int	key_info(char **envp, int j, char **key_one, char **key_two)
{
	int	len_one;
	int	len_two;

	*key_one = find_key(envp[j], &len_one);
	*key_two = find_key(envp[j + 1], &len_two);
	if (len_one < len_two)
		return (len_one);
	else
		return (len_two);
}
