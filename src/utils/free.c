/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 15:10:03 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/11 15:18:50 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_array(char **array)
{
	int	i;

	i = -1;
	while (array[++i])
		free(array[i]);
	free(array);
}

void	free_tokens(t_token *tokens)
{
	t_token *tmp;

	while (tokens)
	{
		tmp = tokens->next;
		free(tokens->val);
		free(tokens);
		tokens = tmp;
	}
}

void	free_commands(t_command *cmds)
{
	t_command *tmp;

	while (cmds)
	{
		tmp = cmds->next;
		free_array(cmds->args);
		free(cmds->infile);
		free(cmds->outfile);
		free(cmds);
		cmds = tmp;
	}
}

/* TO DO : put in different file */ 
void	*handle_malloc(size_t bytes)
{
	void	*ret;

	ret = malloc(bytes);
	if (NULL == ret)
		error("Malloc");
	return (ret);
}

void	error(const char *msg)
{
	perror(msg);
	exit(1);
}
