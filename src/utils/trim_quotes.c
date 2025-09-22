/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim_quotes.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 15:46:22 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/19 21:06:03 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* trim_quotes:
 * If the token text is wrapped by matching quotes ('...' or "...")
 * remove exactly one pair; otherwise return a duplicate unchanged.
 
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

*/

char *strip_quotes(const char *token)
{
    int i = 0, j = 0;
    char state = 0;
    char *clean = malloc(strlen(token) + 1);
    if (!clean) return NULL;

    while (token[i]) 
	{
        if (state == 0)
		{
            if (token[i] == '\'') state = '\'';
            else if (token[i] == '"') state = '"';
            else clean[j++] = token[i];
        } 
		else if (state == '\'') 
		{
            if (token[i] == '\'') state = 0;
            else clean[j++] = token[i];
			//dont send to expander
        } 
		else if (state == '"') 
		{
            if (token[i] == '"') state = 0;
            else clean[j++] = token[i];
        }
        i++;
    }
	if (state != 0) 
	  {
		printf(">");
        // unclosed quote detected
        free(clean);
        return NULL; //let main know to do >
	  }
    clean[j] = '\0';
    return clean;
}

void trim_quotes_for_execution(char **tokens) 
{
    for (int i = 0; tokens[i]; i++) {
        char *cleaned = strip_quotes(tokens[i]);
        free(tokens[i]);
        tokens[i] = cleaned;
    }
}