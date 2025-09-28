/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:58:45 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/28 15:59:03 by rbestman         ###   ########.fr       */
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

static void write_heredoc(char *delim, int write_fd, int status)
{
    char    *line;
    char    *trimmed_delim;
    char    *trimmed_line;
    int     expand;

    trimmed_delim = ft_strtrim(delim, "'\"");
    expand  = !delimiter_was_quoted(delim);

    while (1)
    {
        ft_putstr_fd("> ", STDERR_FILENO);
        line = get_next_line(STDIN_FILENO);
        if (!line)
            break ;
        trimmed_line = ft_strtrim(line, "\n");
        if (str_equals(trimmed_line, trimmed_delim))
        {
            free(trimmed_line);
            free(line);
            break;
        }
        free(trimmed_line);
        line = check_expand_line(line, expand, status);
        write(write_fd, line, ft_strlen(line));
        free(line);
    }
    free(trimmed_delim);
}

void    apply_heredocs(t_command *cmd, int status)
{
    int hd_pipe[2];
    int i;

    if (!cmd->heredoc_count)
        return ;
    if (pipe(hd_pipe) == -1)
        error("pipe heredoc");
    i = -1;
    while (++i < cmd->heredoc_count)
        write_heredoc(cmd->heredoc_delim[i], hd_pipe[1], status);
    close(hd_pipe[1]);
    fd_check(hd_pipe[0], STDIN_FILENO, "heredoc");
}