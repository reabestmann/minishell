/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/23 16:06:53 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static void	add_pwd_node(t_env **paths, char *pwd, char *old_or_new)
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

static void	update_oldpwd(t_env **env, char *oldpwd)
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

static void	update_newpwd(t_env **env, char *new_pwd)
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

char *expand_home(char *target_dir)
{
	char *home;
	char *expanded;

	if(!target_dir)
		return(getenv("HOME"));
	if(ft_strncmp(target_dir, "~", 1) == 0)
	{
		home = getenv("HOME");
		if (!home)
			return(NULL);
		if (target_dir[1] == '/' || target_dir[1] == '\0')
		{
			expanded = malloc(ft_strlen(home) + ft_strlen(target_dir));
			if(!expanded)
				return (NULL);
			ft_strlcpy(expanded, home, ft_strlen(home) + 1);
			ft_strlcat(expanded, target_dir + 1, ft_strlen(home) + ft_strlen(target_dir) + 1);
			return(expanded);
		}
	}
	return(ft_strdup(target_dir));
}

int	cd_cmd(t_command *cmd, t_env **env)
{
	char	*oldpwd;
	char	*newpwd;
	char	*target_dir;
	t_env	*paths;
	char *temp;

	temp = NULL;
	target_dir = cmd->args[1];
	if (!target_dir)
		target_dir = getenv("HOME");
	else if (ft_strncmp(target_dir, "~", 1) == 0 && (target_dir[1] == '\0' || target_dir[1] == '/'))
	{
		temp = expand_home(target_dir);
		/*if (!temp)
			return (1);
		target_dir = temp;*/
	}
	else if (ft_strncmp(target_dir, "-", 1) == 0 && target_dir[1] == '\0')
	{
		paths = *env;
		while(paths && ft_strncmp(paths->key, "OLDPWD", 6) != 0)
			paths = paths->next;
		if (!paths || !paths->value)
		{
			ft_putendl_fd("cd: OLDPWD not set", 2);
			return (1);
		}
		temp = ft_strdup(paths->value);
		ft_putendl_fd(temp, 1);
	}
	if(temp)
		target_dir = temp;
	if (!target_dir)
		return (1);
	oldpwd = getcwd(NULL, 0);
	if(!oldpwd)
		return(1);
	if (chdir(target_dir) == -1)
	{
		perror("cd");
		free(oldpwd);
		return (1);
	}
	update_oldpwd(env, oldpwd);
	newpwd = getcwd(NULL, 0);
	update_newpwd(env, newpwd);
	free(oldpwd);
	free(newpwd);
	return (0);
}
