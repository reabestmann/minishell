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

/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted.
   - Returns 1 if delimiter starts/ends with matching quotes.
   - Used to decide whether expansion is disabled in heredoc.
*/
int	delimiter_was_quoted(const char *delimiter)
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
char	*get_trimmed_delimiter(const char *delimiter)
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

 write_heredoc:
   Creates and fills a temporary file for one heredoc.
   - Opens a unique .heredoc_tmp file for writing.
   - Reads lines until the given delimiter is found.
   - Expands variables if allowed (expand==1).
   - Writes all valid lines to the file.
   - Returns the fd opened for reading the heredoc content.


void	write_heredoc(const char *delimiter, int first)
{
	int fd;
	char *trimmed;
	int expand;

	trimmed = get_trimmed_delimiter(delimiter);
	expand = !delimiter_was_quoted(delimiter);

	if (first)
		fd = open(HEREDOC_TMP, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(HEREDOC_TMP, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		error("heredoc");

	while (1)
		if (run_heredoc(trimmed, expand, fd) == 1)
			break;

	close(fd);
	free(trimmed);
}*/

void	add_heredoc(t_command *cmd, const char *delimiter)
{
	char	**arr;
	int		i;
	int		j;

	i = 0;
	while(cmd->heredoc_delim && cmd->heredoc_delim[i])
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