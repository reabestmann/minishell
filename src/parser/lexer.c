/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:26:43 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/13 07:15:18 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* TO DO: move 1 function to another file

typedef struct s_token
{
	char          *value;     // "echo"
	t_token_type  type;       // TOKEN_WORD
	int           in_squote;  // was it inside ' '
	int           in_dquote;  // was it inside " "
}   t_token;
*/

static char	*trim_quotes(char *str, int in_squote, int in_dquote)
{
	size_t	len;
	char	*res;

	if ((in_squote || in_dquote) && ft_strlen(str)>= 2)
	{
		len = ft_strlen(str) - 2;
		res = ft_substr(str, 1, len);
		return (res);
	}
	return (ft_strdup(str));
}

static t_token	*create_token(const char *str, t_token_type type, int in_squote, int in_dquote)
{
	t_token	*token;
	int	i;

	token = handle_malloc(sizeof(t_token));

	token->val = trim_quotes(str, in_squote, in_dquote);
	if (!token->val)
	{
		free(token);
		return (NULL);
	}
	token->type = type;
	token->in_squote = in_squote;
	token->in_dquote = in_dquote;
	token->next = NULL;

	return (token);
}

static void append_token(t_token **head, t_token *new_token)
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

static int	handle_stype(t_token **tokens, const char *str)
{
	int	len;
	char	*tmp;
	t_token_type	type;

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
	else
		type = T_PIPE;
	
	tmp = ft_substr(str, 0, len);
	append_token(tokens, create_token(tmp, type, 0, 0));
	free(tmp)
	return (len);
}

static int	handle_wtype(t_token **tokens, const char *str)
{
	int	len;
	int	in_squote;
	int	in_dquote;
	char	*tmp;

	len = -1;
	in_squote = 0;
	in_dquote = 0;

	if (str[len++] && (in_squote || in_dquote || !ft_strchr(" >|<", str[i])))
		update_quotes(str[len], &in_quote, &in_dquote)
	tmp = ft_substr(str, 0, len);
	append_token(tokens, create_token(tmp, type, in_squote, in_dquote));
	free(tmp);
	return(len);
}

t_token	*lexer(const char *input)
{
	t_token	*tokens;
	int	i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		i = skip_spaces(input, i);
		if (input[i])
			break ;
		if (ft_memchr("<>|", input[i], 3))
			i += handle_stype(&tokens, input + i);
		else
			i += handle_wtype(&tokens, input + i);
	}
	return (tokens);
}
