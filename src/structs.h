/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:17:57 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/12 15:19:10 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTS_H
# define STRUCTS_H


typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,			// |
	TOKEN_REDIR_IN,			// <
	TOKEN_REDIR_OUT,		// >
	TOKEN_REDIR_APPEND,		// >>
	TOKEN_HEREDOC			// <<
}	t_token_type;

typedef struct s_token
{
	char          *val;     // "echo"
	t_token_type  type;       // TOKEN_WORD
	int           in_squote;  // was it inside ' '
	int           in_dquote;  // was it inside " "
	struct s_token	*next;	// next member of the linked list
}   t_token;

typedef struct s_command
{
	char	**args;			// Command and its arguments
	char	*infile;		// Redirection input file
	char	*outfile;		// Redirection output file
	int	append;			// 1 to append (>>), 2 to overwrite (>)
	int	modifies_shell;		// 1 if cmd modifies envp, 0 if not
	int	in_child;		// 1 if cmd is already running in a child process
	struct s_command	*next;  // Next command in pipeline
} t_command;

typedef struct s_env
{
    char            *key;    // the variable name, e.g., "PWD"
    char            *value;  // the variable value, e.g., "/home/alexandra"
    struct s_env    *next;   // pointer to the next env variable
	int exported;
}   t_env;

#endif
