/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 18:06:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 13:43:48 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* check_pipe_syntax:
 * Verifies a PIPE token is followed by a valid token.
 * Prints error if PIPE is at end or followed by another PIPE.*/
static int	check_pipe_syntax(t_token *token)
{
	if (!token || token->type != TOKEN_PIPE)
		return (0);
	if (!token->next || token->next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: parse error near pipe.\n", 2);
		return (-1);
	}
	return (0);
}

/* is_redir_token:
 * Returns 1 if type is any redirection operator (<, >, >>, <<).*/
static int	is_redir_token(int type)
{
	return (type == TOKEN_REDIR_OUT || type == TOKEN_REDIR_IN
		|| type == TOKEN_REDIR_APPEND || type == TOKEN_HEREDOC);
}

/* check_redir_syntax:
 * Verifies redirection token is followed by a WORD or another redirection.
 * Prints error if syntax is invalid.*/
static int	check_redir_syntax(t_token *token)
{
	if (!token->next)
	{
		ft_putstr_fd("minishell: parse error near redirection\n", 2);
		return (-1);
	}
	if (token->next->type != TOKEN_WORD
		&& !is_redir_token(token->next->type))
	{
		ft_putstr_fd("minishell: parse error near redirection\n", 2);
		return (-1);
	}
	return (0);
}

/* check_start:
 * Ensures the first token is not a PIPE.
 * Prints error if command starts with PIPE.*/
static int	check_start(t_token *tokens)
{
	if (tokens && tokens->type != TOKEN_WORD)
	{
		if (tokens->type == TOKEN_PIPE)
		{
			ft_putstr_fd("minishell: parse error near start of command\n", 2);
			return (-1);
		}
		else
			return (0);
	}
	return (0);
}

/* syntax_valid:
 * Iterates through token list to validate overall command syntax.
 * Checks quotes (via state), PIPE, and redirection syntax.
 * Returns 0 if valid, 2 if parse errors found.*/
int	syntax_valid(t_token *tokens)
{
	t_token	*cpy;
	char	state;

	state = 0;
	if (check_start(tokens) == -1)
		return (2);
	cpy = tokens;
	while (cpy)
	{
		set_state_str(cpy->val, &state);
		if (cpy->type != TOKEN_WORD && state == 0)
			if (check_pipe_syntax(cpy) == -1 || check_redir_syntax(cpy) == -1)
				return (2);
		if (is_redir_token(cpy->type) && cpy->next)
		{
			if (cpy->next->type == TOKEN_PIPE
				|| is_redir_token(cpy->next->type))
			{
				ft_putstr_fd("minishell: parse error near redirection\n", 2);
				return (2);
			}
		}
		cpy = cpy->next;
	}
	return (0);
}
