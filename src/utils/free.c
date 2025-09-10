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

/* free_array(char **array)
   - Purpose: Frees a dynamically allocated array of environment strings.
   - Variables:
     * envp: array of strings.
     * i: index to iterate through envp.
   - Called from: Anywhere you need to free a char ** representing the environment.
   - Calls: free(). */
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

/* free_env_struct(t_env *head):
   - Purpose: Frees a linked list of t_env nodes, including keys and values.
   - Variables:
     * head: pointer to the first node of the linked list; iterates through list.
     * temp: temporary pointer to store next node while freeing current node.
   - Called from: Anywhere you need to free a t_env linked list (e.g., on error or program exit).
   - Calls: free(). */
void free_env_struct(t_env *head)
{
	t_env *temp;

	while(head)
	{
		temp = head->next;
		free(head->key);
		free(head->value);
		free(head);
		head = temp;	
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
