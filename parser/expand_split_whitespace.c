/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split_whitespace.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:55:09 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:55:21 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/* is_ifs_char:
**   Checks if a character is considered whitespace (space, tab, newline).
*/
static int	is_ifs_char(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

/* count_words:
**   Counts the number of words in a string, separated by IFS characters.
*/
static int	count_words(const char *s)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (is_ifs_char(*s))
			in_word = 0;
		else if (!in_word)
		{
			in_word = 1;
			count++;
		}
		s++;
	}
	return (count);
}

/* dup_word:
**   Duplicates a substring given start and end pointers.
*/
static char	*dup_word(const char *start, const char *end)
{
	size_t	len;
	char	*word;

	len = end - start;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	ft_memcpy(word, start, len);
	word[len] = '\0';
	return (word);
}

/* ft_split_whitespace:
**   Splits a string into words based on whitespace (IFS).
**   Returns a NULL-terminated array of strings.
*/

static int	process_word(const char **str, char **result, int *i)
{
	const char	*start;

	start = *str;
	while (**str && !is_ifs_char(**str))
		(*str)++;
	result[*i] = dup_word(start, *str);
	if (!result[*i])
	{
		while (*i > 0)
			free(result[--(*i)]);
		free(result);
		return (0);
	}
	(*i)++;
	return (1);
}

/* ft_split_whitespace
**   Splits a string into words based on whitespace (IFS).
**   Returns a NULL-terminated array of strings, or NULL on allocation failure.
*/
char	**ft_split_whitespace(const char *str)
{
	char		**result;
	int			i;

	i = 0;
	if (!str)
		return (NULL);
	result = malloc((count_words(str) + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	while (*str)
	{
		while (*str && is_ifs_char(*str))
			str++;
		if (*str)
		{
			if (!process_word(&str, result, &i))
				return (NULL);
		}
	}
	result[i] = NULL;
	return (result);
}
