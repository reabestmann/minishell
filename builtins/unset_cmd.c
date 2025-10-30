/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:03:05 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/30 20:22:23 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* find_and_delete_node(t_env **env, t_env *crnt, t_env **prev, char *key)
   - Purpose: Checks a single t_env node against a key and deletes it if it 
   matches.
   - Variables:
     * env: pointer to the head of the linked list.
     * crnt: the current node being checked.
     * prev: pointer to the previous node, used for relinking.
     * key: the environment variable key to match.
     * next: stores crnt->next to continue iteration safely after deletion.
   - Behavior:
     * Compares crnt->key to key.
     * If it matches, removes crnt from the list, updates pointers, and frees 
	 * memory.
     * If not, updates prev to point to crnt.
   - Returns: pointer to the next node for continuing iteration.
*/
t_env	*find_and_delete_node(t_env **env, t_env *crnt, t_env **prev, char *key)
{
	t_env	*next;

	next = crnt->next;
	if (ft_strncmp(crnt->key, key, ft_strlen(key)) == 0
		&& ft_strlen(crnt->key) == ft_strlen(key))
	{
		if (*prev == NULL)
			*env = next;
		else
			(*prev)->next = next;
		free(crnt->value);
		free(crnt->key);
		free(crnt);
	}
	else
		*prev = crnt;
	return (next);
}

/* unset_cmd(t_command *cmd, t_env **env)
   - Purpose: Removes one or more enviro variables from the t_env linked list.
   - Variables:
     * cmd: command structure containing the arguments to unset.
     * env: pointer to the head of the linked list.
     * current: node currently being checked.
     * previous: previous node for relinking the list.
     * i: index for iterating through cmd->args.
   - Behavior:
     * Iterates over all arguments after 'unset' in cmd->args.
     * For each argument, iterates through the linked list and 
	 * 		calls find_and_delete_node 
	 * to remove matching nodes.
     * Supports multiple args and multiple nodes with the same key.
   - Returns: 0 on success, 1 if cmd or env is NULL.
*/
/*OG*/
/*mostly works*/
/*int is_valid_unset_key(char *key)
{
    int i;

    if (!key || key[0] == '\0')
        return 0; // empty string ignored

    // Check for truly invalid characters for unset (like ; or whitespace)
    i = 0;
    while (key[i])
    {
        if (key[i] == ';' || key[i] == ' ' || key[i] == '\t' || key[i] == '\n')
            return 0; // invalid
        i++;
    }

    return 1; // everything else is valid
}*/

/*OG*/
/*int	unset_cmd(t_command *cmd, t_env **env)
{
	t_env	*current;
	t_env	*previous;
	int		i;
	int ret;
	ret = 0;

	if (!cmd || !env)
		return (1);
	i = 1;
	while (cmd->args[i])
	{
		current = *env;
		previous = NULL;
		while (current != NULL)
			current = find_and_delete_node(env, current,
					&previous, cmd->args[i]);
		i++;
	}
	return (ret);
}*/
int	is_valid_unset_key(char *key)
{
	int	i;

	if (!key || key[0] == '\0')
		return (0);
	if (key[0] == '-')
		return (0);
	i = 0;
	while (key[i])
	{
		if (key[i] == ';' || key[i] == '|' || key[i] == '&'
			|| key[i] == '>' || key[i] == '<')
			return (-1);
		i++;
	}
	return (1);
}


static int	handle_invalid_key(char *arg)
{
	ft_putstr_fd("mini: unset: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd("': command not found", 2);
	return (127);
}

static int	handle_invalid_identifier(char *arg)
{
	ft_putstr_fd("mini: unset: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd("': not a valid identifier", 2);
	return (2);
}

static void	remove_env_var(t_env **env, char *key)
{
	t_env	*current;
	t_env	*previous;

	current = *env;
	previous = NULL;
	while (current)
		current = find_and_delete_node(env, current, &previous, key);
}

static void	handle_unset_arg(t_command *cmd, t_env **env, int *ret, int *i)
{
	int	valid;
	int	err;

	valid = is_valid_unset_key(cmd->args[*i]);
	if (valid == -1)
	{
		*ret = handle_invalid_key(cmd->args[*i]);
		(*i)++;
		return ;
	}
	else if (!valid)
	{
		err = handle_invalid_identifier(cmd->args[*i]);
		if (*ret < err)
			*ret = err;
		(*i)++;
		return ;
	}
	remove_env_var(env, cmd->args[*i]);
	(*i)++;
}

int	unset_cmd(t_command *cmd, t_env **env)
{
	int	i;
	int	ret;

	if (!cmd || !env)
		return (1);
	if (!cmd->args[1])
		return (0); 
	ret = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] == '\0')
		{
			i++;
			continue ;
		}
		handle_unset_arg(cmd, env, &ret, &i);
	}
	return (ret);
}

// 0 if all ok, 2 for invalid identifiers, 127 for syntax-like errors
/*int	unset_cmd(t_command *cmd, t_env **env)
{
	t_env	*current;
	t_env	*previous;
	int		i;
	int		ret;
	int		valid;

	ret = 0;
	if (!cmd || !env)
		return (1);
	i = 1;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] == '\0')
		{
			i++;
			continue ;
		}
		valid = is_valid_unset_key(cmd->args[i]);
		if (valid == -1)
		{
			ft_putstr_fd("mini: unset: `", 2);
			ft_putstr_fd(cmd->args[i], 2);
			ft_putendl_fd("': command not found", 2);
			ret = 127;
			i++;
			continue ;
		}
		else if (!valid)
		{
			ft_putstr_fd("mini: unset: `", 2);
			ft_putstr_fd(cmd->args[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			if (ret < 2)
				ret = 2;
			i++;
			continue ;
		}
		current = *env;
		previous = NULL;
		while (current)
			current = find_and_delete_node(env, current,
					&previous, cmd->args[i]);
		i++;
	}
	return (ret);
}*/
/*
int	unset_cmd(t_command *cmd, t_env **env)
{
	t_env	*current;
	t_env	*previous;
	int		i;
	int		ret;

	if (!cmd || !env)
		return (1);

	i = 1;
	ret = 0;
	while (cmd->args[i])
	{
		// Skip empty strings silently
		if (cmd->args[i][0] == '\0')
		{
			i++;
			continue;
		}

		// Check if key is valid
		if (!is_valid_unset_key(cmd->args[i]))
		{
			ft_putstr_fd("mini: unset: `", 2);
			ft_putstr_fd(cmd->args[i], 2);
			ft_putendl_fd("': not a valid identifier", 2);
			ret = 1; // mark invalid keys
		}
		else
		{
			current = *env;
			previous = NULL;
			while (current != NULL)
				current = find_and_delete_node(env, current,
				 &previous, cmd->args[i]);
		}
		i++;
	}
	return (ret);
}*/
