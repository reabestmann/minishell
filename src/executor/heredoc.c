/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:58:45 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/29 11:24:19 by aabelkis         ###   ########.fr       */
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

static int	write_heredoc(char *delim, int write_fd, int status)
{
	char	*line;
	char	*trimmed_delim;
	char	*trimmed_line;
	int		expand;

	trimmed_delim = ft_strtrim(delim, "'\"");
	expand = !delimiter_was_quoted(delim);
	g_sigint_received = 0;
	while (1)
	{
		ft_putstr_fd("> ", STDERR_FILENO);
		line = get_next_line(STDIN_FILENO);
		if (!line)
			break ;
		if (g_sigint_received)
		{
			free(line);
			free(trimmed_delim);
			return (-1);
		}
		trimmed_line = ft_strtrim(line, "\n");
		if (str_equals(trimmed_line, trimmed_delim))
		{
			free(trimmed_line);
			free(line);
			break ;
		}
		free(trimmed_line);
		line = check_expand_line(line, expand, status);
		write(write_fd, line, ft_strlen(line));
		free(line);
	}
	free(trimmed_delim);
	return (0);
}

int	apply_heredocs(t_command *cmd, int status)
{
	int	hd_pipe[2];
	int	i;

	if (!cmd->heredoc_count)
		return (0);
	i = -1;
	while (++i < cmd->heredoc_count)
	{
		if (pipe(hd_pipe) == -1)
			error("pipe heredoc");
		if (write_heredoc(cmd->heredoc_delim[i], hd_pipe[1], status))
		{
			close(hd_pipe[0]);
			close(hd_pipe[1]);
			return (-1);
		}
		close(hd_pipe[1]);
		cmd->heredoc_fds[i] = hd_pipe[0];
	}
	return (0);
}

int	collect_heredocs(t_command *cmds, int status)
{
	t_command	*current;

	current = cmds;
	while (current)
	{
		if (current->heredoc_count > 0)
		{
			current->heredoc_fds = handle_malloc(sizeof(int)
					* current->heredoc_count);
			if (apply_heredocs(current, status) < 0)
			{
				free(current->heredoc_fds);
				current->heredoc_fds = NULL;
				return (-1);
			}
		}
		current = current->next;
	}
	return (0);
}
