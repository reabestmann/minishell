/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/25 15:03:23 by aabelkis         ###   ########.fr       */
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

/* fd_check:
   Safely redirects one file descriptor to another.
   - Checks if 'fd' is valid (<0 = error).
   - Uses dup2(fd, std_fd) to make std_fd point to the same source as fd.
   - Closes fd after duplication to avoid leaks.
   - Handles stdin, stdout, heredocs, files, or pipes.
   - Example: fd_check(cmd->heredoc, STDIN_FILENO, "heredoc")
*/
void	fd_check(int fd, int std_fd, char *file)
{
	if (fd < 0)
		error(file);
	if (dup2(fd, std_fd) < 0)
	{
		close(fd);
		error("dup2");
	}
	close(fd);
}

/* heredoc_fd:
   Implements the << heredoc behavior.
   - Reads lines from the user until the specified delimiter is typed.
   - Writes all lines to a temporary file.
   - Returns an fd that can be used as stdin for a command.
   - Temporary file is deleted immediately after opening for reading.
   - Example usage: cmd->heredoc = heredoc_fd("EOF");
*/
/*static int	heredoc_fd(const char *delimiter)
{
	int		fd;
	int		rfd;
	char	*line;

	fd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		error("heredoc");
	while (1)
	{
		line = readline("> ");
		if (str_equals(line, delimiter))
		{
			free(line);
			break ;
		}
		ft_putstr_fd(line, fd);
		ft_putstr_fd("\n", fd);
		free(line);
	}
	close(fd);
	rfd = open(".heredoc_tmp", O_RDONLY);
	unlink(".heredoc_tmp");
	return (rfd);
}*/

char *expand_for_heredoc(char *line, int last_status)
{
    char    *result;
    int     i;

    result = ft_strdup(""); // start with empty string
    if (!result)
        return (NULL);
    i = 0;
    while (line[i])
    {
        if (line[i] == '$')
        {
            i++;
            if (line[i] == '?') // special case $?
            {
                char *num = ft_itoa(last_status);
                result = append_normal_text(num, result);
                free(num);
                i++;
            }
            else
            {
                // parse key
                int start = i;
                while (ft_isalnum(line[i]) || line[i] == '_')
                    i++;
                char *key = ft_substr(line, start, i - start);
                char *val = getenv(key);   // use system env for now
                if (!val)
                    val = "";
                result = append_normal_text(val, result);
                free(key);
            }
        }
        else
        {
            char buf[2] = { line[i], '\0' };
            result = append_normal_text(buf, result);
            i++;
        }
    }
    return (result);
}


char *process_line(char *line, int expand)
{
    char *result = line;
    if (expand)
    {
        result = expand_for_heredoc(line, -1);
        free(line);
    }
    return result;
}
int delimiter_was_quoted(const char *delimiter)
{
    int len = ft_strlen(delimiter);
    return (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'') ||
                         (delimiter[0] == '"' && delimiter[len - 1] == '"')));
}
char *get_trimmed_delimiter(const char *delimiter)
{
    int len = ft_strlen(delimiter);
    if (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'') ||
                     (delimiter[0] == '"' && delimiter[len - 1] == '"')))
        return ft_substr(delimiter, 1, len - 2);
    return ft_strdup(delimiter);
}

static int heredoc_fd(const char *delimiter)
{
    int fd;
	int rfd;
    char *line;
    char *trimmed;
    int expand;

    trimmed = get_trimmed_delimiter(delimiter);
    expand = !delimiter_was_quoted(delimiter);

    fd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        error("heredoc");

    while (1)
    {
        line = readline("> ");
        if (str_equals(line, trimmed))
        {
            free(line);
            break;
        }
        line = process_line(line, expand);
        ft_putstr_fd(line, fd);
        ft_putstr_fd("\n", fd);
        free(line);
    }
    close(fd);
    free(trimmed);
    rfd = open(".heredoc_tmp", O_RDONLY);
    unlink(".heredoc_tmp");
    return rfd;
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
void	apply_redirections(t_command *cmd)
{
	int	fd;

	if (cmd->heredoc != -1)
		fd_check(cmd->heredoc, STDIN_FILENO, "heredoc");
	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		fd_check(fd, STDIN_FILENO, cmd->infile);
	}
	if (cmd->outfile)
	{
		if (cmd->append == 1) // >>
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else if (cmd->append == 2) // >
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		fd_check(fd, STDOUT_FILENO, cmd->outfile);
	}
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
	if ((*cpy)->type == TOKEN_REDIR_IN)
	{
		if (next && next->type == TOKEN_WORD)
			cmd->infile = ft_strdup(next->val);
	}
	else if ((*cpy)->type == TOKEN_REDIR_OUT)
		set_redirection(cmd, next, 2);
	else if ((*cpy)->type == TOKEN_REDIR_APPEND)
		set_redirection(cmd, next, 1);
	else if ((*cpy)->type == TOKEN_HEREDOC)
	{
		if (next && next->type == TOKEN_WORD)
			cmd->heredoc = heredoc_fd(next->val);
	}
	if (next)
		*cpy = next;
}
