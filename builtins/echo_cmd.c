/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:37 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/30 19:35:54 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
	int j;
	int	new_line;

	i = 1;
	new_line = 1;
	j = 1;
	while (str_equals(cmd->args[i], "-n") || cmd->args[i][j++] == 'n')
	{
		j = 1;
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
