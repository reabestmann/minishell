/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_basics.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:45:01 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:45:27 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/* append_empty_safely
   - Safely appends an empty string to result.
   - Frees result and returns NULL on failure. */
static char	*append_empty_safely(char *result)
{
	char	*tmp;

	tmp = append_normal_text("", result);
	if (!tmp)
	{
		free(result);
		return (NULL);
	}
	return (tmp);
}

/* expand_one_arg
   - Expands a single argument starting after '$'.
   - Handles special cases like $? for last status.
   - Looks up environment variables and appends values.
   - Returns newly allocated result, or NULL on failure. */
char	*expand_one_arg(char *arg, int *i, t_env *head, int last_status)
{
	char	*result;
	char	*tmp;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	(*i)++;
	if (!arg[*i])
		return (append_normal_text("$", result));
	if (arg[*i] == '?')
	{
		result = append_result(head, NULL, result, last_status);
		(*i)++;
	}
	else
	{
		tmp = append_key_value(head, arg, i, result);
		if (!tmp)
			tmp = append_empty_safely(result);
		if (!tmp)
			return (NULL);
		result = tmp;
	}
	return (result);
}

/*handle_normal_txt:
- Copies literal characters until a special character ($, ', ")
- Uses append_normal_text*/
/* handle_normal_txt:
**   Appends literal text from arg starting at *i until a special character
**   ($, single or double quote) is found.
*/
char	*handle_normal_txt(int *i, char *arg, char *result)
{
	int		start;
	char	*sub;

	start = *i;
	while (arg[*i] && arg[*i] != '$' && arg[*i] != '"' && arg[*i] != '\'')
		(*i)++;
	sub = ft_substr(arg, start, *i - start);
	result = append_normal_text(sub, result);
	free(sub);
	return (result);
}

/* append_char
   - Appends a single character to result using append_normal_text. */
char	*append_char(char c, char *result)
{
	char	temp[2];

	temp[0] = c;
	temp[1] = '\0';
	return (append_normal_text(temp, result));
}
