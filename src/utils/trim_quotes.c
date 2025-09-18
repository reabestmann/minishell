/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 15:46:22 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/18 16:21:47 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* trim_quotes:
 * If the token text is wrapped by matching quotes ('...' or "...")
 * remove exactly one pair; otherwise return a duplicate unchanged.
 */
char	*trim_quotes(const char *str)
{
	size_t	len;

	len = ft_strlen(str);
	if (len >= 2)
	{
		if (str[0] == '\'' && str[len - 1] == '\'')
			return (ft_substr(str, 1, len - 2));
		if (str[0] == '"' && str[len - 1] == '"')
			return (ft_substr(str, 1, len - 2));
	}
	return (ft_strdup((char *)str));
}

void	trim_quotes_for_execution(char **args)
{
	int i;
	char *temp;

	i = 0;
	if(!args)
		return;

	while (args[i])
	{
		temp = trim_quotes(args[i]);
		free(args[i]);
		args[i] = temp;
		i++;
	}
}
