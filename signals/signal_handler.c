/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 15:29:35 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/06 15:57:21 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

volatile sig_atomic_t	g_sigint_received = 0;

/* heredoc_sigint_handler:
   Signal handler for heredoc child 
   - just prints a nl & exits with 130.
*/
void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}

/* ctrl-C displays a new prompt on a new line,
	called by init_signals.*/
static void	sigint_handler(int sig)
{
	(void)sig;
	g_sigint_received = 1;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/* init_signals:
	sets up the shells signal handling:
	SIGQUIT(Ctrl+\) -> ignored
	SIGINT(Ctrl+C) -> sigint_handler */
void	init_signals(void)
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		error("error with sigint_handler");
	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		error("error with sigquit");
}

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
