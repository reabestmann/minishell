/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:58:45 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/06 15:57:12 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted,
   used to decide whether expansion is disabled in heredoc.
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

/* handle_heredoc_eof:
 	Checks if readline returned NULL (EOF or Ctrl+D).
*/
static int	handle_heredoc_eof(char *line, char *trimmed_delim)
{
	if (!line)
	{
		ft_putstr_fd("warning: here-document delimited by ", STDERR_FILENO);
		ft_putstr_fd("end-of-file (wanted `", STDERR_FILENO);
		ft_putstr_fd(trimmed_delim, STDERR_FILENO);
		ft_putendl_fd("')", STDERR_FILENO);
		return (1);
	}
	return (0);
}

/* write_heredoc:
	Reads user input line by line until the delimiter is found
	or SIGINT occurs. Expands variables if delimiter was unquoted, 
	writes lines to pipe.
 */
int	write_heredoc(char *delim, int write_fd, int status)
{
	char	*line;
	char	*trimmed_delim;
	int		expand;

	trimmed_delim = remove_quotes(delim);
	expand = !delimiter_was_quoted(delim);
	g_sigint_received = 0;
	while (1)
	{
		line = read_heredoc_line();
		if (handle_heredoc_eof(line, trimmed_delim))
			break ;
		if (is_delimiter_line(line, trimmed_delim))
		{
			free(line);
			break ;
		}
		process_and_write_line(line, expand, status, write_fd);
	}
	free(trimmed_delim);
	return (0);
}

/* collect_heredocs:
  Iterates over all commands, allocates arrays, and applies heredocs;
  stops on error. Parent ignores signals while child reads heredoc.*/
int	collect_heredocs(t_command *cmds)
{
	t_command	*current;
	int			result;

	parent_signal_setup();
	current = cmds;
	result = 0;
	while (current)
	{
		if (current->heredoc_count > 0)
		{
			current->heredoc_fds = handle_malloc(sizeof(int)
					* current->heredoc_count);
			if (apply_heredocs(current) < 0)
			{
				free(current->heredoc_fds);
				current->heredoc_fds = NULL;
				result = -1;
				break ;
			}
		}
		current = current->next;
	}
	init_signals();
	return (result);
}
