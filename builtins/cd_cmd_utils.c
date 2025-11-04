/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:27:15 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 12:40:16 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* get_env_value
   Searches the environment linked list for a given key.
   @env: pointer to the environment list
   @key: environment variable name to look up
   Returns: the matching value (char *) or NULL if not found
*/
char	*get_env_value(t_env **env, const char *key)
{
	t_env	*current;

	current = *env;
	while (current)
	{
		if (ft_strncmp(current->key, key, ft_strlen(key)) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}

/* add_pwd_node
   Creates and appends a new env node for "PWD" or "OLDPWD".
   @paths: pointer to the env list
   @pwd: value for the new node
   @old_or_new: key ("PWD" or "OLDPWD")
   Returns: nothing
   Notes: allocates memory for both key and value
*/
void	add_pwd_node(t_env **paths, char *pwd, char *old_or_new)
{
	t_env	*new_node;
	t_env	*temp;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return ;
	new_node->key = ft_strdup(old_or_new);
	new_node->value = ft_strdup(pwd);
	new_node->next = NULL;
	if (*paths == NULL)
		*paths = new_node;
	else
	{
		temp = *paths;
		while (temp->next != NULL)
			temp = temp->next;
		temp->next = new_node;
	}
}

/* update_oldpwd
   Updates or creates the "OLDPWD" node in the env list.
   @env: pointer to the env list
   @oldpwd: string value of the previous working directory
   If OLDPWD exists → frees old value and replaces it.
   If not → calls add_pwd_node to create it.
*/
void	update_oldpwd(t_env **env, char *oldpwd)
{
	t_env	*paths;

	paths = *env;
	while (paths != NULL && ft_strncmp(paths->key, "OLDPWD", 6) != 0)
		paths = paths->next;
	if (paths == NULL)
		add_pwd_node(env, oldpwd, "OLDPWD");
	else
	{
		free(paths->value);
		paths->value = ft_strdup(oldpwd);
	}
}

/* update_newpwd
   Updates or creates the "PWD" node in the env list.
   @env: pointer to the env list
   @new_pwd: string value of the new working directory
   If PWD exists → frees old value and replaces it.
   If not → calls add_pwd_node to create it.
*/
void	update_newpwd(t_env **env, char *new_pwd)
{
	t_env	*paths;

	paths = *env;
	while (paths != NULL && ft_strncmp(paths->key, "PWD", 3) != 0)
		paths = paths->next;
	if (paths == NULL)
		add_pwd_node(env, new_pwd, "PWD");
	else
	{
		free(paths->value);
		paths->value = ft_strdup(new_pwd);
	}
}

/* expand_home
   Expands "~" to the HOME directory value from env.
   @env: pointer to the env list
   @target_dir: raw directory string (may start with ~)
   Cases:
     - If target_dir is NULL → return a copy of HOME.
     - If target_dir starts with "~/" or "~" → replace ~ with HOME.
     - Otherwise → return a duplicate of target_dir.
   Returns: newly allocated string or NULL on failure.
*/
char	*expand_home(t_env **env, char *target_dir)
{
	char	*home;
	char	*expanded;

	if (!target_dir)
		return (get_env_value(env, "HOME"));
	if (ft_strncmp(target_dir, "~", 1) == 0)
	{
		home = get_env_value(env, "HOME");
		if (!home)
			return (NULL);
		if (target_dir[1] == '/' || target_dir[1] == '\0')
		{
			expanded = malloc(ft_strlen(home) + ft_strlen(target_dir));
			if (!expanded)
				return (NULL);
			ft_strlcpy(expanded, home, ft_strlen(home) + 1);
			ft_strlcat(expanded, target_dir + 1, ft_strlen(home)
				+ ft_strlen(target_dir) + 1);
			return (expanded);
		}
	}
	return (ft_strdup(target_dir));
}
