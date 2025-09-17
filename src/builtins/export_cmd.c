/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:54 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/17 16:37:03 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* setting_key(path, equals, new_node)
   - Purpose: Extracts the key part from a "KEY=VALUE" string and assigns it to 
   		the node.
   - Parameters:
       * path: pointer to the environment string.
       * equals: pointer to '=' character in path.
       * new_node: pointer to the t_env node being initialized.
   - Returns: 0 on success, 1 if allocation fails.
   - Notes: Uses ft_substr to allocate memory for key. Frees node on failure.
*/
static int	setting_key(char **path, char **equals, t_env **new_node)
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
static int	setting_value(char **equals, t_env **new_node)
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
		(*new_node)->value = NULL;
	return (0);
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

static void	setting_vars(char **path, char **equals, t_env **new_node)
{
	*equals = ft_strchr(*path, '=');
	if (!*new_node)
		return ;
	ft_memset(*new_node, 0, sizeof(t_env));
	(*new_node)->exported = 1;
}

/* add_nodes(env, path)
   - Purpose: Creates a new t_env node from a "KEY=VALUE" string and 
   	appends it to the linked list.
   - Parameters:
       * env: pointer to head of the t_env list; may be NULL if list 
	   is empty.
       * path: environment string to parse (e.g., "KEY=VALUE").
   - Returns: pointer to the new node, or NULL if allocation fails.
   - Notes: Calls setting_vars, setting_key, and setting_value. Adds 
   new node to end of list and sets exported = 1.
*/
static t_env	*add_nodes(t_env **env, char *path)
{
	char	*equals;
	t_env	*new_node;
	t_env	*last;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	setting_vars(&path, &equals, &new_node);
	if (setting_key(&path, &equals, &new_node) == 1)
		return (NULL);
	if (setting_value(&equals, &new_node) == 1)
		return (NULL);
	new_node->next = NULL;
	if (!*env)
		*env = new_node;
	else
	{
		last = *env;
		while (last->next)
			last = last->next;
		last->next = new_node;
	}
	return (new_node);
}

