/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:00:54 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/19 12:01:17 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "..minishell.h"

/* trim_quotes:
 * If the token text is wrapped by matching quotes ('...' or "...")
 * remove exactly one pair; otherwise return a duplicate unchanged.
 */
char	*trim_quotes(char *str, int in_squote, int in_dquote)
{
	size_t	len;

	len = ft_strlen(str);
	if (len >= 2)
	{
		if (in_squote && str[0] == '\'' && str[len - 1] == '\'')
			return (ft_substr(str, 1, len - 2));
		if (in_dquote && str[0] == '"' && str[len - 1] == '"')
			return (ft_substr(str, 1, len - 2));
	}
	return (ft_strdup(str));
}

/*
 * skip_space
 * helper function to avoid repetition across split functions -
 * skips whitespaces starting from index i in str.
 * returns first index after all spaces.
 */
int	skip_spaces(char *str, int i)
{
	while (str[i] == ' ')
		i++;
	return (i);
}

/* 
 * update_quotes:
 * helper function to avoid repetition across split functions -
 * keeps track wether input is within simple or double quotes
 */
void	update_quotes(char c, int *in_quote, int *in_dquote)
{
	if (c == '\'' && !(*in_dquote))
		*in_quote = !(*in_quote);
	else if (c == '\"' && !(*in_quote))
		*in_dquote = !(*in_dquote);
}