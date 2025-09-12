/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/10 22:56:30 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* to compile, use:
cc -Wall -Wextra -Werror main.c parser/split.c parser/pipe_split.c parser/parse.c executor/exec.c utils/free.c -o minishell -lreadline -Llibft -lft
*/

int	main(int params, char **argv, char **envp)
{
	char	*input;
	t_env	*env;

	(void)params;
	(void)argv;

	env = envp_to_struct(envp);
	if (!env)
    	error("environment: struct not built.");
	init_signals();
	disable_ctrl_echo();

	while (1)
	{
		input = readline("mini$	");
		if (!input)
		{
			printf("exit\n");
		/*todo: clean up here ie deal with any resources*/
			enable_ctrl_echo();
			break ;
		}
		if (*input)
			add_history(input);
		if (str_equals(input, "exit"))
		{
			free(input);
			break ;
		}
		handle_input(input, &env);
		free(input);
	}
	/*todo: final clean up*/
	enable_ctrl_echo();
	return (0);
}
