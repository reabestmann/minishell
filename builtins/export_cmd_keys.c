/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd_keys.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:47:56 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/13 17:43:51 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* is_valid_export_key(arg, len)
   - Purpose: Checks if a string is a valid environment variable name.
   - Rules: Must start with a letter or underscore; can contain letters, 
   digits, underscores; stops at '='.
   - Parameters:
       * arg: the string to validate
       * len: pointer to store the length of the key portion
   - Returns: 1 if valid, 0 if invalid
*/
/*OG*/
/*
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
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	*len = i;
	return (1);
}*/
int is_valid_export_key(char *arg, int *len)
{
    int i = 0;

    if (!arg || arg[0] == '\0')
        return 0;

    if (!ft_isalpha(arg[i]) && arg[i] != '_')
        return 0;
    i++;

    while (arg[i] != '\0' && arg[i] != '=')
    {
        // allow + only immediately before '='
        if (arg[i] == '+' && arg[i+1] == '=')
            break;

        if (!ft_isalnum(arg[i]) && arg[i] != '_')
            return 0;

        i++;
    }

    *len = i;  // length of the key, excluding + and =
    return 1;
}


/* setting_key(path, equals, new_node)
   - Purpose: Extracts the key from a "KEY=VALUE" string and stores it in a 
   t_env node.
   - Parameters:
       * path: pointer to the environment string
       * equals: pointer to the '=' character in the string (or NULL)
       * new_node: pointer to the node where key is set
   - Returns: 0 on success, 1 if memory allocation fails
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

/* find_key(path, key_len)
   - Purpose: Extracts the key from a "KEY=VALUE" string for comparison.
   - Parameters:
       * path: environment string (e.g., "KEY=VALUE").
       * key_len: pointer to int to store length of key.
   - Returns: newly allocated key string, or NULL on allocation failure.
   - Notes: Used in update_var to check for existing variables in the list.
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

/* validate_and_get_key(path, key_len, key)
   - Purpose: Validates that a given string is a valid environment variable key
              and extracts the key for further processing.
   - Parameters:
       * path: input string in the form "KEY" or "KEY=VALUE"
       * key_len: pointer to an int to store the length of the key
       * key: pointer to store the newly allocated key string
   - Returns: 0 if the key is valid and successfully extracted, 1 on invalid
              key or allocation failure
   - Notes:
       * Uses is_valid_key to check key syntax (letters, digits, underscores, 
         starting with a letter or underscore).
       * Allocates memory for the key using find_key; caller must free it.
       * Prints an error message to stderr if the key is invalid.
*/
int validate_and_get_key(char *path, int *key_len, char **key)
{
    if (!is_valid_export_key(path, key_len))
    {
        ft_putstr_fd("mini: export: `", 2);
        ft_putstr_fd(path, 2);
        ft_putendl_fd("': not a valid identifier", 2);

        // if starts with '-', exit code = 2
        if (path[0] == '-')
            return 2;
        return 1;
    }
    *key = find_key(path, key_len);
    if (!*key)
        return 1;
    return 0;
}
/*OG*/
/*int	validate_and_get_key(char *path, int *key_len, char **key)
{
	if (!is_valid_export_key(path, key_len))
	{
		ft_putstr_fd("mini: export: `", 2);
		ft_putstr_fd(path, 2);
		ft_putendl_fd("': not a valid identifier", 2);
		return (1);
	}
	*key = find_key(path, key_len);
	if (!*key)
		return (1);
	return (0);
}*/

/* key_info(envp, j, key_one, key_two)
   - Purpose: Extracts the keys from two adjacent envp strings and returns 
   the minimum key length.
   - Parameters:
       * envp: array of environment strings.
       * j: index of the first string.
       * key_one: pointer to store the first key (allocated inside function).
       * key_two: pointer to store the second key (allocated inside function).
   - Returns: length of the shorter key.
   - Notes: Keys must be freed by the caller.
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
