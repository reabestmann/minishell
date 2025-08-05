/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:23:19 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/05 15:36:51 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * handle_input
 * Parses a full command line input.
 * @input: The raw user input string.
 * @envp: The environment variables for execution.
 *
 * Splits the input by pipes, then splits each segment into arguments,
 * and passes them to the execution function. Also handles memory cleanup.
 */
void	handle_input(char *input, char **envp)
{
	char	**commands;
	char	**args;
	int	i;

	commands = split_pipe(input);
	if (!commands)
		return ;
	i = 0;
	while (commands[i])
	{
		args = split_input(commands[i]);
		if (args)
		{
			run_command(args, envp);
			free_array(args);
		}
		i++;
	}
	free_array(commands);
}
