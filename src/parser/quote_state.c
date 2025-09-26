/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_state.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 20:51:11 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/26 17:02:16 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* update_state_char:
 * Updates the quote state based on a single character
 * state: 0 (none), '\'' (single quote), '"' (double quote)
*/
void	update_state_char(char c, char *state)
{
	if (c == '\'' && *state == 0)
		*state = '\'';
	else if (c == '\'' && *state == '\'')
		*state = 0;
	else if (c == '"' && *state == 0)
		*state = '"';
	else if (c == '"' && *state == '"')
		*state = 0;
}

/* set_state:
    Appends quote char if needed,
    updates state based on current char.
*/
char	*set_state(char *arg, int *i, char *result, char *state)
{
	char	c;
	char	temp[2];

	c = arg[*i];
	if (c == '\'' || c == '"')
	{
		temp[0] = c;
		temp[1] = '\0';
		result = append_normal_text(temp, result);
		update_state_char(c, state);
		(*i)++;
	}
	if (*state == '\'' && c == '\'')
		result = copy_squote(i, arg, result);
	return (result);
}

/* copy_squote:
- Copies characters inside single quotes to result
- Stops safely if closing quote is missing
*/
char	*copy_squote(int *i, char *arg, char *result)
{
	int		start;
	char	*sub;

	start = *i;
	while (arg[*i] && arg[*i] != '\'')
		(*i)++;
	sub = ft_substr(arg, start, *i - start);
	result = append_normal_text(sub, result);
	free(sub);
	return (result);
}

/* update_state:
 * Update the quote state for a whole string (token value)
 * state: pointer to current quote state (0, '\'', or '"')
 */
void	set_state_str(const char *str, char *state)
{
	int	i;

	i = 0;
	while (str[i])
	{
		update_state_char(str[i], state);
		i++;
	}
}
