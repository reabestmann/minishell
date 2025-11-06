/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_and_tty_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 18:17:23 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/06 18:21:21 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/* heredoc_sigint_handler:
   Signal handler for heredoc child - just exits with 130.
*/
void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}

/* reset_terminal_mode:
   Resets terminal to canonical mode for child processes.
   Enables all standard terminal features for interactive programs. */
void	reset_terminal_mode(void)
{
	struct termios	term_state;

	if (isatty(STDIN_FILENO))
	{
		if (tcgetattr(STDIN_FILENO, &term_state) == -1)
			return ;
		term_state.c_lflag |= (ICANON | ECHO | ECHOCTL | ECHOE | ECHOK);
		term_state.c_lflag &= ~ECHONL;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &term_state) == -1)
			return ;
	}
}