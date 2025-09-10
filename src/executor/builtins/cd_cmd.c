/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 18:27:53 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void add_pwd_node(t_env **paths, char *pwd, char *old_or_new)
{
	t_env *new_node;
	t_env *temp;

	new_node = malloc(sizeof(t_env));
	if(!new_node)
		return;
	new_node->key = ft_strdup(old_or_new);
	new_node->value = ft_strdup(pwd);
	new_node->next = NULL;
	if (*paths == NULL)
		*paths = new_node;
	else
	{
		temp = *paths;
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = new_node;
	}
}

void update_oldpwd(t_env **env, char *oldpwd)
{
	t_env *paths;
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

void update_newpwd(t_env **env, char *new_pwd)
{
	t_env *paths;
	paths = *env;
	while (paths != NULL && ft_strncmp(paths->key, "PWD", 3) != 0)
		paths = paths->next;
	if (paths == NULL)
		add_pwd_node(env, newpwd, "PWD");
	else
	{
		free(paths->value);
		paths->value = ft_strdup(new_pwd);
	}
}

int cd_cmd(t_command *cmd, t_env **env)
{
	int i;
	char *oldpwd;
	char *newpwd;
	char *target_dir;

	i = 0;
	if (cmd->args[i] != NULL && ft_strncmp(cmd->args[i], "cd", 2) == 0 && cmd->args[i][2] == '\0')
        i++;
    else
		return(1);
	target_dir = cmd->args[i];
	if(!target_dir)
		target_dir = getenv("HOME");
	if(!target_dir)
		return(1);
	oldpwd = getcwd(NULL, 0);
	if (chdir(target_dir) == -1)
	{
		perror("cd");
		free(oldpwd);
		return(1);
	}
	update_oldpwd(env, oldpwd);
	newpwd = getcwd(NULL, 0);
	update_newpwd(env, newpwd);
	free(oldpwd);
	free(newpwd);
	return(0);
}