/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:54 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 18:14:37 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
		if (temp->value)
			free(temp->value);
		if (equals)
		{
			temp->value = ft_strdup(equals + 1);
			if (!temp->value)
				return (1);
		}
		else
		{
			temp->value = ft_strdup("");
			if (!temp->value)
				return (1);
		}
		temp->exported = 1;
		return (2);
	}
	return (0);
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

	if (validate_and_get_key(path, &key_len, &key))
		return (1);
	temp = *env;
	while (temp)
	{
		match = found_match(key, temp, key_len, path);
		if (match == 1)
			return (free(key), 1);
		if (match == 2)
			return (free(key), 0);
		temp = temp->next;
	}
	free(key);
	if (!add_nodes(env, path))
		return (1);
	return (0);
}

/* list_exported(env)
   - Purpose: Prints all exported environment variables in alphabetical order.
   - Parameters:
       * env: pointer to the head of the t_env linked list
   - Returns: 0 on success, 1 if allocation of envp array fails
   - Notes:
       * Converts the linked list to an array using struct_to_envp.
       * Sorts the array alphabetically with ft_bubble_sort.
       * Prints each variable prefixed with "declare -x ".
       * Frees the array after printing to avoid memory leaks.
*/
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
