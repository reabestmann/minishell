/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/30 20:15:51 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/* update_shlvl:
**   Increments the SHLVL environment variable by 1 when a new shell is started.
**   - If SHLVL is unset, defaults to 1.
**   - Caps SHLVL at 999 and resets negative values to 0.
**   - Updates the environment variable in-place using update_var().
*/
void	update_shlvl(t_env **env)
{
	char	*lvl_str;
	int		lvl;
	char	*new_lvl;
	char	*to_set;

	lvl_str = get_env_value(env, "SHLVL");
	lvl = 1;
	if (lvl_str)
		lvl = ft_atoi(lvl_str);
	if (lvl < 0)
		lvl = 0;
	else if (lvl >= 1000)
		lvl = 1;
	new_lvl = ft_itoa(lvl);
	if (!new_lvl)
		return ;
	to_set = ft_strjoin("SHLVL=", new_lvl);
	free(new_lvl);
	if (!to_set)
		return ;
	update_var(to_set, env);
	free(to_set);
}

void	update_last_command(t_env **env, char *last_cmd)
{
	char	*to_set;

	if (!last_cmd)
		return ;
	to_set = ft_strjoin("_=", last_cmd);
	if (!to_set)
		return ;
	update_var(to_set, env);
	free(to_set);
}
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

static int	handle_heredoc_fail(t_command *cmds, t_token *tokens)
{
	free_commands(cmds);
	free_tokens(tokens);
	return (1);
}

static int	handle_syntax_error(
	t_command *cmds, t_token *tokens, int valid)
{
	free_heredocs(cmds);
	free_tokens(tokens);
	free_commands(cmds);
	return (valid);
}

static void	cleanup_after_exec(t_command *cmds, t_token *tokens)
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
	if (collect_heredocs(cmds, status) < 0)
		return (handle_heredoc_fail(cmds, tokens));
	if (valid > 0)
		return (handle_syntax_error(cmds, tokens, valid));
	status = run_command(cmds, env, status);
	cleanup_after_exec(cmds, tokens);
	return (status);
}

/*int	handle_input(char *input, t_env **env, int status)
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
	if (collect_heredocs(cmds, status) < 0)
	{
		free_commands(cmds);
		free_tokens(tokens);
		return (1);
	}
	if (valid > 0)
	{
		free_heredocs(cmds);
		free_tokens(tokens);
		free_commands(cmds);
		return (valid);
	}
	status = run_command(cmds, env, status);
	free_heredocs(cmds);
	free_commands(cmds);
	free_tokens(tokens);
	return (status);
}*/

/*initiates everything */
void	init_main_vars(int *params, char **argv, t_env **env, char **envp)
{
	(void)params;
	(void)argv;
	*env = envp_to_struct(envp);
	if (!env)
		error("environment: struct not built.");
	update_shlvl(env);
	update_last_command(env, "./minishell");
	init_signals();
	disable_ctrl_echo();
}

/*determines wether in noninteractive or interactive mode and reads the lines*/
void	read_line(char **input)
{
	char	*line;

	if (isatty(STDIN_FILENO))
		*input = readline("mini$	");
	else
	{
		line = get_next_line(STDIN_FILENO);
		if (line)
		{
			*input = ft_strtrim(line, "\n");
			free(line);
		}
		else
			*input = NULL;
	}
}

/*sends to handle input and deals with status*/
void	look_at_input(char **input, int *status, t_env **env)
{
	if (*input && isatty(STDIN_FILENO))
		add_history(*input);
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
	if (isatty(STDIN_FILENO))
		enable_ctrl_echo();
	if (env)
		free_env_struct(env);
	return (status);
}
