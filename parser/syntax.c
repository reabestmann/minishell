/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 18:06:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/18 12:09:24 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* ================= Helpers ================= */

// Determine if token type is a redirection
static int is_redirection_token(int type)
{
    return (type == TOKEN_REDIR_OUT || type == TOKEN_REDIR_IN
         || type == TOKEN_REDIR_APPEND || type == TOKEN_REDIR_ERR
         || type == TOKEN_REDIR_ERR_APPEND || type == TOKEN_REDIR_BOTH
         || type == TOKEN_REDIR_BOTH_APPEND || type == TOKEN_HEREDOC);
}

// Create the file and immediately close it
static void create_empty_file(const char *file, int append_mode)
{
    int fd;

    if (!file)
        return;

    if (append_mode == 1)
        fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0664);
    else
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd >= 0)
        close(fd);
}

// Return 1 for append, 2 for truncate
static int get_append_mode(t_token_type type)
{
    if (type == TOKEN_REDIR_APPEND
        || type == TOKEN_REDIR_ERR_APPEND
        || type == TOKEN_REDIR_BOTH_APPEND)
        return 1; // append
    return 2;     // truncate/create
}

/* ================= Syntax Checks ================= */

// Check pipe syntax
static int check_pipe_syntax(t_token *token)
{
    if (!token || token->type != TOKEN_PIPE)
        return 0;

    if (!token->next || token->next->type == TOKEN_PIPE)
    {
        ft_putstr_fd("minishell: parse error near pipe.\n", 2);
        return -1;
    }
    return 0;
}

// Check redirection syntax
static int check_redirection_syntax(t_token *token)
{
    int append_mode;

    if (!token->next)
    {
        ft_putstr_fd("minishell: parse error near redirection\n", 2);
        return -1;
    }

    if (token->next->type == TOKEN_PIPE || is_redirection_token(token->next->type))
    {
        ft_putstr_fd("minishell: parse error near redirection\n", 2);
        return -1;
    }
    append_mode = get_append_mode(token->type);
    create_empty_file(token->next->val, append_mode);

    return 0;
}

// Check if the first token is invalid (pipe at start)
static int check_start(t_token *tokens)
{
    if (tokens && tokens->type != TOKEN_WORD)
    {
        if (tokens->type == TOKEN_PIPE)
        {
            ft_putstr_fd("minishell: parse error near start of command\n", 2);
            return -1;
        }
    }
    return 0;
}

/* ================= Main Syntax Validator ================= */

int syntax_valid(t_token *tokens)
{
    t_token	*cpy;
    char	state;

    if (!tokens)
        return 0;
	state = 0;
    if (check_start(tokens) == -1)
        return 2;
	cpy = tokens;
    while (cpy)
    {
        set_state_str(cpy->val, &state);
        if (cpy->type != TOKEN_WORD && state == 0)
        {
            if (check_pipe_syntax(cpy) == -1)
                return 2;
            if (is_redirection_token(cpy->type))
            {
                if (check_redirection_syntax(cpy) == -1)
                    return 2;
            }
        }
        cpy = cpy->next;
    }
    return 0;
}
