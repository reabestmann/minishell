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
   - Looks up value with getenv(key).
   - Appends value (or "" if not found) to *result.
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
	val = getenv(key);
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
*/
static int	handle_normal_text(int *i, char *line, char **result)
{
	char	buf[2];
	char	*temp;

	buf[0] = line[*i];
	buf[1] = '\0';
	temp = append_normal_text(buf, *result);
	if (!temp)
	{
		free(*result);
		return (1);
	}
	*result = temp;
	(*i)++;
	return (0);
}

/* init_res_i:
   Initializes heredoc expansion loop variables.
*/
static int	init_res_i(int *i, char **result)
{
	*i = 0;
	*result = ft_strdup("");
	if (!*result)
		return (0);
	return (1);
}

/* dol_q_expansion:
   Handles the special "$?" expansion inside heredocs.
   If current char is '?', replace with last_status,
   appends it to result using append_normal_text().
*/
int	dol_q_expansion(char *line, int *i, int last_status, char **result)
{
	char	*num;
	char	*temp;

	if (line[*i] == '?')
	{
		num = ft_itoa(last_status);
		if (!num)
			return (-1);
		temp = append_normal_text(num, *result);
		if (!temp)
			return (free(num), -1);
		free(*result);
		*result = temp;
		free(num);
		(*i)++;
		return (1);
	}
	return (0);
}

/* expand_for_heredoc:
   Performs variable expansion inside a heredoc line.
   - Iterates through characters in line.
   - If '$' is found:
       → try "$?" via dol_q_expansion
       → else parse variable with parse_key
   - Otherwise appends normal characters.
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
			if (handle_normal_text(&i, line, &result))
				return (NULL);
	}
	return (result);
}
