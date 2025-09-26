/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_heredoc1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 21:29:48 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 21:42:17 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* process_line:
   Prepares a line before writing it to heredoc file.
   - If expand == 1, performs variable expansion.
   - If expand == 0, returns line unchanged.
   - Always frees original line if expansion is done.
*/
static char	*process_line(char *line, int expand)
{
	char	*result;

	if (!line)
		return (NULL);
	if (expand)
	{
		result = expand_for_heredoc(line, -1);//how can I actally get last_status here?
		free(line);
		return (result);
	}
	result = ft_strdup(line);
	free(line);
	return (result);
}

/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted.
   - Returns 1 if delimiter starts/ends with matching quotes.
   - Used to decide whether expansion is disabled in heredoc.
*/
static int	delimiter_was_quoted(const char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	return (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')));
}

/* get_trimmed_delimiter:
   Removes surrounding quotes from heredoc delimiter if present.
   - Returns newly allocated string with quotes stripped.
   - If not quoted, returns a strdup of original delimiter.
*/
static char	*get_trimmed_delimiter(const char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	if (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')))
		return (ft_substr(delimiter, 1, len - 2));
	return (ft_strdup(delimiter));
}

/* run_heredoc:
   Reads one line of heredoc input.
   - Prompts with "> ".
   - If line matches trimmed delimiter → return 1 (stop).
   - Otherwise, expand (if expand==1), write line to fd, and continue.
   - Returns 0 if more lines expected.
*/
static int	run_heredoc(char *trimmed, int expand, int fd)
{
	char	*line;
	char	*processed;

	line = readline("> ");
	if (!line)
		return (1);
	if (str_equals(line, trimmed))
	{
		free(line);
		return (1);
	}
	processed = process_line(line, expand);
	if (!processed)
		return (free(line), 1);
	ft_putstr_fd(processed, fd);
	ft_putstr_fd("\n", fd);
	free(processed);
	return (0);
}

/* heredoc_fd:
   Full heredoc implementation for << operator.
   - Trims delimiter (removes quotes if present).
   - Determines if expansion is enabled (no quotes).
   - Reads lines until delimiter is reached.
   - Writes to temporary ".heredoc_tmp" file.
   - Returns fd opened for reading the collected heredoc text.
*/
int	heredoc_fd(const char *delimiter)
{
	int		fd;
	int		rfd;
	char	*trimmed;
	int		expand;

	trimmed = get_trimmed_delimiter(delimiter);
	expand = !delimiter_was_quoted(delimiter);
	fd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		error("heredoc");
	while (1)
	{
		if (run_heredoc(trimmed, expand, fd) == 1)
			break ;
	}
	close(fd);
	free(trimmed);
	rfd = open(".heredoc_tmp", O_RDONLY);
	unlink(".heredoc_tmp");
	return (rfd);
}
