/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:01:46 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
/* cd_core:
 * Changes directory to target_dir and updates PWD/OLDPWD in env.
 * Returns 0 on success, 1 on failure.
 */
static int	cd_core(char *target_dir, t_env **env)
{
	char	*oldpwd;
	char	*newpwd;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (1);
	if (chdir(target_dir) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		perror(target_dir);
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

/* too_many_args:
 * Checks if cd has extra arguments beyond the first.
 * Returns 1 and prints error if too many, 0 otherwise.
 */
int	too_many_args(t_command *cmd)
{
	if (cmd->args[2] != NULL)
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		return (1);
	}
	return (0);
}

/* update_pwd_and_print:
 * Updates PWD/OLDPWD after cd - and prints the new directory.
 * Returns 0 on success, 1 if getcwd fails.
 */
static int	update_pwd_and_print(t_env **env, char *pwd_before)
{
	char	*pwd_after;

	pwd_after = getcwd(NULL, 0);
	if (!pwd_after)
		return (1);
	update_oldpwd(env, pwd_before);
	update_newpwd(env, pwd_after);
	ft_putendl_fd(pwd_after, 1);
	free(pwd_after);
	free(pwd_before);
	return (0);
}

/* cd_oldpwd_check:
 * Handles 'cd -' by changing to OLDPWD and printing it.
 * Returns 1 on error, 0 on success.
 */
int	cd_oldpwd_check(char *target_dir, t_env **env)
{
	char	*oldpwd_val;
	char	*pwd_before;

	if (ft_strncmp(target_dir, "-", 1) == 0 && target_dir[1] == '\0')
	{
		oldpwd_val = get_env_value(env, "OLDPWD");
		if (!oldpwd_val)
		{
			ft_putendl_fd("cd: OLDPWD not set", 2);
			return (1);
		}
		pwd_before = getcwd(NULL, 0);
		if (!pwd_before)
			return (1);
		if (chdir(oldpwd_val) == -1)
		{
			perror(oldpwd_val);
			free(pwd_before);
			return (1);
		}
		return (update_pwd_and_print(env, pwd_before));
	}
	return (0);
}

/* cd_cmd:
 * Main cd command handler.
 * Handles too many args, cd -, cd --, no args, and normal directory changes.
 * Returns 0 on success, 1 on failure.
 */
int	cd_cmd(t_command *cmd, t_env **env)
{
	char	*target_dir;
	char	*home;

	target_dir = cmd->args[1];
	if (too_many_args(cmd))
		return (1);
	if (target_dir && ft_strncmp(target_dir, "-", 2) == 0)
		return (cd_oldpwd_check(target_dir, env));
	if (target_dir && ft_strncmp(target_dir, "--", 3) == 0)
		return (cd_core(get_env_value(env, "HOME"), env));
	if (!target_dir)
	{
		home = get_env_value(env, "HOME");
		if (!home)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			return (1);
		}
		return (cd_core(home, env));
	}
	return (cd_core(target_dir, env));
}
