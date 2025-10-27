/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:26:43 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/23 22:04:38 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* 
typedef struct s_token
{
	char          *value;     // "echo"
	t_token_type  type;       // TOKEN_WORD
	int           in_squote;  // was it inside ' '
	int           in_dquote;  // was it inside " "
}   t_token;
*/

/* create_token
	Allocates and initializes a new token.
	Stores the token string (with quotes trimmed), type, and quote state.
	Returns pointer to the new t_token.
	strdup uses malloc
*/
static t_token	*create_token(const char *str, t_token_type type)
{
	t_token	*token;

	token = handle_malloc(sizeof(t_token));
	token->val = ft_strdup((char *)str);
	if (!token->val)
	{
		free(token);
		return (NULL);
	}
	token->type = type;
	token->next = NULL;
	return (token);
}

/* append_token
	Adds a token to the end of a token linked list.
	If the list is empty, the new token becomes the head.
*/
static void	append_token(t_token **head, t_token *new_token)
{
	t_token	*tmp;

	if (!*head)
		*head = new_token;
	else
	{
		tmp = *head;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_token;
	}
}

/* handle_stype
	Handles special operator tokens (>, >>, <, <<, |).
	Determines the correct token type and length.
	Creates and appends the new token to the token list.
	Returns number of characters consumed (1 or 2)

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_ERR,
	TOKEN_REDIR_ERR_APPEND,
	TOKEN_REDIR_BOTH,
	TOKEN_REDIR_BOTH_APPEND,
	TOKEN_HEREDOC
}	t_token_type;

*/

static int	handle_else(const char *str, t_token_type	*type)
{
	if (str[0] == '2' && str[1] == '>')
	{
		if (str[2] == '>')
		{
			*type = TOKEN_REDIR_ERR_APPEND;
			return (3);
		}
		*type = TOKEN_REDIR_ERR;
		return (2);
	}
	else if (str[0] == '&' && str[1] == '>')
	{
		if (str[2] == '>')
		{
			*type = TOKEN_REDIR_BOTH_APPEND;
			return (3);
		}
		*type = TOKEN_REDIR_BOTH;
		return (2);
	}
	else if (str[0] == '1' && str[1] == '>')
	{
		if (str[2] == '>')
		{
			*type = TOKEN_REDIR_APPEND;
			return (3);
		}
		*type = TOKEN_REDIR_OUT;
		return (2);
	}
	return (1);
}

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
