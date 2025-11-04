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

/* set_redirection:
Helper for parse_redirection.
Sets cmd->outfile and cmd->append type based on the token.
*/
void	set_redirection(t_command *cmd, t_token *token, int append_type)
{
	int	fd;

	if (!token || token->type != TOKEN_WORD)
		return ;
	if (cmd->fd_type == STDOUT_FILENO || cmd->fd_type == 3)
	{
		if (append_type == 1)
			fd = open(token->val, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
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
