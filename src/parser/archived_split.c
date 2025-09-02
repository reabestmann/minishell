/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 13:52:46 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/05 15:03:15 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
 * 2. words_array:
 * Counts how many words are in the input string, ignoring spaces outside quotes.
 * It helps decide how big the result array should be.
 * Returns memory for that many words plus one for the NULL end.
 */
static char	**words_array(char *str)
{
	int		i;
	int		wc;
	int		in_quote;
	int		in_dquote;

	i = 0;
	wc = 0;
	in_quote = 0;
	in_dquote = 0;
	while (str[i])
	{
		i = skip_space(str, i);
		if (str[i])
			wc++;
		while (str[i] && (in_quote || in_dquote|| str[i] != ' '))
			update_quotes(str[i++], &in_quote, &in_dquote);
	}
	return (malloc(sizeof(char *) * (wc + 1)));
}

/*
 * 3. str_len:
 * Finds how long a word is starting at a certain place in the string.
 * It counts characters until it hits a space outside quotes.
 * Quotes are handled so words with spaces inside quotes stay together.
 */
static int	str_len(char *str, int i)
{
	int	len;
	int	in_quote;
	int	in_dquote;

	len = 0;
	in_quote = 0;
	in_dquote = 0;
	while (str[i] && (in_quote || in_dquote || str[i] != ' '))
	{
		update_quotes(str[i++], &in_quote, &in_dquote);
		len++;
	}
	return (len);
}

/*
 * 4. fill_array:
 * Copies a word from the input string.
 * If the word starts and ends with quotes, those quotes are removed.
 * Returns the cleaned-up word as a new string.
 */
static char	*fill_array(char *str, int i, int len)
{
	char	*word;
	char	*temp;

	temp = ft_substr(str, i, len);
	if ((temp[0] == '\'' && temp[len - 1] == '\'') ||
		((temp[0] == '\"' && temp[len - 1] == '\"')))
	{
		word = ft_substr(temp, 1, len - 2);
		free(temp);
	}
	else
		word = temp;
	return (word);
}

/*
 * 5. split_input:
 * Breaks the input string into words.
 * Words inside quotes stay together.
 * Uses the helper functions to find words and clean quotes.
 * Returns an array of words, ending with NULL.
 */
char	**split_input(char *str)
{
	int		i;
	int		j;
	int		len;
	char	**array;

	i = 0;
	j = 0;
	len = 0;
	array = words_array(str);
	if (!array)
		return (NULL);
	while (str[i])
	{
		i = skip_space(str, i);
		if (str[i])
		{
			len = str_len(str, i);
			array[j++] = fill_array(str, i, len);
			i += len;
		}
	}
	array[j] = NULL;
	return (array);
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