/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 21:37:03 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 21:37:47 by aabelkis         ###   ########.fr       */
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
		{
			close(fd);
			error("dup2");
		}
		close(fd);
		return ;
	}
	if (dup2(fd, std_fd) < 0)
	{
		close(fd);
		error("dup2");
	}
	close(fd);
}

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
