/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_cmd_bubble.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 17:57:48 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 18:13:51 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* sort(envp, j)
   Swaps two adjacent strings in the envp array.
   Used by alphabatize to reorder strings alphabetically.
*/
static void	sort(char **envp, int j)
{
	char	*temp;

	temp = envp[j];
	envp[j] = envp[j + 1];
	envp[j + 1] = temp;
	return ;
}

/* keys_differ(key_one, key_two, envp, j)
   Purpose: Handles the case when two keys differ
   and the first should come after the second.
   Returns: 1 to indicate a swap occurred.
   Notes: Frees allocated keys and calls sort to swap the strings in envp.
*/
static int	keys_differ(char *key_one, char *key_two, char **envp, int j)
{
	free_keys(key_one, key_two);
	sort(envp, j);
	return (1);
}

/* alphabatize(envp, j)
   Compares two adjacent env strings and swaps them
   if they are out of order.
   Returns: 1 if a swap occurred, 0 if already in order.
*/
static int	alphabatize(char **envp, int j)
{
	char	*key_one;
	char	*key_two;
	int		i;
	int		min_len;

	i = 0;
	min_len = key_info(envp, j, &key_one, &key_two);
	while (i < min_len)
	{
		if (key_one[i] > key_two[i])
			return (keys_differ(key_one, key_two, envp, j));
		else if (key_one[i] < key_two[i])
		{
			free_keys(key_one, key_two);
			return (0);
		}
		i++;
	}
	if (ft_strlen(key_one) > ft_strlen(key_two))
		return (keys_differ(key_one, key_two, envp, j));
	free_keys(key_one, key_two);
	return (0);
}

/* ft_bubble_sort(envp)
   - Purpose: Sorts an array of environment strings alphabetically using 
   bubble sort. Stop early if the array is already sorted.
*/
void	ft_bubble_sort(char **envp)
{
	int	i;
	int	j;
	int	n;
	int	swap;

	n = 0;
	while (envp[n] != NULL)
		n++;
	i = 0;
	while (i < n - 1)
	{
		swap = 0;
		j = 0;
		while (j < n - i - 1)
		{
			if (alphabatize(envp, j))
				swap = 1;
			j++;
		}
		if (!swap)
			return ;
		i++;
	}
}
