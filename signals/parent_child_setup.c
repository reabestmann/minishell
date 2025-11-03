/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent_child_setup.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 15:29:42 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/17 15:41:55 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*here I set up sigaction struct so default actions occur instead of what we 
	had in main and then execute SIGINT, SIGQUIT and SIGPIPE signals in child*/
void	child_signal_setup(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		write(2, "error with sigint_handler \n", 26);
		exit(1);
	}
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		write(2, "error with sigquit \n", 19);
		exit(1);
	}
	if (sigaction(SIGPIPE, &sa, NULL) == -1)
	{
		write(2, "error with sigpipe\n", 19);
		exit(1);
	}
}

/*here I set up sigaction struct so that signals are ignored instead 
	of what we had in main (for parent process)*/
void	parent_signal_setup(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		write(2, "error with sigint_handler \n", 26);
		exit(1);
	}
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		write(2, "error with sigquit \n", 19);
		exit(1);
	}
}
