/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 13:52:46 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/03 14:23:25 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

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
		while (str[i] == ' ')
			i++;
		if (str[i])
			wc++;
		while (str[i] && (in_quote || in_dquote|| str[i] != ' '))
		{
			if (str[i] == '\'' && !in_dquote)
				in_quote = !in_quote;
			else if (str[i] == '\"' && !in_quote)
				in_dquote = !in_dquote;
			i++;
		}
	}
	return (malloc(sizeof(char *) * (wc + 1)));
}

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
		if (str[i] == '\'' && !in_dquote)
			in_quote = !in_quote;
		else if (str[i] == '\"' && !in_quote)
			in_dquote = !in_dquote;
		i++;
		len++;
	}
	return (len);
}

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
		while (str[i] == ' ')
			i++;
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
