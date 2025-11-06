/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handler.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 15:29:35 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/06 18:25:37 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

volatile sig_atomic_t	g_sigint_received = 0;

/* sets up terminal attributes, disableing display of 
	received signals */
void	disable_ctrl_echo(void)
{
	struct termios	term_state;

	if (isatty(STDIN_FILENO))
	{
		if (tcgetattr(STDIN_FILENO, &term_state) == -1)
			error("Error getting terminal attributes");
		term_state.c_lflag &= ~ECHOCTL;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &term_state) == -1)
			error("Error setting terminal attributes");
	}
}

/* does the same as above but restores original echoing */
void	enable_ctrl_echo(void)
{
	struct termios	term_state;

	if (isatty(STDIN_FILENO))
	{
		if (tcgetattr(STDIN_FILENO, &term_state) == -1)
			error("Error getting terminal attributes");
		term_state.c_lflag |= ECHOCTL;
		if (tcsetattr(STDIN_FILENO, TCSANOW, &term_state) == -1)
			error("Error setting terminal attributes");
	}
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
