/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:58:45 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/30 19:59:38 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_heredocs(t_command *cmds)
{
	int	i;

	while (cmds)
	{
		if (cmds->heredoc_count > 0 && cmds->heredoc_delim)
		{
			i = 0;
			while (i < cmds->heredoc_count)
			{
				free(cmds->heredoc_delim[i]);
				cmds->heredoc_delim[i] = NULL;
				i++;
			}
			free(cmds->heredoc_delim);
			cmds->heredoc_delim = NULL;
		}
		if (cmds->heredoc_fds)
		{
			free(cmds->heredoc_fds);
			cmds->heredoc_fds = NULL;
		}
		cmds = cmds->next;
	}
}

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

static void	print_heredoc_prompt(void)
{
	int	tty_fd;

	tty_fd = open("/dev/tty", O_WRONLY);
	if (tty_fd >= 0)
	{
		ft_putstr_fd("> ", tty_fd);
		close(tty_fd);
	}
}

static int	is_delimiter_line(char *line, char *trimmed_delim)
{
	char	*trimmed_line;
	int		match;

	trimmed_line = ft_strtrim(line, "\n");
	match = str_equals(trimmed_line, trimmed_delim);
	free(trimmed_line);
	return (match);
}

static void	process_and_write_line(char *line, int expand,
	int status, int write_fd)
{
	line = check_expand_line(line, expand, status);
	write(write_fd, line, ft_strlen(line));
	free(line);
}

//returns 1 if sigint was received, 0 otherwise
static int	handle_sigint_in_heredoc(char *line, char *trimmed_delim)
{
	if (g_sigint_received)
	{
		free(line);
		free(trimmed_delim);
		return (1);
	}
	return (0);
}

static int	write_heredoc(char *delim, int write_fd, int status)
{
	char	*line;
	char	*trimmed_delim;
	int		expand;

	trimmed_delim = remove_quotes(delim);
	expand = !delimiter_was_quoted(delim);
	g_sigint_received = 0;
	while (1)
	{
		print_heredoc_prompt();
		line = get_next_line(STDIN_FILENO);
		if (!line)
			break ;
		if (handle_sigint_in_heredoc(line, trimmed_delim))
			return (-1);
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

/*static int	write_heredoc(char *delim, int write_fd, int status)
{
	char	*line;
	char	*trimmed_delim;
	char	*trimmed_line;
	int		expand;
	int		tty_fd;

	trimmed_delim = remove_quotes(delim);
	expand = !delimiter_was_quoted(delim);
	g_sigint_received = 0;
	while (1)
	{
		tty_fd = open("/dev/tty", O_WRONLY);
		if (tty_fd >= 0)
		{
			ft_putstr_fd("> ", tty_fd);
			close(tty_fd);
		}
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
}*/

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
