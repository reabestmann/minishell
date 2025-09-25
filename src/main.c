/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/25 12:25:35 by aabelkis         ###   ########.fr       */
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
	if (!syntax_valid(tokens))
	{
		free_tokens(tokens);
		return (2);
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
		if (isatty(STDIN_FILENO))
			input = readline("mini$	");
		else 
			input = get_next_line(STDIN_FILENO);
		if (!input)
		{
			if (isatty(STDIN_FILENO))
				printf("exit\n");
			break ;
		}
		
		if (*input && isatty(STDIN_FILENO))
			add_history(input);
		status = handle_input(input, &env, status);
		free(input);
	}
	enable_ctrl_echo();
	if (env)
		free_env_struct(env);
	return (status);
}

/*

int main(int argc, char **argv, char **envp)
{
    t_env *env = envp_to_struct(envp);
    int status = 0;
    char buffer[4096];
    size_t filled = 0;
    ssize_t bytes;
    char *line_start = buffer;

    if (!env)
        error("environment: struct not built.");

    init_signals();
    disable_ctrl_echo();

    int interactive = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);

    if (interactive)
    {
        while (1)
        {
            char *input = readline("mini$ ");
            if (!input)
            {
                write(1, "exit\n", 5);
                break;
            }
            if (*input)
                add_history(input);
            status = handle_input(input, &env, status);
            free(input);
        }
    }
    else
    {
        // Non-interactive mode: read from stdin
        while ((bytes = read(STDIN_FILENO, buffer + filled, sizeof(buffer) - filled - 1)) > 0)
        {
            filled += bytes;
            buffer[filled] = '\0';

            char *newline;
            while ((newline = strchr(line_start, '\n')))
            {
                *newline = '\0';
                if (*line_start)
                    status = handle_input(line_start, &env, status);
                line_start = newline + 1;
            }

            // Move leftover bytes to start of buffer
            filled = buffer + filled - line_start;
            if (filled > 0)
                memmove(buffer, line_start, filled);
            line_start = buffer;
        }

        // Handle last line if no trailing newline
        if (filled > 0)
        {
            buffer[filled] = '\0';
            status = handle_input(buffer, &env, status);
        }
    }

    enable_ctrl_echo();
    if (env)
        free_env_struct(env);

    return status;
}*/