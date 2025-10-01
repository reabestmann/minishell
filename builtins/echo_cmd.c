/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:37 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/01 16:34:52 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int is_n_flag(const char *arg)
{
    int i;

    if (!arg || arg[0] != '-')
        return (0);
    i = 1;
    if (arg[i] != 'n')
        return (0);
    while (arg[i] == 'n')
        i++;
    return (arg[i] == '\0');  // true only if it was just -n or -nnn...
}
/* BUILTIN_LIST
 	/echo with option -n 
	/cd with only a relative or absolute path
	/pwd with no optios
	export with no options
	/unset with no options
	/env with no options
	exit with no options
*/
int	echo_cmd(t_command *cmd)
{
	int	i;
	int	new_line;

	i = 1;
	new_line = 1;
	while (cmd->args[i] && is_n_flag(cmd->args[i]))
	{
		new_line = 0;
		i++;
	}
	trim_quotes_for_execution(cmd->args);
	while (cmd->args[i] != NULL)
	{
		printf("%s", cmd->args[i]);
		if (cmd->args[i + 1] != NULL)
			printf(" ");
		i++;
	}
	if (new_line == 1)
		printf("\n");
	return (0);
}
