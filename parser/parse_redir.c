/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 15:02:35 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* set_redirection:
Helper for parse_redirection.
Sets cmd->outfile and cmd->append type based on the token. */
void	set_redirection(t_command *cmd, t_token *token,
							int append_type, int last)
{
	int	fd;

	if (!token || token->type != TOKEN_WORD)
		return ;
	fd = -1;
	if (cmd->fd_type == STDOUT_FILENO || cmd->fd_type == 3)
	{
		if (append_type == 1 && !last)
			fd = open(token->val, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (append_type == 2 && !last)
			fd = open(token->val, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd >= 0)
			close(fd);
		if (cmd->outfile)
			free(cmd->outfile);
		cmd->outfile = ft_strdup(token->val);
		cmd->append = append_type;
	}
	if (cmd->fd_type == STDERR_FILENO || cmd->fd_type == 3)
	{
		if (cmd->errfile)
			free(cmd->errfile);
		cmd->errfile = ft_strdup(token->val);
		cmd->append_err = append_type;
	}
}

/* handle_input_redir:
 * Sets cmd->infile from the next token if it exists and is a word.
 * Frees previous infile if needed.*/
void	handle_input_redir(t_command *cmd, t_token *next)
{
	if (next && next->type == TOKEN_WORD)
	{
		if (cmd->infile)
			free(cmd->infile);
		cmd->infile = ft_strdup(next->val);
	}
}

/* add_heredoc:
	 Adds a new heredoc delimiter to the command struct */
void	add_heredoc(t_command *cmd, const char *delimiter)
{
	char	**arr;
	int		i;
	int		j;

	i = 0;
	while (cmd->heredoc_delim && cmd->heredoc_delim[i])
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

void	parse_redirection(t_command *cmd, t_token **cpy)
{
	t_token	*next;
	int		append_type;
	int		last;

	if (!*cpy)
		return ;
	next = (*cpy)->next;
	append_type = 2;
	if (is_append_type((*cpy)->type))
		append_type = 1;
	set_fd_type(cmd, *cpy);
	if ((*cpy)->type == TOKEN_REDIR_IN)
		handle_input_redir(cmd, next);
	else if (is_output_redir((*cpy)->type))
	{
		last = is_last_redir(*cpy);
		set_redirection(cmd, next, append_type, last);
	}
	else if ((*cpy)->type == TOKEN_HEREDOC)
	{
		if (next && next->type == TOKEN_WORD)
			add_heredoc(cmd, next->val);
	}
	if (next)
		*cpy = next;
}
