/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 15:46:22 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/22 23:38:15 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * quote_check:
 *  - Sets the quote state when a single (') or double (") quote is found.
 *  - Inputs:
 *      arg   : string being processed
 *      i     : pointer to current index in arg
 *      state : pointer to current quote state (0 if none, '\'' or '"')
 *  - Output: Updates *state to indicate current quote context
 */
void	quote_check(char *arg, int *i, char *state)
{
	if (arg[*i] == '\'')
		*state = '\'';
	else if (arg[*i] == '"')
		*state = '"';
}

/*
 * found:
 *  - Checks if the quote at arg[*i] has a matching
 *  closing quote later in the string.
 *  - Inputs:
 *      arg : string being processed
 *      i   : pointer to current index of the quote in arg
 *  - Output: Returns 1 if a closing quote is found, 0 otherwise
 */
int	found(char *arg, int *i)
{
	int	k;
	int	found;

	k = *i + 1;
	found = 0;
	while (arg[k])
	{
		if (arg[k] == arg[*i])
		{
			found = 1;
			break ;
		}
		k++;
	}
	return (found);
}

/* arg_loop:
 *  - Processes a single character of arg and copies it 
 to clean while removing quotes.
 *  - Handles single and double quotes using a 
 * static state variable.
 *  - Preserves unclosed quotes at their original positions.
 *  - Inputs:
 *      arg   : original string
 *      i     : pointer to current index in arg
 *      j     : pointer to current index in clean
 *      clean : destination buffer for string without quotes
 *  - Behavior:
 *      - Skips quotes only if they are properly closed
 *      - Copies all other characters to clean
 *      - Resets static state at the end of the string
 */
void	arg_loop(char *arg, int *i, int *j, char *clean)
{
	static char	state = 0;

	if (state == 0)
	{
		if (arg[*i] == '\'' || arg[*i] == '"')
		{
			if (found(arg, i))
				state = arg[*i];
			else
				clean[(*j)++] = arg[*i];
		}
		else
			clean[(*j)++] = arg[*i];
	}
	else
	{
		if (arg[*i] == state)
			state = 0;
		else
			clean[(*j)++] = arg[*i];
	}
	if (arg[*i + 1] == '\0')
		state = 0;
}

/*
 * remove_quotes:
 *  - Removes quotes from a single string while preserving content inside quotes.
 *  - Input:
 *      arg : string to process
 *  - Output:
 *      Returns a newly allocated string with quotes removed
 *      Caller is responsible for freeing the returned string
 */
char	*remove_quotes(char *arg)
{
	int		i;
	int		j;
	char	*clean;

	i = 0;
	j = 0;
	clean = malloc(ft_strlen(arg) + 1);
	if (!clean)
		return (NULL);
	while (arg[i])
	{
		arg_loop(arg, &i, &j, clean);
		i++;
	}
	clean[j] = '\0';
	return (clean);
}

/*
 * trim_quotes_for_execution:
 *  - Removes quotes from all strings in a null-terminated array of arguments.
 *  - Input:
 *      args : array of strings
 *  - Behavior:
 *      - Calls remove_quotes for each argument
 *      - Frees the original string
 *      - Replaces it with the cleaned version
 */
void	trim_quotes_for_execution(char **args)
{
	int		i;
	char	*cleaned;

	i = 0;
	while (args[i])
	{
		cleaned = remove_quotes(args[i]);
		free(args[i]);
		args[i] = cleaned;
		i++;
	}
}
