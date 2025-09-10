/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 17:11:44 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void add_pwd_node(t_env **paths, char *pwd, char *old_or_new)
{

	while(paths != NULL && paths->next != NULL)
		paths = paths->next;
	paths->next = malloc(sizeof(t_env));
	if (!paths->next)
		return;
	paths = paths->next;
	paths->key = ft_strdup(old_or_new);
	paths->value = ft_strdup(pwd);
	paths->next = NULL;
}
/**
void add_oldpwd_node(t_env *paths, char *oldpwd)
{
	while(paths != NULL && paths->next != NULL)
		paths = paths->next;
	paths->next = malloc(sizeof(t_env));
	if(!paths->next)
		return;
	paths = paths->next;
	paths->key = ft_strdup("OLDPWD");
	paths->value = ft_strdup(oldpwd);
	paths->next = NULL;
}*/

void update_oldpwd(t_env *env, char *oldpwd)
{
	t_env *paths = env;
	while (paths != NULL && ft_strncmp(paths->key, "OLDPWD", 6) != 0)
		paths = paths->next;
	if (paths == NULL)
		add_pwd_node(paths, oldpwd, "OLDPWD");
	else
	{
		free(paths->value);
		paths->value = ft_strdup(oldpwd);
	}
}

void update_newpwd(t_env **env, char *new_pwd)
{
	t_env *paths = env;
	while (paths != NULL && ft_strncmp(paths->key, "PWD", 3) != 0)
		paths = paths->next;
	if (paths == NULL)
		add_pwd_node(paths, oldpwd, "PWD");
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

	i = 0;
	if (cmd->args[i] != NULL && ft_strncmp(cmd->args[i], "cd", 2) == 0 && cmd->args[i][2] == '\0')
        i++;
    else
		return(1);
	oldpwd = getcwd(NULL, 0);
	if (chdir(cmd->args[i]) == -1) //maybe need to duplicate this from args
		return(1);
	update_oldpwd(env, oldpwd);
	newpwd = getcwd(NULL, 0);
	update_newpwd(env, newpwd);
	free(oldpwd);
	free(newpwd);
	return(0);
}