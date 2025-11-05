/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_utils2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 14:54:18 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 15:03:52 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* set_fd_type:
Helper for parse_redirection.
Sets the file descriptor we want to write to.
(std_out, std_err or both)
*/
void	set_fd_type(t_command *cmd, t_token *cpy)
{
	cmd->fd_type = STDOUT_FILENO;
	if (cpy->type == TOKEN_REDIR_ERR || cpy->type == TOKEN_REDIR_ERR_APPEND)
		cmd->fd_type = STDERR_FILENO;
	if (cpy->type == TOKEN_REDIR_BOTH || cpy->type == TOKEN_REDIR_BOTH_APPEND)
		cmd->fd_type = 3;
}

/* is_append_type:
 * Returns 1 if token type indicates append redirection (>>),
 * including stdout, stderr, or both.*/
int	is_append_type(int type)
{
	return (type == TOKEN_REDIR_APPEND
		|| type == TOKEN_REDIR_ERR_APPEND
		|| type == TOKEN_REDIR_BOTH_APPEND);
}

/* is_output_redir:
 * Returns 1 if token type is any kind of output redirection:
 * >, >>, stderr, or both.*/
int	is_output_redir(int type)
{
	return (type == TOKEN_REDIR_OUT
		|| type == TOKEN_REDIR_ERR
		|| type == TOKEN_REDIR_BOTH
		|| type == TOKEN_REDIR_APPEND
		|| type == TOKEN_REDIR_ERR_APPEND
		|| type == TOKEN_REDIR_BOTH_APPEND);
}

/* Checks if the current redirection token is the last one
	for the same fd type */
int	is_last_redir(t_token *current)
{
	t_token	*tmp;

	tmp = current->next;
	if (!current)
		return (1);
	while (tmp)
	{
		if (tmp->type == TOKEN_REDIR_OUT
			|| tmp->type == TOKEN_REDIR_APPEND)
		{
			if (current->type == TOKEN_REDIR_OUT
				|| current->type == TOKEN_REDIR_APPEND)
				return (0);
		}
		if (tmp->type == TOKEN_REDIR_ERR
			|| tmp->type == TOKEN_REDIR_ERR_APPEND)
		{
			if (current->type == TOKEN_REDIR_ERR
				|| current->type == TOKEN_REDIR_ERR_APPEND)
				return (0);
		}
		tmp = tmp->next;
	}
	return (1);
}
