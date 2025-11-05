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

/* helper for apply_redirections:
	*/
char	*expand_arg_keep_quotes_simple(char *arg, t_env *head, int last_status)
{
	int		i;
	char	state;
	char	*result;
	char	*tmp;

	i = 0;
	state = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (arg[i])
	{
		result = set_state(arg, &i, result, &state);
		if (arg[i] == '$' && state != '\'')
		{
			tmp = expand_one_arg(arg, &i, head, last_status);
			if (!tmp)
				return (free(result), NULL);
			result = append_normal_text(tmp, result);
			free(tmp);
		}
		else
			result = handle_normal_txt(&i, arg, result);
	}
	return (result);
}

/* helper for apply_heredocs: 
	reads all content from src_fd and writes it to dest_fd */
void	merge_fd_into_pipe(int src_fd, int dest_fd)
{
	char	*line;

	line = get_next_line(src_fd);
	while (line)
	{
		write(dest_fd, line, ft_strlen(line));
		free(line);
		line = get_next_line(src_fd);
	}
	close(src_fd);
}

/* free_heredocs:
 *   Frees all heredoc delimiters and file descriptor arrays for all commands.*/
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
