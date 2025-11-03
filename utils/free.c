/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 15:10:03 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/16 12:18:11 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* free_val:
- Frees a string for $? expansion if last_status != -1
- last_status == -1 means env value, which is not freed here
*/
void	free_val(int *last_status, char **val)
{
	if (*last_status != -1)
		free(*val);
}

/* free_array(char **array)
   - Purpose: Frees memory of a given array of strings.
   - Variables:
     * array: array of strings.
     * i: index to iterate through envp.
   - Called from: Anywhere you need to free a char ** representing 
   		the environment.
   - Calls: free(). 
*/
void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = -1;
	while (array[++i])
		free(array[i]);
	free(array);
}

/* free_tokens:
	frees all allocated memory used by tokens
	going through members of the linked list,
	freeing each component of each struct.
*/
void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		free(tokens->val);
		free(tokens);
		tokens = tmp;
	}
}

/* free_commands:
	frees all allocated memory used by commands by
	iterating through components of each member
	of the linked list.
*/
void	free_commands(t_command *cmds)
{
	t_command	*tmp;

	while (cmds)
	{
		tmp = cmds->next;
		free_array(cmds->args);
		free(cmds->infile);
		free(cmds->outfile);
		free(cmds->errfile);
		free(cmds);
		cmds = tmp;
	}
}

/* free_env_struct(t_env *head):
   - Purpose: Frees a linked list of t_env nodes, including keys and values.
   - Variables:
     * head: pointer to the first node of the linked list; iterates through list.
     * temp: temporary pointer to store next node while freeing current node.
   - Called from: Anywhere you need to free a t_env linked list (e.g., on error 
   		or program exit).
   - Calls: free(). */
void	free_env_struct(t_env *head)
{
	t_env	*temp;

	while (head)
	{
		temp = head->next;
		free(head->key);
		free(head->value);
		free(head);
		head = temp;
	}
}
