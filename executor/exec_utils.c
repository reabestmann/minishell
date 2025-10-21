/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 20:34:04 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/02 13:55:52 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* Expands unquoted ~ to HOME in a single argument */
static char *expand_tilde_if_needed(t_env **env, char *arg)
{
    if (!arg)
        return NULL;

    if (arg[0] == '~' && !is_quoted(arg))
        return expand_home(env, arg);

    return ft_strdup(arg);
}

/* Expands ~ in cmd->args and handles cd with no argument */
void expand_tilde_in_args(t_command *cmd, t_env **env)
{
    int i;
    char *expanded;

    if (!cmd || !cmd->args)
        return;

    // Handle "cd" with no argument → set args[1] = $HOME
    if (ft_strncmp(cmd->args[0], "cd", 3) == 0 && !cmd->args[1])
    {
        char *home = get_env_value(env, "HOME");
        if (home)
            cmd->args[1] = ft_strdup(home);
    }

    for (i = 0; cmd->args[i]; i++)
    {
        char *arg = cmd->args[i];
        expanded = expand_tilde_if_needed(env, arg);
        if (expanded)
        {
            free(cmd->args[i]);
            cmd->args[i] = expanded;
        }
    }
}

/* run_builtin: 
    Expands '~' in arguments and trims quotes for execution.
    Trims the quotes for execution.
    Applies any redirections (stdout/stderr) for the command.
	checks if 'cmd' matches any builtin name.
	Calls str_equals for each of the 7 builtin commands.
	Calls the function directly and returns its result.
	-> 0 = success, >0 = Error.
	returns -1 if command is not a builtin.
*/
int	run_builtin(t_command *cmd, t_env **env, int status)
{
    expand_tilde_in_args(cmd, env);
	trim_quotes_for_execution(cmd->args);
	if (str_equals(cmd->args[0], "cd"))
		return (cd_cmd(cmd, env));
	if (str_equals(cmd->args[0], "echo"))
		return (echo_cmd(cmd));
	if (str_equals(cmd->args[0], "pwd"))
		return (pwd_cmd(env));
	if (str_equals(cmd->args[0], "export"))
		return (export_cmd(cmd, env));
	if (str_equals(cmd->args[0], "unset"))
		return (unset_cmd(cmd, env));
	if (str_equals(cmd->args[0], "env"))
		return (env_cmd(cmd, env));
	if (str_equals(cmd->args[0], "exit"))
		return (exit_cmd(cmd, env, status));
	return (-1);
}

/* prepare_builtin_exec:
 * - Saves and restores the original stdout/stderr so that
 *   the shell prompt and subsequent commands are unaffected,
 *   which is necessary because builtins run in the parent process.
 * - Applies any redirections (stdout/stderr) for the command.
 * - Returns the builtin's return code.
 */
int  prepare_builtin_exec(t_command *cmds, t_env **env, int status)
{
    int saved_stdout;
    int saved_stderr;
    int ret;

    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);
    apply_redirections(cmds, env, status);
    ret = run_builtin(cmds, env, status);
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stdout);
    close(saved_stderr);
    return (ret);
}

/*trims empty args so that the other args are usable in execute*/
void	trim_empty_args(char **args)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (args[i] && args[i][0] == '\0')
		i++;
	if (i == 0)
		return ;
	while (args[i])
	{
		args[j] = args[i];
		i++;
		j++;
	}
	args[j] = NULL;
}
