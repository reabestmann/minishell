/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/11 12:51:56 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* REMINDERS

   REDIRECTIONS:
    < = input redirection
    > = output overwrite
    >> = output append
    << = heredoc

	
   FILE PERMISSION '0644':
	0 6 4 4
	│ │ │ │
	│ │ │ └─ others: read only (4)
	│ │ └── group: read only (4)
	│ └── owner: read + write (6)
	└── special flag (0)

   FILE DESCRIPTORS:
	0 = stdin
	1 = stdout
	2 = stderr
*/

/* fd_check:
	Checks if 'fd' is valid.
	calls dup2(old_fd, new_fd): 
	 redirects stdin/stdout to files, pipes or heredocs.
	 - makes new_fd point to the same file/pipe as old_fd.
	 - if new_fd was open, it closes it.
	Closes the orinigal fd and handles errors.
*/
static void	fd_check(int fd, int std_fd, char *file)
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
	Reads user input until the user specified delimiter
	- eg.: cat << EOF
	  Hello
	  World
	  EOF
	  -> Everything typed goes to cat until EOF is typed on a line by itself.
	Writes those input lines to a pipe, then returns the read end of the pipe.
	The calling command can redirect stdin from this file descriptor.
*/
static int	heredoc_fd(const char *delimiter)
{
	int		fd;
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
			break;
		}
		ft_putstr_fd(line, fd);
		ft_putstr_fd("\n", fd);
		free(line);
	}
	close(fd);
	return (open(".heredoc_tmp", O_RDONLY));
}

/* apply_redirections:
	Applies input/output redirections for a command.
	Redirects stdin from infile, stdout to outfile. 
	- if append = 1, it appends the outfile (>>).
	- if append = 2, it overwrites/truncates the outfile (>).
	- if append = 3, temporary heredoc file (<<), redirected from STDIN.
	Calls fd_check for safe file descriptor handling.
*/
void	apply_redirections(t_command *cmd)
{
	int	fd;

	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		fd_check(fd, STDIN_FILENO, cmd->infile);
	}
	if (cmd->outfile)
	{
		if (cmd->append == 1 || cmd->append == 2)
		{
			if (cmd->append == 1)	// >>
				fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644); 
			else if (cmd->append == 2) // >
				fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			fd_check(fd, STDOUT_FILENO, cmd->outfile);
		}
		else if (cmd->append == 3) // <<
		{
			fd = heredoc_fd(cmd->outfile);
			fd_check(fd, STDIN_FILENO, cmd->outfile);
			unlink(".heredoc_tmp");
		}
	}
}

/* set_redirection:
    fills the command struct with the values of its outfile and append type
    based on the values contained in the token.
*/
static void set_redirection(t_command *cmd, t_token *token, int append_type)
{
    if (token && token->type == TOKEN_WORD)
    {
        cmd->outfile = ft_strdup(token->val);
        cmd->append = append_type;
    }
}

/* parse_redirection:
    Checks the current token for a redirection operator.
    Sets the command's infile or outfile and append type accordingly.
    Advances the token pointer to the filename/delimiter token.
*/
void	parse_redirection(t_command *cmd, t_token **cpy)
{
    t_token *next;

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
		set_redirection(cmd, next, 3);

	if (next)
		*cpy = next;
}