/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:17:57 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/23 21:29:57 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H

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

typedef struct s_token
{
	char			*val;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_command
{
	char				**args;
	char				*infile;
	char				*outfile;
	char				*errfile;
	char				**heredoc_delim;
	int					heredoc_count;
	int					*heredoc_fds;
	int					append;
	int					append_err;
	int					modifies_shell;
	int					in_child;
	int					fd_type;
	struct s_command	*next;
}	t_command;

typedef struct s_env
{
	char			*key;
	char			*value;
	struct s_env	*next;
	int				exported;
}	t_env;

typedef struct s_split_replace
{
	char	**old_args;
	int		old_len;
	int		parts_len;
	int		new_len;
	char	**new_args;
	int		i;
	int		dst;
}	t_split_replace;

#endif
