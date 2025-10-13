/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_cmd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:25 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/13 17:12:59 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* is_quoted
   Checks if a string starts with a single or double quote.
   @arg: the input string
   Returns: 1 if quoted, 0 otherwise
*/
int	is_quoted(char *arg)
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
/*static int	go_home(char **target_dir, char **temp, t_env **env)
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
}*/

/* display_oldpwd
   Handles "cd -" (switch to previous directory).
   @env: environment list
   @temp: pointer to store duplicated OLDPWD value
   Behavior:
     - If OLDPWD is not set → print error and return 1
     - Otherwise → copy OLDPWD into temp and print it
   Returns: 0 on success, 1 on failure
*/
/*
static int	display_oldpwd(t_env **env)
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
	ft_putendl_fd(paths->value, 1);
	return (0);
}

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
*/
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

/*
static int	cd_core(char **target_dir, char **temp, t_env **env)
{
	char	*oldpwd;
	char	*newpwd;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (1);
	if (chdir(*target_dir) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		perror(*target_dir);
		free(oldpwd);
		return(1);
	}
	update_oldpwd(env, oldpwd);
	newpwd = getcwd(NULL, 0);
	update_newpwd(env, newpwd);
	free(oldpwd);
	free(newpwd);
	if (*temp)
		free(*temp);
	return (0);
}*/

/*checks if extra args following cmd*/
int too_many_args(t_command *cmd)
{
	if (cmd->args[2] != NULL) // extra arguments exist
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		return (1);
	}
	return (0);
}
/*
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
	if (too_many_args(cmd) == 1)
		return (1);
	if (ft_strncmp(target_dir, "-", 1) == 0 && target_dir[1] == '\0')
	{
		if (display_oldpwd(env, &temp) == 1)
			return (1);
		return (0);
	}
	if (temp)
		target_dir = temp;
	if (!target_dir)
		return (1);
	if (cd_core(&target_dir, &temp, env))
		return (1);
	return (0)
	;
}*/

int cd_OLDPWD_check(char *target_dir, t_env **env)
{
    char *oldpwd_val;
    char *pwd_before;
    char *pwd_after;

    if (ft_strncmp(target_dir, "-", 1) == 0 && target_dir[1] == '\0')
    {
        oldpwd_val = get_env_value(env, "OLDPWD");
        if (!oldpwd_val)
        {
            ft_putendl_fd("cd: OLDPWD not set", 2);
            return 1;
        }

        pwd_before = getcwd(NULL, 0);    // current PWD
        if (!pwd_before)
            return 1;

        if (chdir(oldpwd_val) == -1)
        {
            perror(oldpwd_val);
            free(pwd_before);
            return 1;
        }

        // swap PWD and OLDPWD
        update_oldpwd(env, pwd_before);

        pwd_after = getcwd(NULL, 0);     // new PWD after chdir
        update_newpwd(env, pwd_after);

        ft_putendl_fd(pwd_after, 1);     // print new PWD

        free(pwd_before);
        free(pwd_after);

        return 0;
    }
    return 0;
}



/*woring one*/
/*int	cd_cmd(t_command *cmd, t_env **env)
{
	char	*target_dir;

	target_dir = cmd->args[1]; // already preprocessed (~ expanded, quotes trimmed)
	if (too_many_args(cmd))
		return (1);

	// handle cd - 
	if (target_dir && ft_strncmp(target_dir, "-", 2) == 0)
		return cd_OLDPWD_check(target_dir, env); // instead of display_old...

	// if no target_dir, default to HOME (should already be handled in preprocess)
	if (!target_dir)
	{
		ft_putendl_fd("minishell: cd: HOME not set", 2);
		return (1);
	}

	return cd_core(target_dir, env);
}
*/
#include "../minishell.h"

int cd_cmd(t_command *cmd, t_env **env)
{
    char *target_dir;

    target_dir = cmd->args[1]; // already preprocessed (~ expanded, quotes trimmed)

    // check for too many arguments
    if (too_many_args(cmd))
        return 1;

    // handle `cd -` (previous directory)
    if (target_dir && strncmp(target_dir, "-", 2) == 0)
        return cd_OLDPWD_check(target_dir, env);

    // handle `cd --` (explicit home)
    if (target_dir && strncmp(target_dir, "--", 3) == 0)
        return cd_core(get_env_value(env, "HOME"), env);

    // if no target_dir, default to HOME
    if (!target_dir)
    {
        char *home = get_env_value(env, "HOME");
        if (!home)
        {
            ft_putendl_fd("minishell: cd: HOME not set", 2);
            return 1;
        }
        return cd_core(home, env);
    }

    // default: cd to target_dir
    return cd_core(target_dir, env);
}
