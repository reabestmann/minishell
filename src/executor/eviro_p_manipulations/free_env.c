/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:01:36 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 20:28:39 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

1. free_env_struct(t_env *head)
   - Purpose: Frees a linked list of t_env nodes, including keys and values.
   - Variables:
     * head: pointer to the first node of the linked list; iterates through list.
     * temp: temporary pointer to store next node while freeing current node.
   - Called from: Anywhere you need to free a t_env linked list (e.g., on error or program exit).
   - Calls: free().
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

2. free_envp_array(char **envp)
   - Purpose: Frees a dynamically allocated array of environment strings.
   - Variables:
     * envp: array of strings.
     * i: index to iterate through envp.
   - Called from: Anywhere you need to free a char ** representing the environment.
   - Calls: free().
void free_envp_array(char **envp)
{
	int i;

	i = 0;
	while(envp[i])
	{
		free(envp[i]);
		i++;	
	}
	free(envp);
}