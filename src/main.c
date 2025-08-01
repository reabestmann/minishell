/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/01 19:46:42 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* compile with cc -Wall -Wextra -Werror main.c -lreadline -Llibft -lft */
int	main(void)
{
	char	*input;

	while (1)
	{
		input = readline("mini	");
		if (!input)
			break ;
		if (!ft_strncmp(input, "exit", 5) && input[4] == 0)
		{
			free(input);
			return (0);
		}
		printf("You typed: %s\n", input);
		add_history(input);
		free(input);
	}
	return (0);
}
