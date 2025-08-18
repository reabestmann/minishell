/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/05 15:37:46 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* to compile, use:
cc -Wall -Wextra -Werror main.c parser/split.c parser/pipe_split.c parser/parse.c executor/exec.c utils/free.c -o minishell -lreadline -Llibft -lft
*/

int	main(int params, char **argv, char **envp)
{
	char	*input;

	(void)params;
	(void)argv;

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
		//handle_input(input, envp); //add back later
		t_command cmd = parse_input(input); //temp
    	run_command(&cmd, envp); // your existing executor //temp
		// Free t_command//temp
		t_command *current = &cmd;//temp
		while (current)//temp
		{//temp
			for (int i = 0; current->args[i]; i++)//temp
				free(current->args[i]);//temp
			free(current->args);//temp
			free(current->input_file);//temp
			free(current->output_file);//temp
			t_command *tmp = current->next;//temp
			if (current != &cmd)//temp
				free(current);//temp
			current = tmp;//temp
		}//temp
		free(input);
	}
	/*todo: final clean up*/
	enable_ctrl_echo();
	return (0);
}
