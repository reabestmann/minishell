/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:28:03 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 13:57:26 by aabelkis         ###   ########.fr       */
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
	return (1);
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

/* args_len:
**   Returns the length of a NULL-terminated argument array.
*/
int	args_len(char **args)
{
	int	len;

	len = 0;
	while (args && args[len])
		len++;
	return (len);
}

/* is_quoted - used in cd_cmd.c
   Checks if a string starts with a single or double quote.
   @arg: the input string
   Returns: 1 if quoted, 0 otherwise
*/
int	is_quoted(char *arg)
{
	if (!arg)
		return (0);
	return (arg[0] == '\'' || arg[0] == '"');
}
