/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/25 21:33:44 by aabelkis         ###   ########.fr       */
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

/* dol_q_expansion:
   Handles the special "$?" expansion inside heredocs.
   - If current char is '?', replace with last_status converted to string.
   - Appends it to result using append_normal_text().
   - Advances index past '?'.
   - Returns:
       1 → expansion handled
      -1 → malloc failure
       0 → not a "$?" expansion
*/
int dol_q_expansion(char *line, int *i, int last_status, char **result)
{
	char	*num;
	char	*temp;

	if (line[*i] == '?') // special case $?
	{
		num = ft_itoa(last_status);
		if (!num)
			return (-1);
		temp = append_normal_text(num, *result);
		if (!temp)
			return (free(num), -1);
		free(*result);
		*result = temp;
		free(num);
		(*i)++;
		return (1);
	}
	return (0);
}

/* parse_key:
   Parses a variable name starting at *i inside heredoc expansion.
   - Extracts alphanumeric/underscore sequence as key.
   - Looks up value with getenv(key) (later replace with lookup in t_env).
   - Appends value (or "" if not found) to *result.
   - Advances *i to the end of the key.
   - On malloc failure, frees key and leaves *result unchanged.
   - Note:
   ⚠️ Later you might replace getenv with lookup in your own t_env.
*/
void	parse_key(int start_idx, int *i, char *line, char **result)
{
	char	*key;
	char	*val;
	int		start;
	char	*temp;

	start = start_idx;
	if (start == -1)
		start = *i;
	while (ft_isalnum(line[*i]) || line[*i] == '_')
		(*i)++;
	key = ft_substr(line, start, *i - start);
	if (!key)
		return ;
	val = getenv(key); // is out t_env available at this point?
	if (!val)
		val = "";
	temp = append_normal_text(val, *result);
	if (!temp)
	{
		free(key);
		return ;
	}
	free(*result);
	*result = temp;
	free(key);
}

/* handle_normal_txt:
   Appends a single non-special character from line[*i] to result.
   - Wraps it in a buffer and calls append_normal_text().
   - Returns updated result, or NULL on malloc failure.
   - Caller must free result if NULL is returned.
*/
char	*handle_normal_text(int *i, char *line, char *result)
{
	char	buf[2];
	char	*temp;

	buf[0] = line[*i];
	buf[1] = '\0';
	temp = append_normal_text(buf, result);
	if (!temp)
	{
		free(result);
		return (NULL);
	}
	return (temp);
}

/* init_res_i:
   Initializes heredoc expansion loop variables.
   - Sets index *i to 0.
   - Allocates an empty result string ("").
   - Returns 1 on success, 0 on malloc failure.
*/
int	init_res_i(int *i, char **result)
{
	*i = 0;
	*result = ft_strdup(""); // start with empty string
	if (!*result)
		return (0);
	return (1);
}

/* expand_for_heredoc:
   Performs variable expansion inside a heredoc line.
   - Iterates through characters in line.
   - If '$' is found:
       → try "$?" via dol_q_expansion
       → else parse variable with parse_key
   - Otherwise appends normal characters.
   - Returns expanded result string or NULL on error.
*/
char	*expand_for_heredoc(char *line, int last_status)
{
	char	*result;
	int		i;
	int		handled;

	if (init_res_i(&i, &result) == 0)
		return (NULL);
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			handled = dol_q_expansion(line, &i, last_status, &result);
			if (handled == -1)
				return (free(result), NULL);
			if (handled == 1)
				continue ;
			parse_key(i, &i, line, &result);
		}
		else
		{
			result = handle_normal_text(&i, line, result);
			i++;
		}
	}
	return (result);
}

/* process_line:
   Prepares a line before writing it to heredoc file.
   - If expand == 1, performs variable expansion.
   - If expand == 0, returns line unchanged.
   - Always frees original line if expansion is done.
*/
char	*process_line(char *line, int expand)
{
	char	*result;

	result = line;
	if (expand)
	{
		result = expand_for_heredoc(line, -1);//how can I actally get last_status here?
		free(line);
	}
	return (result);
}

/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted.
   - Returns 1 if delimiter starts/ends with matching quotes.
   - Used to decide whether expansion is disabled in heredoc.
*/
int	delimiter_was_quoted(const char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	return (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')));
}

/* get_trimmed_delimiter:
   Removes surrounding quotes from heredoc delimiter if present.
   - Returns newly allocated string with quotes stripped.
   - If not quoted, returns a strdup of original delimiter.
*/
char	*get_trimmed_delimiter(const char *delimiter)
{
	int	len;

	len = ft_strlen(delimiter);
	if (len >= 2 && ((delimiter[0] == '\'' && delimiter[len - 1] == '\'')
			|| (delimiter[0] == '"' && delimiter[len - 1] == '"')))
		return (ft_substr(delimiter, 1, len - 2));
	return (ft_strdup(delimiter));
}

/* run_heredoc:
   Reads one line of heredoc input.
   - Prompts with "> ".
   - If line matches trimmed delimiter → return 1 (stop).
   - Otherwise, expand (if expand==1), write line to fd, and continue.
   - Returns 0 if more lines expected.
*/
int	run_heredoc(char *trimmed, int expand, int fd)
{
	char	*line;

	line = readline("> ");
	if (str_equals(line, trimmed))
	{
		free(line);
		return (1);
	}
	line = process_line(line, expand);
	ft_putstr_fd(line, fd);
	ft_putstr_fd("\n", fd);
	free(line);
	return (0);
}

/* heredoc_fd:
   Full heredoc implementation for << operator.
   - Trims delimiter (removes quotes if present).
   - Determines if expansion is enabled (no quotes).
   - Reads lines until delimiter is reached.
   - Writes to temporary ".heredoc_tmp" file.
   - Returns fd opened for reading the collected heredoc text.
*/
static int	heredoc_fd(const char *delimiter)
{
	int		fd;
	int		rfd;
	char	*trimmed;
	int		expand;

	trimmed = get_trimmed_delimiter(delimiter);
	expand = !delimiter_was_quoted(delimiter);
	fd = open(".heredoc_tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		error("heredoc");
	while (1)
	{
		if (run_heredoc(trimmed, expand, fd) == 1)
			break ;
	}
	close(fd);
	free(trimmed);
	rfd = open(".heredoc_tmp", O_RDONLY);
	unlink(".heredoc_tmp");
	return (rfd);
}

// helper functions of apply, redirections
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

static void	handle_outfile(char **filename, int append)
{
	char	*clean;
	int		fd;

	clean = remove_quotes(*filename);
	free(*filename);
	*filename = clean;
	fd = 0;
	if (append == 1)
		fd = open(*filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
	else if (append == 2)
		fd = open(*filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	fd_check(fd, STDOUT_FILENO, *filename);
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
	if (cmd->heredoc != -1)
		fd_check(cmd->heredoc, STDIN_FILENO, "heredoc");
	if (cmd->infile)
		handle_infile(&cmd->infile);
	if (cmd->outfile)
		handle_outfile(&cmd->outfile, cmd->append);
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
