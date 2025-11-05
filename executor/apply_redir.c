/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 15:02:35 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* fd_check:
   Safely redirects one file descriptor to another.
	(dup2 with error handling)
*/
void	fd_check(int fd, int std_fd, char *file)
{
	if (fd < 0)
		error(file);
	if (std_fd == 3)
	{
		if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0)
			error("dup2");
		return ;
	}
	if (dup2(fd, std_fd) < 0)
	{
		close(fd);
		error("dup2");
	}
	close(fd);
}

/* handle_redir_file:
Opens a file for output redirection according to append_mode.
 - fd_type: target file descriptor (STDOUT, STDERR, or both)
Uses fd_check() to safely duplicate and close the FD.*/
void	handle_redir_file(char *file, int append_mode, int fd_type)
{
	int	fd;

	fd = -1;
	if (append_mode == 1)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0664);
	else if (append_mode == 2)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	fd_check(fd, fd_type, file);
}

/* handle_infile:
 Prepares an input file for redirection.
  - Removes quotes from filename
  - Opens file read-only
 - Uses fd_check() to set STDIN and handle errors*/
void	handle_infile(char **filename)
{
	char	*clean;
	int		fd;

	clean = remove_quotes(*filename);
	free(*filename);
	*filename = clean;
	fd = open(*filename, O_RDONLY);
	fd_check(fd, STDIN_FILENO, *filename);
}

// call this from apply_redirections() before other redirections
void	handle_heredocs(t_command *cmd)
{
	int	merged_pipe[2];
	int	i;

	if (cmd->heredoc_count == 1)
	{
		fd_check(cmd->heredoc_fds[0], STDIN_FILENO, "heredoc");
		return ;
	}
	if (pipe(merged_pipe) == -1)
		error("pipe merged heredoc");
	i = -1;
	while (++i < cmd->heredoc_count)
		merge_fd_into_pipe(cmd->heredoc_fds[i], merged_pipe[1]);
	close(merged_pipe[1]);
	fd_check(merged_pipe[0], STDIN_FILENO, "heredoc");
}

/* apply_redirections:
Sets up all input/output redirections for a command before execution.
Input priority: heredoc > infile > pipe from previous command
Output priority: outfile > pipe to next command
- If heredoc exists (cmd->heredoc != -1), dup2 it to STDIN
- If infile exists, open it and dup2 to STDIN
- If outfile/errfile exists:
	append == 1 → >> append mode
	append == 2 → > truncate/overwrite mode
	dup2 to STDOUT
*/
void	apply_redirections(t_command *cmd, t_env **env, int last_status)
{
	char	*expanded;

	if (cmd->heredoc_count)
		handle_heredocs(cmd);
	if (cmd->infile)
		handle_infile(&cmd->infile);
	if (cmd->outfile)
	{
		expanded = expand_arg_keep_quotes_simple(cmd->outfile,
				*env, last_status);
		free(cmd->outfile);
		cmd->outfile = remove_quotes(expanded);
		free(expanded);
		handle_redir_file(cmd->outfile, cmd->append, cmd->fd_type);
	}
	if (cmd->errfile)
	{
		expanded = expand_arg_keep_quotes_simple(cmd->errfile,
				*env, last_status);
		free(cmd->errfile);
		cmd->errfile = remove_quotes(expanded);
		free(expanded);
		handle_redir_file(cmd->errfile, cmd->append_err, cmd->fd_type);
	}
}
