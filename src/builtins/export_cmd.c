/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:54 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/12 15:22:32 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int setting_key(char **path, char **equals, t_env **new_node)
{
	if(*equals)
		(*new_node)->key = ft_substr(*path, 0, *equals - *path);
	else
		(*new_node)->key = ft_substr(*path, 0, ft_strlen(*path));
	if (!(*new_node)->key)
	{
		free(*new_node);
		return(1);
	}
	return(0);
}

static int setting_value(char **equals, t_env **new_node)
{
	if(*new_node == NULL || (*new_node)->key == NULL)
		return(1);
	if(*equals)
	{
		(*new_node)->value = ft_strdup(*equals + 1);
		if(!(*new_node)->value)
		{
			free((*new_node)->key);
			free(*new_node);
			return(1);
		}
	}
	else
		(*new_node)->value = NULL;
	return(0);
}


static void setting_vars(char **path, char **equals, t_env **new_node)
{
	*equals = ft_strchr(*path, '=');
	if (!*new_node)
		return;
	ft_memset(*new_node, 0, sizeof(t_env));
	(*new_node)->exported = 1;
}

/* add_nodes(t_env *current, char *equals, char *env_str)
   - Purpose: Creates a new t_env node from a "KEY=VALUE" string and appends it to the list.
   - Parameters:
       * current: pointer to the last node in the list; NULL if list is empty.
       * equals: pointer to '=' character in env_str.
       * env_str: full environment string to split into key and value.
   - Returns: pointer to the new node, or NULL on allocation failure.
   - Notes: Allocates memory for key and value. Sets exported = 1. Uses now_free() to handle allocation errors.
*/
static t_env *add_nodes(t_env **env, char *path)
{
	char *equals;
	t_env *new_node;
	t_env *last;

	new_node = malloc(sizeof(t_env));
	if(!new_node)
		return(NULL);
	setting_vars(&path, &equals, &new_node);
	if(setting_key(&path, &equals, &new_node) == 1)
		return(NULL);
	if(setting_value(&equals, &new_node) == 1)
		return(NULL);
	new_node->next = NULL;
	if(!*env)
		*env = new_node;
	else
	{
		last = *env;
		while(last->next)
			last = last->next;
		last->next = new_node;
	}
	return(new_node);
}

static int found_match(char *key, t_env *temp, int key_len, char *path)
{
	char *equals;

	equals = ft_strchr(path, '=');
	if (ft_strncmp(key, temp->key, key_len) == 0 && temp->key[key_len] == '\0')
	{
		free(key);
		if(temp->value)
			free(temp->value);
		if(equals)
		{
			temp->value = ft_strdup(equals + 1);
			if(!temp->value)
				return(1);
		}
		else
			temp->value = NULL;
		temp->exported = 1;
		return(2);
	}
	return(0);
}

static char *find_key(char *path, int *key_len)
{
	char *key;
	char *equals;

	equals = ft_strchr(path, '=');
	if(*equals)
		*key_len = *equals - path;
	else
		*key_len = ft_strlen(path);
	key = ft_substr(path, 0, *key_len);
	if (!key)
		return(NULL);
	return(key);
}

static int update_var(char *path, t_env **env)
{
	t_env *temp;
	char *key;
	int match;
	int key_len;

	key = find_key(path, &key_len);
	if(!key)
		return(1);
	temp = *env;
	while (temp)
	{
		match = found_match(key, temp, key_len, path);
		if(match == 1)
			return(1); //failed ft_strdup
		if(match == 2)
			return(0); //assigned and dont need to add node
		temp = temp->next;		
	}
	free(key);
	if(!add_nodes(env, path))
		return(1);
	return(0);
}

static int list_exported()
{
	/*sort all that are listed as exported by alphabetical and print*/
	return(0);
}

int export_cmd(t_command *cmd, t_env **env)
{
	int i;
	
	i = 1;
    if (!cmd || !env)
        return (1);
	if (!cmd->args[1])
	{
		list_exported();
		printf("export is not available yet\n");
		return(0);
	}
	while(cmd->args[i])
	{
		update_var(cmd->args[i], env);
		i++;
	}
	return (0);
}