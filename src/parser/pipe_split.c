/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_split.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 14:22:55 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/05 15:41:25 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * 1. skip_space
 * helper function to avoid repetition across split functions -
 * skips whitespaces starting from index i in str.
 * returns first index after all spaces.
 */
int	skip_space(char *str, int i)
{
	while (str[i] == ' ')
		i++;
	return (i);
}

/*
 * 2. count_commands
 * Counts how many commands are seperated by unquoted pipes ('|').
 * Starts at 1 since one command means no pipes are needed.
 */
static int	count_commands(char *str)
{
	int	i;
	int	count;
	int	in_quote;
	int	in_dquote;

	i = 0;
	count = 1;
	in_quote = 0;
	in_dquote = 0;
	while (str[i])
	{
		update_quotes(str[i], &in_quote, &in_dquote);
		if (str[i] == '|' && !in_quote && !in_dquote)
			count++;
		i++;
	}
	return (count);
}

/*
 * 3. command_len
 * Calculates the length of the current command segment 
 * staring from a given index start until an unquoted pipe ('|') is found.
 */ 
static int	command_len(char *str, int start)
{
	int	len;
	int	in_quote;
	int	in_dquote;

	len = 0;
	in_quote = 0;
	in_dquote = 0;
	while (str[start + len])
	{
		update_quotes(str[start + len], &in_quote, &in_dquote);
		if (str[start + len] == '|' && !in_quote && !in_dquote)
			break ;
		len++;
	}
	return (len);
}

/*
 * 4. split_pipe
 * Splits the input string into seperate commands using unquoted pipes ('|')
 * as a seperator. Skips spaces and respects quotes.
 * Returns an array of command strings.
 */
char	**split_pipe(char *str)
{
	char	**array;
	int	i;
	int	j;
	int	len;

	array = malloc(sizeof(char *) * (count_commands(str) + 1));
	if (!array)
		return (NULL);

	i = 0;
	j = 0;
	while (str[i])
	{
		i = skip_space(str, i);
		if (str[i])
		{
			len = command_len(str, i);
			array[j] = ft_substr(str, i, len);
			j++;
			i +=len;
			if (str[i] == '|')
				i++;
		}
	}
	array[j] = NULL;
	return (array);
}
