/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 14:17:57 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/02 17:35:16 by rbestman         ###   ########.fr       */
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
	char **args;           // Command and its arguments
	char *infile;      // Redirection input file
	char *outfile;     // Redirection output file
	int append;     // Whether to append (>>) or overwrite (>)
	struct s_command *next;  // Next command in pipeline
} t_command;

typedef enum 
{
	NO_PIPE,
	PIPE_IN,
	PIPE_OUT,
	PIPE_BOTH
} t_pipe_mode;

#endif
