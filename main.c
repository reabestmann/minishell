/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/06 15:21:40 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* handle_input + error helpers
	sends input to lexer, parser, syntax check.
	collects heredocs and sends to run_command.
*/
static int	handle_heredoc_fail(t_command *cmds, t_token *tokens)
{
	free_commands(cmds);
	free_tokens(tokens);
	return (130);
}

static void	free_cmd_tokens(t_command *cmds, t_token *tokens)
{
	free_heredocs(cmds);
	free_commands(cmds);
	free_tokens(tokens);
}

int	handle_input(char *input, t_env **env, int status)
{
	t_token		*tokens;
	t_command	*cmds;
	int			valid;

	tokens = lexer(input);
	if (!tokens)
		return (0);
	cmds = parser(tokens);
	if (!cmds)
	{
		free_tokens(tokens);
		return (1);
	}
	valid = syntax_valid(tokens);
	if (collect_heredocs(cmds) < 0)
		return (handle_heredoc_fail(cmds, tokens));
	if (valid > 0)
	{
		free_cmd_tokens(cmds, tokens);
		return (valid);
	}
	pretruncate_files(cmds);
	status = run_command(cmds, env, status);
	free_cmd_tokens(cmds, tokens);
	return (status);
}

/* sends to handle input and deals with history
	and multiline input that readline can't handle */
void	look_at_input(char **input, int *status, t_env **env)
{
	char	*cleaned;

	if (*input && isatty(STDIN_FILENO))
	{
		add_history(*input);
		cleaned = remove_control_chars(*input);
		free(*input);
		*input = cleaned;
	}
	*status = handle_input(*input, env, *status);
	free(*input);
}

int	main(int params, char **argv, char **envp)
{
	char	*input;
	t_env	*env;
	int		status;

	init_main_vars(&params, argv, &env, envp);
	status = 0;
	while (1)
	{
		read_line(&input);
		if (!input)
			break ;
		look_at_input(&input, &status, &env);
	}
	if (env)
		free_env_struct(env);
	return (status);
}
