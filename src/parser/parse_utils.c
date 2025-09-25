/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:00:54 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/23 19:23:13 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int has_dollar(char **args)
{
    int i = 0;
    while (args[i])
    {
        if (ft_strchr(args[i], '$'))
            return (1);
        i++;
    }
    return (0);
}
/*
 * skip_space
 * helper function to avoid repetition across split functions -
 * skips whitespaces starting from index i in str.
 * returns first index after all spaces.
 */
int	skip_spaces(const char *str, int i)
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

/* set_cmd_flags:
	sets the modifies_shell flag to 1 if the command is cd, export or unset.
	Used later to decide wether it must run in the parent instead of a child proccess
*/
void	set_cmd_flags(t_command *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (str_equals(cmd->args[0], "cd") || 
		str_equals(cmd->args[0], "export") ||
		str_equals(cmd->args[0], "unset") ||
		str_equals(cmd->args[0], "exit")) 
		cmd->modifies_shell = 1;
	else
		cmd->modifies_shell = 0;
}

// Update the quote state for a whole string (token value)
// state: pointer to current quote state (0, '\'', or '"')
void update_state(const char *str, char *state)
{
    int i = 0;

    while (str[i])
    {
        if (str[i] == '\'' && *state == 0)
            *state = '\'';       // open single quote
        else if (str[i] == '\'' && *state == '\'')
            *state = 0;          // close single quote
        else if (str[i] == '"' && *state == 0)
            *state = '"';        // open double quote
        else if (str[i] == '"' && *state == '"')
            *state = 0;          // close double quote
        // single quotes inside double quotes or vice versa are literal
        i++;
    }
}

/* trim_quotes:
 * If the token text is wrapped by matching quotes ('...' or "...")
 * remove exactly one pair; otherwise return a duplicate unchanged.
 
char	*trim_quotes(const char *str, int in_squote, int in_dquote)
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
	return (ft_strdup((char *)str));
}
*/