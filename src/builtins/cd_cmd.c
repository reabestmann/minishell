/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/25 17:44:21 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	free_new_node(t_env *new_node)
{
	free(new_node->key);
	free(new_node->value);
	free(new_node);
	return (1);
}

static int	add_pwd_node(t_env **paths, char *pwd, char *old_or_new)
{
	t_env	*new_node;
	t_env	*temp;

	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (1);
	new_node->key = ft_strdup(old_or_new);
	new_node->value = ft_strdup(pwd);
	if (!new_node->key || !new_node->value)
		return (free_new_node(new_node));
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
	return (0);
}

static int	update_oldpwd(t_env **env, char *oldpwd)
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
	if (!paths || !paths->value)
		return (1);
	return (0);
}

static int	update_newpwd(t_env **env, char *new_pwd)
{
	t_env	*paths;

	paths = *env;
	while (paths != NULL && ft_strncmp(paths->key, "PWD", 3) != 0)
		paths = paths->next;
	if (paths == NULL)
		return (add_pwd_node(env, new_pwd, "PWD"));
	free(paths->value);
	paths->value = ft_strdup(new_pwd);
	if (!paths->value)
		return (1);
	return (0);
}

char	*expand_home(char *target_dir)
{
	char	*home;
	char	*expanded;

	if (!target_dir)
		return (getenv("HOME"));
	if (ft_strncmp(target_dir, "~", 1) == 0)
	{
		home = getenv("HOME");
		if (!home)
			return (NULL);
		if (target_dir[1] == '/' || target_dir[1] == '\0')
		{
			expanded = malloc(ft_strlen(home) + ft_strlen(target_dir) + 1);
			if (!expanded)
				return (NULL);
			ft_strlcpy(expanded, home, ft_strlen(home) + 1);
			ft_strlcat(expanded, target_dir + 1,
				ft_strlen(home) + ft_strlen(target_dir) + 1);
			return (expanded);
		}
	}
	return (ft_strdup(target_dir));
}

static int	is_quoted(char *arg)
{
	if (!arg)
		return (0);
	return (arg[0] == '\'' || arg[0] == '"');
}

int	get_oldpwd_for_cd(t_env **env, char **temp)
{
	t_env	*paths;

	paths = *env;
	while (paths && ft_strncmp(paths->key, "OLDPWD", 6) != 0)
		paths = paths->next;
	if (!paths || !paths->value)
	{
		ft_putendl_fd("cd: OLDPWD not set", 2);
		return (1);
	}
	*temp = ft_strdup(paths->value);
	if (!(*temp))
		return (1);
	ft_putendl_fd(*temp, 1);
	return (0);
}

int	special_cases(char **target_dir, char **temp, t_command *cmd, t_env **env)
{
	if ((*target_dir)[0] == '~' && ((*target_dir)[1] == '\0' ||
	*target_dir[1] == '/') && !is_quoted(*target_dir))
	{
		*temp = expand_home(*target_dir);
		if (!*temp)
			return (1);
	}
	trim_quotes_for_execution(cmd->args);
	if (cmd->args[1] && !is_quoted(cmd->args[1]))
		*target_dir = cmd->args[1];
	if (ft_strncmp(*target_dir, "-", 1) == 0 && (*target_dir)[1] == '\0')
	{
		if (get_oldpwd_for_cd(env, temp) == 1)
			return (1);
	}
	return (0);
}

int	update_opwd_chdir(char *oldpwd, char *temp, char *target_dir, t_env **env)
{
	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
	{
		if (temp)
			free(temp);
		return (1);
	}
	if (chdir(target_dir) == -1)
	{
		perror("cd");
		free(oldpwd);
		if (temp)
			free(temp);
		return (1);
	}
	if (update_oldpwd(env, oldpwd))
	{
		free(oldpwd);
		if (temp)
			free(temp);
		return (1);
	}
	free(oldpwd);
	return (0);
}

int	cd_core(char *target_dir, char *temp, t_env **env)
{
	char	*newpwd;
	char	*oldpwd;

	oldpwd = NULL;
	if (update_opwd_chdir(oldpwd, temp, target_dir, env) == 1)
		return (1);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (1);
	else
	{
		if (update_newpwd(env, newpwd))
		{
			free(newpwd);
			if (temp)
				free(temp);
			return (1);
		}
		free(newpwd);
	}
	if (temp)
		free(temp);
	return (0);
}

int	cd_cmd(t_command *cmd, t_env **env)
{
	char	*target_dir;
	char	*temp;

	temp = NULL;
	target_dir = cmd->args[1];
	if (!target_dir)
		target_dir = getenv("HOME");
	if (special_cases(&target_dir, &temp, cmd, env))
		return (1);
	if (temp && target_dir != temp)
		target_dir = temp;
	if (!target_dir)
		return (1);
	if (cd_core(target_dir, temp, env))
		return (1);
	return (0);
}
