/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 14:12:31 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 14:17:46 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* check_expand_line:
   Prepares a line before writing it to heredoc file.
   - If expand == 1, performs variable expansion.
   - If expand == 0, returns line unchanged.
   - Always frees original line if expansion is done.
*/
static char	*check_expand_line(char *line, int expand, int status)
{
	char	*result;

	if (!line)
		return (NULL);
	if (expand)
	{
		result = expand_for_heredoc(line, status);
		free(line);
		return (result);
	}
	result = ft_strdup(line);
	free(line);
	return (result);
}

/* print_heredoc_prompt:
 *   Prints the heredoc prompt "> " to the terminal.*/
void	print_heredoc_prompt(void)
{
	int	tty_fd;

	tty_fd = open("/dev/tty", O_WRONLY);
	if (tty_fd >= 0)
	{
		ft_putstr_fd("> ", tty_fd);
		close(tty_fd);
	}
}

/* is_delimiter_line:
 *   Returns 1 if the input line matches the heredoc delimiter 
 (ignores newline), 0 otherwise.*/
int	is_delimiter_line(char *line, char *trimmed_delim)
{
	char	*trimmed_line;
	int		match;

	trimmed_line = ft_strtrim(line, "\n");
	match = str_equals(trimmed_line, trimmed_delim);
	free(trimmed_line);
	return (match);
}

/* process_and_write_line:
 *   Expands line if needed and writes it to the heredoc pipe.*/
void	process_and_write_line(char *line, int expand, int status, int write_fd)
{
	line = check_expand_line(line, expand, status);
	write(write_fd, line, ft_strlen(line));
	free(line);
}

//returns 1 if sigint was received, 0 otherwise
int	handle_sigint_in_heredoc(char *line, char *trimmed_delim)
{
	if (g_sigint_received)
	{
		free(line);
		free(trimmed_delim);
		return (1);
	}
	return (0);
}
