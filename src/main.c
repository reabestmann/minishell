/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/02 18:20:00 by rbestman         ###   ########.fr       */
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
		if (!ft_strncmp(input, "exit", 5) && input[4] == 0)
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
