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

/* tee_pipe:
 *  - Reads from stdin, writes to pipe_fd and optionally to a file.
 *  - append: 1 = append to file, 2 = overwrite, 0 = no file.
 *  - Allocates fd for file, writes line by line from stdin using
 *  get_next_line.*/
void	tee_pipe(int pipe_fd, const char *outfile, int append)
{
	int		fd;
	char	*line;

	fd = -1;
	if (append == 1)
		fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else if (append == 2)
		fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		error(outfile);
	line = get_next_line(STDIN_FILENO);
	while (line)
	{
		write(pipe_fd, line, ft_strlen(line));
		if (fd != -1)
			write(fd, line, ft_strlen(line));
		line = get_next_line(STDIN_FILENO);
	}
}

/* dol_q_expansion:
   Handles the special "$?" expansion inside heredocs.
   - If current char is '?', replace with last_status converted to string.
   - Appends it to result using append_normal_text().
   - Advances index past '?'.
   - Returns:
       1 → expansion handled
      -1 → malloc failure
       0 → not a "$?" expansion
*/
int	dol_q_expansion(char *line, int *i, int last_status, char **result)
{
	char	*num;
	char	*temp;

	if (line[*i] == '?')
	{
		num = ft_itoa(last_status);
		if (!num)
			return (-1);
		temp = append_normal_text(num, *result);
		if (!temp)
			return (free(num), -1);
		free(*result);
		*result = temp;
		free(num);
		(*i)++;
		return (1);
	}
	return (0);
}

/*handle_redir_file:
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
