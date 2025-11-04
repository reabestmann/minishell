/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:26:43 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 13:54:35 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* handle_else
- Checks for special prefixed redirections (2>, &>, 1>) and delegates.
*/
static int	handle_else(const char *str, t_token_type *type)
{
	if (str[0] == '2' && str[1] == '>')
		return (handle_redir_err(str, type));
	else if (str[0] == '&' && str[1] == '>')
		return (handle_redir_both(str, type));
	else if (str[0] == '1' && str[1] == '>')
		return (handle_redir_out(str, type));
	return (1);
}

/* handle_stype
- Handles all operator tokens (|, <, >, <<, >>, 2>, etc.).
- Creates and appends a token for the operator.
- Returns length of operator in input string.
*/
static int	handle_stype(t_token **tokens, const char *str)
{
	int				len;
	char			*tmp;
	t_token_type	type;

	type = TOKEN_PIPE;
	len = handle_else(str, &type);
	if (str[0] == '>' && str[1] == '>')
	{
		type = TOKEN_REDIR_APPEND;
		len = 2;
	}
	else if (str[0] == '<' && str[1] == '<')
	{
		type = TOKEN_HEREDOC;
		len = 2;
	}
	else if (str[0] == '>')
		type = TOKEN_REDIR_OUT;
	else if (str[0] == '<')
		type = TOKEN_REDIR_IN;
	tmp = ft_substr(str, 0, len);
	append_token(tokens, create_token(tmp, type));
	free(tmp);
	return (len);
}

/* handle_wtype
- Handles normal word tokens, respecting quotes.
- Stops at whitespace or operator.
- Returns length of the word consumed.
*/
static int	handle_wtype(t_token **tokens, const char *str)
{
	int		len;
	int		in_squote;
	int		in_dquote;
	char	*tmp;

	len = 0;
	in_squote = 0;
	in_dquote = 0;
	while (str[len])
	{
		if (!in_squote && !in_dquote && ft_strchr(" \t<>|\n", str[len]))
			break ;
		if (str[len] == '\'' && !in_dquote)
			in_squote = !in_squote;
		else if (str[len] == '\"' && !in_squote)
			in_dquote = !in_dquote;
		len++;
	}
	tmp = ft_substr(str, 0, len);
	append_token(tokens, create_token(tmp, TOKEN_WORD));
	free(tmp);
	return (len);
}

/*	lexer
	Converts the raw input string into a linked list of tokens.
	-Skips whitespace
	- Splits operators (|, <, >, <<, >>)
	- Splits words (respecting quotes)
	Returns pointer to the head of the list.
*/
t_token	*lexer(const char *input)
{
	t_token	*tokens;
	int		i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		i = skip_spaces(input, i);
		if (!input[i])
			break ;
		if ((input[i] == '1' || input[i] == '2' || input[i] == '&')
			&& input[i + 1] == '>')
			i += handle_stype(&tokens, input + i);
		else if (ft_strchr("<>|", input[i]))
			i += handle_stype(&tokens, input + i);
		else
			i += handle_wtype(&tokens, input + i);
	}
	return (tokens);
}
