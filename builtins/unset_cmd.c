/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:03:05 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:02:32 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* is_valid_unset_key
   - Checks if a key is valid for unset:
       1 → valid
       0 → invalid identifier
      -1 → syntax-like error (contains ;, |, &, <, >, etc.) */

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

/* handle_invalid_key
   - Prints a syntax-like error for invalid keys and returns 127. */
static int	handle_invalid_key(char *arg)
{
	ft_putstr_fd("mini: unset: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd("': command not found", 2);
	return (127);
}

/* handle_invalid_identifier
   - Prints "not a valid identifier" for the key and returns 2. */
static int	handle_invalid_identifier(char *arg)
{
	ft_putstr_fd("mini: unset: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd("': not a valid identifier", 2);
	return (2);
}

/* handle_unset_arg
   - Processes a single unset argument:
       * Validates key
       * Calls remove_env_var if valid
       * Updates `ret` for errors */
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

/* unset_cmd
   - Implements the `unset` builtin:
       * Iterates cmd->args after 'unset'
       * Validates keys and deletes env nodes
       * Returns 0 if all OK, 2 for invalid identifiers, 
	   127 for syntax errors */
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
