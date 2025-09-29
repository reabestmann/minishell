/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 17:41:36 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* is_quoted
   Checks if a string starts with a single or double quote.
   @arg: the input string
   Returns: 1 if quoted, 0 otherwise
*/
static int	is_quoted(char *arg)
{
	if (!arg)
		return (0);
	return (arg[0] == '\'' || arg[0] == '"');
}

/* go_home
   Handles "cd" with no argument or "~" expansion.
   @target_dir: pointer to the directory string (may be modified)
   @temp: temporary string to hold expanded HOME path
   @env: environment list
   Cases:
     - If target_dir is NULL → set it to HOME value
     - If target_dir starts with "~" (and not quoted) → expand to HOME
   Returns: 0 on success, 1 on failure
*/
static int	go_home(char **target_dir, char **temp, t_env **env)
{
	if (!*target_dir)
		*target_dir = get_env_value(env, "HOME");
	else if ((*target_dir)[0] == '~' && ((*target_dir)[1] == '\0'
			|| (*target_dir)[1] == '/') && !is_quoted(*target_dir))
	{
		*temp = expand_home(env, *target_dir);
		if (!*temp)
			return (1);
	}
	return (0);
}

/* display_oldpwd
   Handles "cd -" (switch to previous directory).
   @env: environment list
   @temp: pointer to store duplicated OLDPWD value
   Behavior:
     - If OLDPWD is not set → print error and return 1
     - Otherwise → copy OLDPWD into temp and print it
   Returns: 0 on success, 1 on failure
*/
static int	display_oldpwd(t_env **env, char **temp)
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
	ft_putendl_fd(*temp, 1);
	return (0);
}

/* cd_core
   Executes the directory change logic.
   @target_dir: directory to change into
   @temp: optional temporary expansion string (freed here if used)
   @env: environment list (updated with new PWD/OLDPWD)
   Steps:
     - Save old working directory
     - Attempt chdir() into target_dir
     - On success → update OLDPWD and PWD in env
   Returns: 0 on success, 1 on error
*/
static int	cd_core(char **target_dir, char **temp, t_env **env)
{
	char	*oldpwd;
	char	*newpwd;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (1);
	if (chdir(*target_dir) == -1)
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
	if (*temp)
		free(*temp);
	return (0);
}

int	cd_cmd(t_command *cmd, t_env **env)
{
	char	*target_dir;
	char	*temp;

	temp = NULL;
	target_dir = cmd->args[1];
	if (go_home(&target_dir, &temp, env) == 1)
		return (1);
	trim_quotes_for_execution(cmd->args);
	if (cmd->args[1])
		target_dir = cmd->args[1];
	if (ft_strncmp(target_dir, "-", 1) == 0 && target_dir[1] == '\0')
		if (display_oldpwd(env, &temp) == 1)
			return (1);
	if (temp)
		target_dir = temp;
	if (!target_dir)
		return (1);
	if (cd_core(&target_dir, &temp, env))
		return (1);
	return (0);
}
