/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:23:19 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/12 16:34:47 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* TO DO
command_t *parser(t_token *tokens);                // turn tokens -> linked command_t pipeline
int executor(command_t *cmds, char **envp);        // run the pipeline (pipes/redirs/builtins)

typedef struct s_command
{
	char **args;           // Command and its arguments
	char *infile;      // Redirection input file
	char *outfile;     // Redirection output file
	int append;     // Whether to append (1 = >>) or overwrite (0 = >)
	struct command *next;  // Next command in pipeline
} t_command;
*/

t_command	*create_cmd(void)
{
	t_command	*cmd;

	cmd = handle_malloc(sizeof(t_command));
	cmd->args = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->next = NULL;

	return (cmd);
}

static void	append_cmd(t_command **head, t_command *new_cmd)
{
	t_command *tmp;

	if (!*head)
		*head = new_cmd;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_cmd;
	}
}

void	add_arg(t_command *cmd, const char *arg)
{
	char	**new_args;
	int	count;
	int	i;

	while (cmd->args && cmd->args[count])
		count++;
	new_args = handle_malloc(sizeof(char *) * (count + 2));
	i = 0;
}

t_command	*parser(t_token *tokens)
{
	t_command	*cmds;
	t_command	*current;
	t_token	*cpy;

	cmds = NULL;
	current = create_cmd();
	if (!current)
		return (NULL);
	cpy = tokens;
	while (t)
	{
		if (t->type == TOKEN_WORD)
			add_arg(current, t->val);
		else if (t->type == TOKEN_PIPE)
		{
			append_cmd(&cmds, current);
			current = create_cmd();
			if (!current)
				return (NULL);
			handle_pipe //TO DO
		}
		else
			handle_redirection //TO DO
		t = t->next;
	}
	append_cmd(&cmds, current);
	return (cmds)
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
void	handle_input(char *input, char **envp)
{
	t_token	*tokens;
	t_command	*cmds;

	tokens = lexer(input);
	if (!tokens)
		return;
	cmds = parser(tokens);
	if (cmds)
	{
		run_commads(cmds, envp);
		free_commands(cmds);
	}
	free_tokens(tokens);
}
