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
 * 1. update_quotes:
 * helper function to avoid repetition across split functions -
 * keeps track wether input is within simple or double quotes
 */
void	update_quotes(char c, int *in_quote, int *in_dquote)
{
	if (c == '\'' && !(*in_dquote))
		*in_quote = !(*in_quote);
	else if (c == '\"' && !(in_quote))
		*in_dquote = !(*in_dquote);
}

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
