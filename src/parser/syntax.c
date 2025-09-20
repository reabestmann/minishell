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

int	check_pipe_syntax(t_command *current, t_token *token)
{
	if (!token || token->type != TOKEN_PIPE)
		return (0);
	if (!current->args || current->args[0] ||
		!token->next || token->next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: parse error near pipe.\n", 2);
		return (-1);
	}
	return (0);
}
