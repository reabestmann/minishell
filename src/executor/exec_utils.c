/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 20:34:04 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/26 21:25:44 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* run_builtin: 
	checks if 'cmd' matches any builtin name.
	Calls str_equals for each of the 7 builtin commands.
	Calls the function directly and returns its result.
	-> 0 = success, >0 = Error.
	returns -1 if command is not a builtin.
*/
int	run_builtin(t_command *cmd, t_env **env)
{
	if (str_equals(cmd->args[0], "cd"))
		return (cd_cmd(cmd, env));
	trim_quotes_for_execution(cmd->args);
	if (str_equals(cmd->args[0], "echo"))
		return (echo_cmd(cmd));
	if (str_equals(cmd->args[0], "pwd"))
		return (pwd_cmd(env));
	if (str_equals(cmd->args[0], "export"))
		return (export_cmd(cmd, env));
	if (str_equals(cmd->args[0], "unset"))
		return (unset_cmd(cmd, env));
	if (str_equals(cmd->args[0], "env"))
		return (env_cmd(env));
	if (str_equals(cmd->args[0], "exit"))
		return (exit_cmd(cmd, env));
	return (-1);
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
