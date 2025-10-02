/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:37 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/02 13:21:11 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int is_n_flag(const char *arg)
{
    int i;

    if (!arg || arg[0] != '-')
        return 0;

    i = 1;
    if (arg[i] != 'n')
        return 0;

    // consume all consecutive n's
    while (arg[i] == 'n')
        i++;

    // skip any trailing spaces or tabs
    while (arg[i] && isspace((unsigned char)arg[i]))
        i++;

    // if we reached the end of string, it's a valid -n* flag
    return arg[i] == '\0';
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

#include <stdio.h>

void print_args(t_command *cmd)
{
    int i;

    if (!cmd || !cmd->args)
    {
        printf("No arguments\n");
        return;
    }

    for (i = 0; cmd->args[i]; i++)
        printf("args[%d]: [%s]\n", i, cmd->args[i]);
}

int	echo_cmd(t_command *cmd)
{
	//print_args(cmd);
	int	i;
	int	new_line;

	//trim_quotes_for_execution(cmd->args);
	i = 1;
	new_line = 1;
	while (cmd->args[i] && is_n_flag(cmd->args[i]))
	{
		new_line = 0;
		i++;
	}
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
