/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:54 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:03:01 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* add_nodes:
 	Creates a new t_env node from "KEY=VALUE"
 	and appends to list. Returns pointer to new node
 */
static t_env	*add_nodes(t_env **env, char *path)
{
	char	*equals;
	t_env	*new_node;
	t_env	*last;

	new_node = handle_malloc(sizeof(t_env));
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

/* found_match:
	Checks if temp node matches key and updates its value.
	0=no match, 1=malloc failure, 2=match updated.
	Frees key on success; sets exported=1.
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
			if (equals + 1)
				temp->value = ft_strdup(equals + 1);
			else
				temp->value = ft_strdup("");
		}
		else
			temp->value = NULL;
		temp->exported = 1;
		return (2);
	}
	return (0);
}

/* update_var:
	Updates an existing variable
 	or adds a new node if not found.
	match = key is already in env.
 */
int	update_var(char *path, t_env **env)
{
	t_env	*temp;
	char	*key;
	int		match;
	int		key_len;
	int		val;

	val = validate_and_get_key(path, &key_len, &key);
	if (val != 0)
		return (val);
	temp = *env;
	while (temp)
	{
		match = found_match(key, temp, key_len, path);
		if (check_match_and_free(match, key) != -1)
			return (check_match(match));
		temp = temp->next;
	}
	free(key);
	if (!add_nodes(env, path))
		return (1);
	return (0);
}

/* list_exported(env)
   Prints all exported environment variables in alphabetical order.
	Converts env to envp array (struct_to_envp).
    Sorts the array alphabetically (ft_bubble_sort).
    Prints each variable prefixed with "declare -x ".
    Frees the array after printing.
   Returns 0 on success, 1 on malloc failure.
*/
static int	list_exported(t_env **env)
{
	char	**envp;
	int		i;
	char	*eq;

	i = 0;
	envp = struct_to_envp(*env, 1);
	if (!envp)
		return (1);
	ft_bubble_sort(envp);
	while (envp[i])
	{
		eq = ft_strchr(envp[i], '=');
		if (eq)
		{
			printf("declare -x %.*s", (int)(eq - envp[i]), envp[i]);
			printf("=\"%s\"\n", eq +1);
		}
		else
			printf("declare -x %s\n", (envp[i]));
		i++;
	}
	free_array(envp);
	return (0);
}

/* export_cmd:
 	If no args → lists exported variables.
	Otherwise, updates or adds each argument (update_var).
	Returns 0 on success, 1 on malloc failure, 2 for invalid identifier.
 */
int	export_cmd(t_command *cmd, t_env **env)
{
	int	i;
	int	ret;
	int	res;

	i = 1;
	ret = 0;
	if (!cmd || !env)
		return (1);
	if (!cmd->args[1])
		return (list_exported(env));
	while (cmd->args[i])
	{
		res = update_var(cmd->args[i], env);
		if (res == 2)
			ret = 2;
		else if (res == 1)
			ret = 1;
		i++;
	}
	return (ret);
}
