/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 15:23:19 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/02 17:39:54 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* COMMAND STRUCT
	typedef struct s_command
	{
		char **args;           // Command and its arguments
		char *infile;      // Redirection input file
		char *outfile;     // Redirection output file
		int append;     //append (1 = >>), overwrite (2 = >) or heredoc (3 = <<)
		int	modifies_shell;  //if cmd modifies envp (1 = yes, 0 = no)
		int	in_child;		// if the command currently runs in a child process
		struct s_command *next;  // Next command in pipeline
	} t_command;
*/

/* create_cmd:
	Allocate and initializes a new commands structure.
		Sets args/infile/outfile to NULL, append flag to 0,
		and returns a pointer to the new empty t_command.
*/
static t_command	*create_cmd(void)
{
	t_command	*cmd;

	cmd = handle_malloc(sizeof(t_command));
	cmd->args = NULL;
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->append = 0;
	cmd->in_child = 0;
	cmd->next = NULL;
	cmd->heredoc = -1;
	return (cmd);
}

/* append_cmd:
	Adds a command node to the end of a command list.
	If the list is empty, the new command becomes the head.
	Otherwise, it walks to the last node and attaches the 
	new one
*/
static void	append_cmd(t_command **head, t_command *new_cmd)
{
	t_command	*tmp;

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

/* add_arg:
	We count existing args, allocate a new array one bigger,
	copy over the old pointers, add the new arg with ft_strdup, 
	free the old array container (not the srings inside)
*/
static void	add_arg(t_command *cmd, const char *arg)
{
	char	**new_args;
	int		count;
	int		i;

	count = 0;
	if (cmd->args)
	{
		while (cmd->args[count])
			count++;
	}
	new_args = handle_malloc(sizeof(char *) * (count + 2));
	i = -1;
	while (++i < count)
		new_args[i] = cmd->args[i];
	new_args[i] = ft_strdup((char *)arg);
	new_args[i + 1] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
}

/* handle_token
   Processes a single token and updates the current command / command list.
   Returns 1 if a new command was created (pipe), 0 otherwise.
*/
static int	handle_token(t_token **cpy, t_command **cmds, t_command **current)
{
	if ((*cpy)->type == TOKEN_WORD)
		add_arg(*current, (*cpy)->val);
	else if ((*cpy)->type == TOKEN_PIPE)
	{
		set_cmd_flags(*current);
		append_cmd(cmds, *current);
		*current = create_cmd();
		if (!*current)
			return (-1);
		return (1);
	}
	else
		parse_redirection(*current, cpy);
	return (0);
}

/* parser:
	takes a list of tokens from the lexer & groups them into commands.
	splits commands when it finds a TOKEN_PIPE, returns a linked list of commands
	ready for execution.
*/
t_command	*parser(t_token *tokens)
{
	t_command	*cmds;
	t_command	*current;
	t_token		*cpy;

	cmds = NULL;
	current = create_cmd();
	if (!current)
		return (NULL);
	cpy = tokens;
	while (cpy)
	{
		if (handle_token(&cpy, &cmds, &current) == -1)
			return (NULL);
		cpy = cpy->next;
	}
	set_cmd_flags(current);
	append_cmd(&cmds, current);
	return (cmds);
}
