/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_to_ptr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:13 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 14:01:48 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* count_nodes:
**   Counts nodes in a t_env list. 
**   If export_only is non-zero, counts only exported nodes.*/
static int	count_nodes(t_env *head, int export_only)
{
	int		count;
	t_env	*temp;

	count = 0;
	temp = head;
	while (temp)
	{
		if (!export_only || temp->exported)
			count++;
		temp = temp->next;
	}
	return (count);
}

/* add_strings_help:
**   Creates "KEY=VALUE" string for a single node in envp array.
**   Returns 1 on malloc failure, 0 on success.*/
static int	add_strings_help(t_env *temp, char **envp, int *i)
{
	int		buff;
	int		val_len;

	if (!temp->key)
		return (1);
	if (temp->value)
		val_len = ft_strlen(temp->value);
	else
		val_len = 0;
	buff = (ft_strlen(temp->key) + val_len + 2);
	envp[(*i)] = malloc(buff);
	if (!envp[(*i)])
		return (1);
	ft_strlcpy(envp[(*i)], temp->key, buff);
	ft_strlcat(envp[(*i)], "=", buff);
	if (temp->value)
		ft_strlcat(envp[(*i)], temp->value, buff);
	return (0);
}

/* add_strings:
**   Fills preallocated envp array with "KEY=VALUE" strings from t_env list.
**   Returns 1 on failure, 0 on success.*/
static int	add_strings(t_env *head, char **envp, int export_only)
{
	int		i;
	t_env	*temp;

	i = 0;
	temp = head;
	while (temp)
	{
		if (!export_only || temp->exported)
		{
			if (add_strings_help(temp, envp, &i) == 1)
			{
				free_array(envp);
				return (1);
			}
			i++;
		}
		temp = temp->next;
	}
	return (0);
}

/* struct_to_envp:
**   Converts t_env list to NULL-terminated char ** array.
**   If export_only is non-zero, includes only exported nodes.
**   Returns allocated array, or NULL on failure.*/
char	**struct_to_envp(t_env *head, int export_only)
{
	char	**envp;
	int		struct_size;

	struct_size = count_nodes(head, export_only);
	envp = malloc(sizeof(char *) * (struct_size + 1));
	if (!envp)
		return (NULL);
	envp[struct_size] = NULL;
	if (add_strings(head, envp, export_only) == 1)
	{
		free_array(envp);
		return (NULL);
	}
	return (envp);
}
