/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/26 21:36:50 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*    Execution flow:
     1. parse_redirection() sets infile, outfile, append, heredoc
     2. heredoc_fd() reads user input → returns fd
     3. apply_redirections() dup2s FDs based on priority:
        stdin  ← heredoc / infile / pipe read
        stdout → outfile / pipe write
     4. fd_check() safely duplicates and closes original fd.

	 Notes:
     - Heredoc overrides infile if both exist
     - FDs are closed after dup2 to prevent leaks
     - Works with standalone commands, pipelines, heredocs, and redirections
*/

// helper functions of apply, redirections
static void	handle_infile(char **filename)
{
	char	*clean;
	int		fd;

	clean = remove_quotes(*filename);
	free(*filename);
	*filename = clean;
	fd = open(*filename, O_RDONLY);
	fd_check(fd, STDIN_FILENO, *filename);
}

static void	handle_outfile(char **filename, int append)
{
	char	*clean;
	int		fd;

	clean = remove_quotes(*filename);
	free(*filename);
	*filename = clean;
	fd = 0;
	if (append == 1)
		fd = open(*filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
	else if (append == 2)
		fd = open(*filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	fd_check(fd, STDOUT_FILENO, *filename);
}

/* apply_redirections:
   Sets up all input/output redirections for a command before execution.
   Input priority: heredoc > infile > pipe from previous command
   Output priority: outfile > pipe to next command
   - If heredoc exists (cmd->heredoc != -1), dup2 it to STDIN
   - If infile exists, open it and dup2 to STDIN
   - If outfile exists:
       append == 1 → >> append mode
       append == 2 → > truncate/overwrite mode
       dup2 to STDOUT
*/
void	apply_redirections(t_command *cmd)
{
	if (cmd->heredoc != -1)
		fd_check(cmd->heredoc, STDIN_FILENO, "heredoc");
	else if (cmd->infile)
		handle_infile(&cmd->infile);
	if (cmd->outfile)
		handle_outfile(&cmd->outfile, cmd->append);
}

/* set_redirection:
   Helper for parse_redirection.
   Sets cmd->outfile and cmd->append type based on the token.
*/
static void	set_redirection(t_command *cmd, t_token *token, int append_type)
{
	if (token && token->type == TOKEN_WORD)
	{
		cmd->outfile = ft_strdup(token->val);
		cmd->append = append_type;
	}
}

/* parse_redirection:
   Parses a token and updates the command struct with redirection info.
   - < : input file → cmd->infile
   - > : output truncate → cmd->outfile, append=2
   - >>: output append → cmd->outfile, append=1
   - <<: heredoc → cmd->heredoc = heredoc_fd(delimiter)
   - Advances token pointer to the filename/delimiter
*/
void	parse_redirection(t_command *cmd, t_token **cpy)
{
	t_token	*next;

	if (!*cpy)
		return ;
	next = (*cpy)->next;
	if ((*cpy)->type == TOKEN_REDIR_IN)
	{
		if (next && next->type == TOKEN_WORD)
			cmd->infile = ft_strdup(next->val);
	}
	else if ((*cpy)->type == TOKEN_REDIR_OUT)
		set_redirection(cmd, next, 2);
	else if ((*cpy)->type == TOKEN_REDIR_APPEND)
		set_redirection(cmd, next, 1);
	else if ((*cpy)->type == TOKEN_HEREDOC)
	{
		if (next && next->type == TOKEN_WORD)
			cmd->heredoc = heredoc_fd(next->val);
	}
	if (next)
		*cpy = next;
}
