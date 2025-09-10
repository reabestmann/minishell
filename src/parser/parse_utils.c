/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 12:00:54 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/02 17:42:15 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* trim_quotes:
 * If the token text is wrapped by matching quotes ('...' or "...")
 * remove exactly one pair; otherwise return a duplicate unchanged.
 */
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

/* handle_redirection:
   Updates infile/outfile/append fields in the current command.
   Looks at the redirection token (>, >>, <) and sets the right target file.
   Advances the token pointer to skip the filename.
*/
void	handle_redirection(t_command *current, t_token **cpy)
{
	if ((*cpy)->type == TOKEN_REDIR_IN)
    {
        *cpy = (*cpy)->next;
    	if (*cpy && (*cpy)->type == TOKEN_WORD)
           	current->infile = ft_strdup((*cpy)->val);
    }
    else if ((*cpy)->type == TOKEN_REDIR_OUT)
    {
        *cpy = (*cpy)->next;
        if (*cpy && (*cpy)->type == TOKEN_WORD)
        {
			current->outfile = ft_strdup((*cpy)->val);
            current->append = 2; // >
        }
    }
    else if ((*cpy)->type == TOKEN_REDIR_APPEND)
	{
        *cpy = (*cpy)->next;
        if (*cpy && (*cpy)->type == TOKEN_WORD)
        {
            current->outfile = ft_strdup((*cpy)->val);
            current->append = 1; // >>
        }
    }
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
		str_equals(cmd->args[0], "unset"))
		cmd->modifies_shell = 1;
	else
		cmd->modifies_shell = 0;
}