/* found_match(key, temp, key_len, path)
   - Purpose: Checks if a node matches a key and updates its value if necessary.
   - Parameters:
       * key: key string to match.
       * temp: pointer to current node in list.
       * key_len: length of key.
       * path: full "KEY=VALUE" string to update value from.
   - Returns: 
       0 → no match,
       1 → allocation failure,
       2 → match found and value updated.
   - Notes: Frees key after successful match. Updates exported = 1. 
   Uses ft_strdup for value allocation.
*/
static int	found_match(char *key, t_env *temp, int key_len, char *path)
{
	char	*equals;

	equals = ft_strchr(path, '=');
	if (ft_strncmp(key, temp->key, key_len) == 0 && temp->key[key_len] == '\0')
	{
		free(key);
		if (temp->value)
			free(temp->value);
		if (equals)
		{
			temp->value = ft_strdup(equals + 1);
			if (!temp->value)
				return (1);
		}
		else
			temp->value = NULL;
		temp->exported = 1;
		return (2);
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
static char	*find_key(char *path, int *key_len)
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

/* update_var(path, env)
   - Purpose: Updates an existing variable if key exists; otherwise 
   adds a new node.
   - Parameters:
       * path: environment string ("KEY=VALUE" or "KEY").
       * env: pointer to head of the t_env list.
   - Returns: 0 on success, 1 on allocation failure.
   - Notes: Calls find_key to get key, found_match to update existing nodes, 
   and add_nodes to create new nodes.
*/
static int	update_var(char *path, t_env **env)
{
	t_env	*temp;
	char	*key;
	int		match;
	int		key_len;

	key = find_key(path, &key_len);
	if (!key)
		return (1);
	temp = *env;
	while (temp)
	{
		match = found_match(key, temp, key_len, path);
		if (match == 1)
			return (free(key), 1); //failed ft_strdup
		if (match == 2)
			return (free(key), 0); //assigned and dont need to add node
		temp = temp->next;
	}
	free(key);
	if (!add_nodes(env, path))
		return (1);
	return (0);
}

/* sort(envp, j)
   - Purpose: Swaps two adjacent strings in the envp array.
   - Parameters:
       * envp: array of environment strings.
       * j: index of the first string to swap with the next.
   - Returns: void.
   - Notes: Used by alphabatize to reorder strings alphabetically.
*/
static void	sort(char **envp, int j)
{
	char	*temp;

	temp = envp[j];
	envp[j] = envp[j + 1];
	envp[j + 1] = temp;
	return ;
}

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
static int	key_info(char **envp, int j, char **key_one, char **key_two)
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

/* free_keys(key_one, key_two)
   - Purpose: Frees memory allocated for two keys.
   - Parameters:
       * key_one: first key to free.
       * key_two: second key to free.
   - Returns: void.
   - Notes: Used after comparison in alphabatize to avoid memory leaks.
*/
static void	free_keys(char *key_one, char *key_two)
{
	free(key_one);
	free(key_two);
}

/* keys_differ(key_one, key_two, envp, j)
   - Purpose: Handles the case when two keys differ and the first should 
   come after the second.
   - Parameters:
       * key_one: first key string (to be freed).
       * key_two: second key string (to be freed).
       * envp: array of environment strings.
       * j: index of the first string to swap.
   - Returns: 1 to indicate a swap occurred.
   - Notes: Frees allocated keys and calls sort to swap the strings in envp.
*/
static int	keys_differ(char *key_one, char *key_two, char **envp, int j)
{
	free_keys(key_one, key_two);
	sort(envp, j);
	return (1);
}

/* alphabatize(envp, j)
   - Purpose: Compares two adjacent env strings and swaps them if they are out 
   of order.
   - Parameters:
       * envp: array of environment strings.
       * j: index of the first string to compare.
   - Returns: 1 if a swap occurred, 0 if already in order.
   - Notes: Uses keys_differ() to handle keys of different lengths and free 
   memory.
*/
static int	alphabatize(char **envp, int j)
{
	char	*key_one;
	char	*key_two;
	int		i;
	int		min_len;

	i = 0;
	min_len = key_info(envp, j, &key_one, &key_two);
	while (i < min_len)
	{
		if (key_one[i] > key_two[i])
			return (keys_differ(key_one, key_two, envp, j));
		else if (key_one[i] < key_two[i])
		{
			free_keys(key_one, key_two);
			return (0);
		}
		i++;
	}
	if (ft_strlen(key_one) > ft_strlen(key_two))
		return (keys_differ(key_one, key_two, envp, j));
	free_keys(key_one, key_two);
	return (0);
}

/* ft_bubble_sort(envp)
   - Purpose: Sorts an array of environment strings alphabetically using 
   bubble sort.
   - Parameters:
       * envp: array of environment strings.
   - Returns: void.
   - Notes: Optimized to stop early if the array is already sorted (swap flag).
    Uses alphabatize for comparisons.
*/
static void	ft_bubble_sort(char **envp)
{
	int	i;
	int	j;
	int	n;
	int	swap;

	n = 0;
	while (envp[n] != NULL)
		n++; //as many times as there are strings
	i = 0;
	while (i < n - 1)
	{
		swap = 0;
		j = 0;
		while (j < n - i - 1) //until we get to end of array
		{
			if (alphabatize(envp, j))
				swap = 1;
			j++;
		}
		if (!swap)
			return ;
		i++;
	}
}

static int	list_exported(t_env **env)
{
	char	**envp;
	int		i;

	i = 0;
	envp = struct_to_envp(*env, 1);
	if (!envp)
		return (1);
	ft_bubble_sort(envp);
	while (envp[i])
	{
		printf("declare -x %s\n", envp[i]);
		i++;
	}
	free_array(envp);
	return (0);
}

/* export_cmd(cmd, env)
   - Purpose: Implements the `export` shell builtin.
   - Parameters:
       * cmd: command struct containing arguments (cmd->args[0] = "export").
       * env: pointer to head of the t_env list.
   - Returns: 0 on success, 1 on failure.
   - Notes: 
       - If no arguments, calls list_exported.
       - Otherwise, loops through cmd->args and calls update_var 
	   for each argument.
       - Updates existing variables or adds new nodes.
*/
int	export_cmd(t_command *cmd, t_env **env)
{
	int	i;
	int	ret;

	i = 1;
	ret = 0;
	if (!cmd || !env)
		return (1);
	if (!cmd->args[1])
		return (list_exported(env));
	while (cmd->args[i])
	{
		if (update_var(cmd->args[i], env) != 0)
			ret = 1;
		i++;
	}
	return (ret);
}
