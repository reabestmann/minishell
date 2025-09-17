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

/* handle_input:
   Parses a full command line input.
   Puts input into tokens and commands.
   @input: The raw user input string.
   @envp: the environment variables for execution.
   *tokens - pointer to the first token of a linked list of tokens.
   *cmds - pointer to the first command of a linked list of commands.
   gets called by: main
   calls: lexer, parser, executor, free_commands, free_tokens
*/
int	handle_input(char *input, t_env **env, int status)
{
	t_token		*tokens;
	t_command	*cmds;

	tokens = lexer(input);
	if (!tokens)
		return (0);
	cmds = parser(tokens);
	if (cmds)
	{
		status = run_command(cmds, env, status);
		free_commands(cmds);
	}
	free_tokens(tokens);
	return (status);
}

int	main(int params, char **argv, char **envp)
{
	char	*input;
	t_env	*env;
	int		status;

	(void)params;
	(void)argv;

	env = envp_to_struct(envp);
	if (!env)
    	error("environment: struct not built.");
	init_signals();
	disable_ctrl_echo();
	status = 0;

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
		handle_input(input, &env, status);
		free(input);
	}
	/*todo: final clean up*/
	enable_ctrl_echo();
	return (status);
}
