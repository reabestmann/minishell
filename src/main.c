/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/03 15:40:21 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* to compile, use:
cc -Wall -Wextra -Werror main.c parser/split.c executor/exec.c utils/free.c -o minishell -lreadline -Llibft -lft
*/

int	main(int params, char **argv, char **envp)
{
	char	*input;
	char	**args;

	(void)params;
	(void)argv;

	while (1)
	{
		input = readline("mini	");
		if (!input)
			break ;
		if (*input)
			add_history(input);
		if (!ft_strncmp(input, "exit", 5) && input[4] == 0)
		{
			free(input);
			break ;
		}
		args = split_input(input);
                if (args)
                {       
			run_command(args, envp);
			free_array(args);
                }
		free(input);
	}
	return (0);
}
