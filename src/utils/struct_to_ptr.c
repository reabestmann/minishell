/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_to_ptr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:13 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 18:40:02 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* count_nodes(t_env *head, int export_only)
   - Purpose: Counts the number of nodes in a t_env linked list.
              Can optionally count only exported variables.
   - Parameters:
       * head: pointer to the first node of the linked list.
       * export_only: if non-zero, only counts nodes where exported == 1.
   - Returns: Number of nodes matching the criteria.
   - Notes: Used to determine the size of an envp-style array before allocation.
*/
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

/* add_strings(t_env *head, char **envp, int export_only)
   - Purpose: Converts a t_env linked list into a null-terminated array 
   		of strings.
              Each string is of the form "KEY=VALUE".
              Can optionally include only exported variables.
   - Parameters:
       * head: pointer to the first node of the linked list.
       * envp: preallocated array of char * to fill.
       * export_only: if non-zero, only include nodes where exported == 1.
   - Returns: 0 on success, 1 if a malloc failure occurs.
   - Notes: Safely handles nodes with NULL value (shell-only variables 
   		without '=').
            Frees any previously allocated strings on failure.
*/
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

/* struct_to_envp(t_env *head, int export_only)
   - Purpose: Converts a t_env linked list into a null-terminated array 
   		of strings ready to pass to execve() or use in an env-like builtin.
   - Parameters:
       * head: pointer to the first node of the linked list.
       * export_only: if non-zero, only include exported variables.
   - Returns: Pointer to the newly allocated char ** array, or NULL on failure.
   - Notes: Allocates exactly the right number of strings using count_nodes().
            Frees the array on error. Caller is responsible for freeing the 
					returned array.
*/
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
