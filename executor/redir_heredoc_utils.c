/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 21:37:03 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 14:56:42 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
 - append_mode: 1 = append (>>), 2 = overwrite (>)
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

/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted.
   - Returns 1 if delimiter starts/ends with matching quotes.
   - Used to decide whether expansion is disabled in heredoc.
*/
int	delimiter_was_quoted(const char *delimiter)
{
	int	s_quote;
	int	d_quote;

	s_quote = 0;
	d_quote = 0;
	while (*delimiter)
	{
		if (*delimiter == '\'')
			s_quote++;
		if (*delimiter == '"')
			d_quote++;
		delimiter++;
	}
	if ((s_quote != 0 && (s_quote % 2 == 0))
		|| (d_quote != 0 && (d_quote % 2 == 0)))
		return (1);
	return (0);
}

/* add_heredoc:
	 Adds a new heredoc delimiter to the command struct */
void	add_heredoc(t_command *cmd, const char *delimiter)
{
	char	**arr;
	int		i;
	int		j;

	i = 0;
	while (cmd->heredoc_delim && cmd->heredoc_delim[i])
		i++;
	arr = handle_malloc(sizeof(char *) * (i + 2));
	j = 0;
	while (j < i)
	{
		arr[j] = ft_strdup(cmd->heredoc_delim[j]);
		j++;
	}
	arr[i] = ft_strdup(delimiter);
	arr[i + 1] = NULL;
	if (cmd->heredoc_delim)
		free_array(cmd->heredoc_delim);
	cmd->heredoc_delim = arr;
	cmd->heredoc_count = i + 1;
}
