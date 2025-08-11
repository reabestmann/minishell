/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/05 15:37:46 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* to compile, use:
cc -Wall -Wextra -Werror main.c parser/split.c parser/pipe_split.c parser/parse.c executor/exec.c utils/free.c -o minishell -lreadline -Llibft -lft
*/
/* ctrl-C displays a new prompt on a new line.*/
/*
void sigint_handler(int signo)
{
	(void)signo;
	write(STDOUT_FILENO, "\n", 1);      // print a newline
	rl_on_new_line();                   // tell readline a new line is coming
	rl_replace_line("", 0);             // clear the current input line
	rl_redisplay();                     // redisplay the prompt
}*/
/*is called from init_signals*/
static void sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*	sigemtpyset(&sa.sa_mask) means: don’t block any additional signals during your handler. 
	sa.sa_flags = SA_RESTART; means: restart any interrupted signals
	struct sigaction {
    void     (*sa_handler)(int);   // Pointer to signal handler function OR special values:
                                   // SIG_DFL (default action), SIG_IGN (ignore)
    
    sigset_t sa_mask;              // Signals to block (mask) during execution of handler
    
    int      sa_flags;             // Flags to modify behavior (e.g., SA_RESTART)
    
    void     (*sa_restorer)(void); // (obsolete, usually unused)
};
Ctrl-C sends SIGINT signal therefore we are waiting for it and sending it to sigint_handler
subject wants us to do nothing for ctr-\ ie sig_ignore
so we make the struct - set various aspects and then call sigaction on it based on that set up - which is different for Ctrl-C and Ctrl-\
this function is being called from main and calls sigint_handler in case of ctrl-C*/
/*sigemptyset(&sa.sa_mask);
→ Initializes the signal mask to empty, meaning no signals are blocked during your handler.
sa.sa_flags = SA_RESTART;
→ Ensures that interrupted system calls (like readline()) automatically restart after the handler finishes, avoiding unwanted errors.
struct sigaction fields:
sa_handler: pointer to your signal handler function (e.g., sigint_handler) or special macros like SIG_IGN (ignore) or SIG_DFL (default action).
sa_mask: signals to block during handler execution.
sa_flags: behavior modifiers.
sa_restorer: obsolete, usually ignored.
Ctrl-C sends SIGINT, so you:
Set up the struct with sa_handler = sigint_handler.
Call sigaction(SIGINT, &sa, NULL) to register your handler.
Ctrl-\ sends SIGQUIT, which your subject wants ignored:
Set up the struct with sa_handler = SIG_IGN.
Call sigaction(SIGQUIT, &sa, NULL).
This setup function is called in main() before your input loop to ensure your shell handles these signals correctly.
When Ctrl-C is pressed, the kernel triggers sigint_handler that you defined, allowing you to handle it (e.g., redraw prompt).*/
/* also checks for error with sigaction and displays message*/
void init_signals()
{
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	sa.sa_handler = sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		write(2, "error with sigint_handler \n", 26);
		exit(1);
	}

	sa.sa_handler = SIG_IGN;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		write(2, "error with sigquit \n", 19);
		exit(1);
	}
}

int	main(int params, char **argv, char **envp)
{
	char	*input;

	(void)params;
	(void)argv;

	init_signals();

	while (1)
	{
		input = readline("mini$	");
		if (!input)
		{
			printf("exit\n");
		/*todo: clean up here ie deal with any resources*/
			break ;
		}
		if (*input)
			add_history(input);
		if (!ft_strncmp(input, "exit", 5) && input[4] == 0)
		{
			free(input);
			break ;
		}
		handle_input(input, envp);
		free(input);
	}
	/*todo: final clean up*/
	return (0);
}
