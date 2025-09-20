/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 18:06:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/20 18:22:08 by rbestman         ###   ########.fr       */
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

int	syntax_valid(t_token *tokens)
{
	t_token	*cpy;

	if (tokens && tokens->type == TOKEN_PIPE)
		return (0);
	cpy = tokens;
	while (cpy)
	{
		if (check_pipe_syntax(cpy) == -1)
				return (0);
		// todo: add more syntax checks that might be missing
		cpy = cpy->next;
	}
	return (1);
}