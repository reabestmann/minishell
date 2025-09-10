/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:37 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 21:01:59 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

 /*echo with option -n
/cd with only a relative or absolute path
/pwd with no options
export with no options
unset with no options
env with no options
exit with no options*/



int echo_cmd(t_command *cmd)
{
	int i;
	int new_line;

	i = 0;
	new_line = 1;
	if (cmd->args[i] != NULL && ft_strncmp(cmd->args[i], "echo", 4) == 0 && cmd->args[i][4] == '\0')
        i++;
    else
		return(1);
	while (cmd->args[i] != NULL && ft_strncmp(cmd->args[i], "-n", 2) == 0 && cmd->args[i][2] == '\0')
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
	return(0);
}