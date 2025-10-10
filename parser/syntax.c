/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 18:06:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/26 21:08:37 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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

static int	check_redirection_syntax(t_token *token)
{
	if (!token->next || token->next->type != TOKEN_WORD)
	{
		ft_putstr_fd("minishell: parse error near redirection\n", 2);
		return (-1);
	}
	return (0);
}

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
		{
			if (check_pipe_syntax(cpy) == -1)
				return (2);
			if (check_redirection_syntax(cpy) == -1)
				return (2);
		}
		// todo: add more syntax checks that might be missing
		cpy = cpy->next;
	}
	return (0);
}
