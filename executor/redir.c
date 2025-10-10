/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/29 11:20:42 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*    Execution flow:
     1. parse_redirection() sets infile, outfile, append, heredoc
     2. heredoc_fd() reads user input → returns fd
     3. apply_redirections() dup2s FDs based on priority:
        stdin  ← heredoc / infile / pipe read
        stdout → outfile / pipe write
     4. fd_check() safely duplicates and closes original fd.

	 Notes:
     - Heredoc overrides infile if both exist
     - FDs are closed after dup2 to prevent leaks
     - Works with standalone commands, pipelines, heredocs, and redirections
*/

/* helper functions of apply, redirections */
static void	handle_infile(char **filename)
{
	char	*clean;
	int		fd;

	clean = remove_quotes(*filename);
	free(*filename);
	*filename = clean;
	fd = open(*filename, O_RDONLY);
	fd_check(fd, STDIN_FILENO, *filename);
}

// append -> 1 = >>, 2 = > 
static void	handle_outfile(t_command *cmd, t_env **env, int last_status)
{
	char	*clean;
	char	*file;
	int		fd;
	
	file = expand_arg_keep_quotes(cmd->outfile, *env, last_status);
	free(cmd->outfile);
	clean = remove_quotes(file);
	file = clean;
	fd = -1;
	if (cmd->append == 1)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0664);
	else if (cmd->append == 2)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	fd_check(fd, cmd->fd_type, file);
}

/* helper: reads all content from src_fd and writes it to dest_fd
static void	merge_fd_into_pipe(int src_fd, int dest_fd)
{
	char	buffer[1024];
	ssize_t	n;

	while ((n = read(src_fd, buffer, sizeof(buffer))) > 0)
		write(dest_fd, buffer, n);
	close(src_fd);
}*/

static void	merge_fd_into_pipe(int src_fd, int dest_fd)
{
	char	*line;

	line = get_next_line(src_fd);
	while(line)
	{
		write(dest_fd, line, ft_strlen(line));
		free(line);
		line = get_next_line(src_fd);
	}
	close(src_fd);
}

// call this from apply_redirections() before other redirections
void	handle_heredocs(t_command *cmd)
{
	int	merged_pipe[2];
	int	i;

	if (cmd->heredoc_count == 1)
	{
		fd_check(cmd->heredoc_fds[0], STDIN_FILENO, "heredoc");
		return ;
	}
	if (pipe(merged_pipe) == -1)
		error("pipe merged heredoc");
	i = -1;
	while (++i < cmd->heredoc_count)
		merge_fd_into_pipe(cmd->heredoc_fds[i], merged_pipe[1]);
	close(merged_pipe[1]);
	fd_check(merged_pipe[0], STDIN_FILENO, "heredoc");
}

/* apply_redirections:
Sets up all input/output redirections for a command before execution.
Input priority: heredoc > infile > pipe from previous command
Output priority: outfile > pipe to next command
- If heredoc exists (cmd->heredoc != -1), dup2 it to STDIN
- If infile exists, open it and dup2 to STDIN
- If outfile exists:
	append == 1 → >> append mode
	append == 2 → > truncate/overwrite mode
	dup2 to STDOUT
*/
void	apply_redirections(t_command *cmd, t_env **env, int last_status)
{
	if (cmd->heredoc_count)
		handle_heredocs(cmd);
	if (cmd->infile)
		handle_infile(&cmd->infile);
	if (cmd->outfile && !cmd->next)
		handle_outfile(cmd, env, last_status);
}

/* set_redirection:
Helper for parse_redirection.
Sets cmd->outfile and cmd->append type based on the token.
*/
static void	set_redirection(t_command *cmd, t_token *token, int append_type)
{
	if (token && token->type == TOKEN_WORD)
	{
		cmd->outfile = ft_strdup(token->val);
		cmd->append = append_type;
	}
}

/* set_fd_type:
Helper for parse_redirection.
Sets the file descriptor we want to write to.
(std_out, std_err or both)
*/
static void	set_fd_type(t_command *cmd, t_token *cpy)
{
	cmd->fd_type = STDOUT_FILENO;
	if (cpy->type == TOKEN_REDIR_ERR || cpy->type == TOKEN_REDIR_ERR_APPEND)
		cmd->fd_type = STDERR_FILENO;
	if (cpy->type == TOKEN_REDIR_BOTH || cpy->type == TOKEN_REDIR_BOTH_APPEND)
		cmd->fd_type = 3;
}
/* parse_redirection:
Parses a token and updates the command struct with redirection info.
- < : input file → cmd->infile
- > : output truncate → cmd->outfile, append=2
- >>: output append → cmd->outfile, append=1
- <<: heredoc → cmd->heredoc = heredoc_fd(delimiter)
- Advances token pointer to the filename/delimiter
*/
void	parse_redirection(t_command *cmd, t_token **cpy)
{
	t_token	*next;

	if (!*cpy)
		return ;
	next = (*cpy)->next;
	set_fd_type(cmd, *cpy);
	if ((*cpy)->type == TOKEN_REDIR_IN)
	{
		if (next && next->type == TOKEN_WORD)
			cmd->infile = ft_strdup(next->val);
	}
	else if ((*cpy)->type == TOKEN_REDIR_OUT || (*cpy)->type == TOKEN_REDIR_ERR || (*cpy)->type == TOKEN_REDIR_BOTH)
		set_redirection(cmd, next, 2);
	else if ((*cpy)->type == TOKEN_REDIR_APPEND || (*cpy)->type == TOKEN_REDIR_ERR_APPEND || (*cpy)->type == TOKEN_REDIR_BOTH_APPEND)
		set_redirection(cmd, next, 1);
	else if ((*cpy)->type == TOKEN_HEREDOC)
	{
		if (next && next->type == TOKEN_WORD)
			add_heredoc(cmd, next->val);
	}
	if (next)
		*cpy = next;
}
