/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_heredoc2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 21:33:17 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/26 21:45:40 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* parse_key:
   Parses a variable name starting at *i inside heredoc expansion.
   - Extracts alphanumeric/underscore sequence as key.
   - Looks up value with getenv(key) (later replace with lookup in t_env).
   - Appends value (or "" if not found) to *result.
   - Advances *i to the end of the key.
   - On malloc failure, frees key and leaves *result unchanged.
   - Note:
   ⚠️ Later you might replace getenv with lookup in your own t_env.
*/
static void	parse_key(int start_idx, int *i, char *line, char **result)
{
	char	*key;
	char	*val;
	int		start;
	char	*temp;

	start = start_idx;
	if (start == -1)
		start = *i;
	while (ft_isalnum(line[*i]) || line[*i] == '_')
		(*i)++;
	key = ft_substr(line, start, *i - start);
	if (!key)
		return ;
	val = getenv(key); // is out t_env available at this point?
	if (!val)
		val = "";
	temp = append_normal_text(val, *result);
	if (!temp)
	{
		free(key);
		return ;
	}
	*result = temp;
	free(key);
}

/* handle_normal_txt:
   Appends a single non-special character from line[*i] to result.
   - Wraps it in a buffer and calls append_normal_text().
   - Returns updated result, or NULL on malloc failure.
   - Caller must free result if NULL is returned.
*/
static char	*handle_normal_text(int *i, char *line, char *result)
{
	char	buf[2];
	char	*temp;

	buf[0] = line[*i];
	buf[1] = '\0';
	temp = append_normal_text(buf, result);
	if (!temp)
	{
		free(result);
		return (NULL);
	}
	return (temp);
}

/* init_res_i:
   Initializes heredoc expansion loop variables.
   - Sets index *i to 0.
   - Allocates an empty result string ("").
   - Returns 1 on success, 0 on malloc failure.
*/
static int	init_res_i(int *i, char **result)
{
	*i = 0;
	*result = ft_strdup("");
	if (!*result)
		return (0);
	return (1);
}

/*sends to handle_normal_text and makes sure reult comes back properlly*/
static int	norm_txt_res_check(int *i, char *line, char **result)
{
	*result = handle_normal_text(i, line, *result);
	if (!*result)
		return (1);
	(*i)++;
	return (0);
}

/* expand_for_heredoc:
   Performs variable expansion inside a heredoc line.
   - Iterates through characters in line.
   - If '$' is found:
       → try "$?" via dol_q_expansion
       → else parse variable with parse_key
   - Otherwise appends normal characters.
   - Returns expanded result string or NULL on error.
*/
char	*expand_for_heredoc(char *line, int last_status)
{
	char	*result;
	int		i;
	int		handled;

	if (init_res_i(&i, &result) == 0)
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			handled = dol_q_expansion(line, &i, last_status, &result);
			if (handled == -1)
				return (free(result), NULL);
			if (handled == 1)
				continue ;
			parse_key(i, &i, line, &result);
		}
		else
			if (norm_txt_res_check(&i, line, &result) == 1)
				return (NULL);
	}
	return (result);
}
