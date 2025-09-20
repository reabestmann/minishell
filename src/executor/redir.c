/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/17 14:43:03 by aabelkis         ###   ########.fr       */
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
*/

/* fd_check:
   Safely redirects one file descriptor to another.
   - Checks if 'fd' is valid (<0 = error).
   - Uses dup2(fd, std_fd) to make std_fd point to the same source as fd.
   - Closes fd after duplication to avoid leaks.
   - Handles stdin, stdout, heredocs, files, or pipes.
   - Example: fd_check(cmd->heredoc, STDIN_FILENO, "heredoc")
*/
void	fd_check(int fd, int std_fd, char *file)
{
	if (fd < 0)
		error(file);
	if (dup2(fd, std_fd) < 0)
	{
		close(fd);
		error("dup2");
	}
	close(fd);
}

/* heredoc_fd:
   Implements the << heredoc behavior.
   - Reads lines from the user until the specified delimiter is typed.
   - Writes all lines to a temporary file.
   - Returns an fd that can be used as stdin for a command.
   - Temporary file is deleted immediately after opening for reading.
   - Example usage: cmd->heredoc = heredoc_fd("EOF");
*/
static int	heredoc_fd(const char *delimiter)
{
	int		fd;
	int		rfd;
	char	*line;

	fd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		error("heredoc");
	while (1)
	{
		line = readline("> ");
		if (str_equals(line, delimiter))
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, fd);
		ft_putstr_fd("\n", fd);
		free(line);
	}
	close(fd);
	rfd = open(".heredoc_tmp", O_RDONLY);
	unlink(".heredoc_tmp");
	return (rfd);
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
	int	fd;

	if (cmd->heredoc != -1)
		fd_check(cmd->heredoc, STDIN_FILENO, "heredoc");
	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		fd_check(fd, STDIN_FILENO, cmd->infile);
	}
	if (cmd->outfile)
	{
		if (cmd->append == 1) // >>
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (cmd->append == 2) // >
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		fd_check(fd, STDOUT_FILENO, cmd->outfile);
	}
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
