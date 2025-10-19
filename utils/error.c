/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 19:43:10 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/15 17:03:08 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* exec_error:
	calls perror and exits
	using specified message & status.
*/
void	exec_error(const char *msg, int status)
{
	perror(msg);
	exit(status);
}

/* exec_error_custom
	prints a custom message like this:
	"minishell: cmd: msg \n"
	to fd 2 (error fd)
*/
void	exec_error_custom(const char *cmd, const char *msg, int status)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd((char *)cmd, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("\n", 2);
	exit(status);
}

/* exec_error_custom
	prints a custom message like this:
	"cmd: msg \n"
	to fd 2 (error fd)
*/
void	exec_error_custom_simple(const char *cmd, const char *msg, int status)
{
	ft_putstr_fd((char *)cmd, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd((char *)msg, 2);
	ft_putstr_fd("\n", 2);
	exit(status);
}

/* error:
    calls perror and exits on failure
    (wrapper function to safe lines)
*/
void	error(const char *msg)
{
	perror(msg);
	exit(1);
}
