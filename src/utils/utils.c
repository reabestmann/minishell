/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:28:03 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/24 20:00:02 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* get_exit_status:
   Converts waitpid status into a shell-style exit code.
   - status: the integer from waitpid
   - returns: exit code (0-255), like bash
*/
int	get_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1); // fallback error
}

/* str_equals: 
	compares a string to a target for exact equality.
	Uses ft_strncmp and also checks that str has no extra characters.
	Returns 1 if equal, otherwise 0.
*/
int	str_equals(const char *str, const char *target)
{
	size_t	len;

	if (!str || !target)
		return (0);
	len = 0;
	while (target[len])
		len++;
	if (ft_strncmp(str, target, len) == 0 && str[len] == '\0')
		return (1);
	return (0);
}

/* handle_malloc:
    calls malloc and handles failure
    by calling error if the return is NULL.
*/
void	*handle_malloc(size_t bytes)
{
	void	*ret;

	ret = malloc(bytes);
	if (NULL == ret)
		error("Malloc");
	return (ret);
}