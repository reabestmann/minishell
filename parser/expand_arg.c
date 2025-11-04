/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_arg.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:44:35 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:54:13 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/* append_quote
   - Append a quote char to result and toggle quote state.
     Handles opening/closing single or double quotes. */
static char	*append_quote(char c, char *result, char *state)
{
	char	temp[2];

	if (*state == 0)
		*state = c;
	else if (*state == c)
		*state = 0;
	temp[0] = c;
	temp[1] = '\0';
	return (append_normal_text(temp, result));
}

/* handle_dollar
   - Append expanded env variable to result.
     Marks had_unquoted if spaces are present and quote state allows.
     Frees the temporary expansion string. */
static char	*handle_dollar(char **temp, char state,
	int *had_unquoted, char *result)
{
	if (!*temp)
	{
		free(result);
		return (NULL);
	}
	if (state == 0 && had_unquoted && (ft_strchr(*temp, ' ')
			|| ft_strchr(*temp, '\t') || ft_strchr(*temp, '\n')))
		*had_unquoted = 1;
	result = append_normal_text(*temp, result);
	free(*temp);
	return (result);
}

/* init_expand_vars
   - Initialize index, quote state, and result string for expansion.
     Returns 1 on success, 0 on allocation failure. */
int	init_expand_vars(	int	*i, char *state, char	**result)
{
	*i = 0;
	*state = 0;
	*result = ft_strdup("");
	if (!*result)
		return (0);
	return (1);
}

/* expand_arg_keep_quotes
   - Expand env variables in the argument while preserving quotes.
     Supports single and double quotes, $ expansions, and tracks
     unquoted spaces if had_unquoted pointer is provided. */
char	*expand_arg_keep_quotes(char *arg, t_env *head,
	int last_status, int *had_unquoted)
{
	int		i;
	char	state;
	char	*result;
	char	*temp;

	if (!init_expand_vars(&i, &state, &result))
		return (NULL);
	if (had_unquoted)
		*had_unquoted = 0;
	while (arg[i])
	{
		if (arg[i] == '\'' || arg[i] == '"')
			result = append_quote(arg[i++], result, &state);
		else if (arg[i] == '$' && state != '\'')
		{
			temp = expand_one_arg(arg, &i, head, last_status);
			result = handle_dollar(&temp, state, had_unquoted, result);
			if (!result)
				return (NULL);
		}
		else
			result = append_char(arg[i++], result);
	}
	return (result);
}
