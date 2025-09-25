/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/23 20:07:33 by aabelkis         ###   ########.fr       */
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
	int			valid;

	tokens = lexer(input);
	if (!tokens)
		return (0);
	valid = syntax_valid(tokens);
	if (valid > 0)
	{
		free_tokens(tokens);
		return (valid);
	}
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
			break ;
		}
		if (*input)
			add_history(input);
		status = handle_input(input, &env, status);
		if (status == 2)
			input = readline("quote> ");
		else if (status == 3)
			input = readline("dquote> ");
		free(input);
	}
	enable_ctrl_echo();
	if (env)
		free_env_struct(env);
	return (status);
}
