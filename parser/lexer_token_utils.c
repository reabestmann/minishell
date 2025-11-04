/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:49:31 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:54:45 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
/* create_token
	Allocates and initializes a new token.
	Stores the token string (with quotes trimmed), type, and quote state.
	Returns pointer to the new t_token.
	strdup uses malloc
*/
t_token	*create_token(const char *str, t_token_type type)
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
void	append_token(t_token **head, t_token *new_token)
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

/* handle_redir_err / handle_redir_both / handle_redir_out
- Determine type and length for redirection operators (2>, &>, 1>, etc.).
*/
int	handle_redir_err(const char *str, t_token_type *type)
{
	if (str[2] == '>')
	{
		*type = TOKEN_REDIR_ERR_APPEND;
		return (3);
	}
	*type = TOKEN_REDIR_ERR;
	return (2);
}

int	handle_redir_both(const char *str, t_token_type *type)
{
	if (str[2] == '>')
	{
		*type = TOKEN_REDIR_BOTH_APPEND;
		return (3);
	}
	*type = TOKEN_REDIR_BOTH;
	return (2);
}

int	handle_redir_out(const char *str, t_token_type *type)
{
	if (str[2] == '>')
	{
		*type = TOKEN_REDIR_APPEND;
		return (3);
	}
	*type = TOKEN_REDIR_OUT;
	return (2);
}
