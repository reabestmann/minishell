/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 15:46:22 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 14:05:25 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/*
 * quote_check:
 *   Sets the quote state if current char is ' or ".*/
void	quote_check(char *arg, int *i, char *state)
{
	if (arg[*i] == '\'')
		*state = '\'';
	else if (arg[*i] == '"')
		*state = '"';
}

/** found:
 *   Returns 1 if quote at arg[*i] has a matching closing quote later.*/
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

/** arg_loop:
 *   Processes arg[*i] for clean[], skipping quotes if properly closed.
 *   Preserves unclosed quotes; handles single/double quote state.*/
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

/*remove_quotes:
 *  - Removes quotes from a single string while preserving 
 content inside quotes.*/

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

/*trim_quotes_for_execution:
 *   Removes quotes from all strings in args array, replacing originals.*/
void	trim_quotes_for_execution(char **args)
{
	int		i;
	char	*cleaned;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		cleaned = remove_quotes(args[i]);
		free(args[i]);
		args[i] = cleaned;
		i++;
	}
}
