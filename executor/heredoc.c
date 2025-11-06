/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 15:58:45 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/06 15:57:12 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*Key Changes:
Added heredoc_child_process() function: 
	This function runs in the child process and:

Sets up child signal handling (default signal behavior via child_signal_setup())
Calls write_heredoc() to read user input
Exits with status 130 if interrupted by Ctrl+C (SIGINT)
Exits with status 0 on normal completion
Added wait_for_heredoc_child() function: This function in the parent:

Closes the write end of the pipe
Waits for the child process to complete
Detects if the child was interrupted (exit status 130 or killed by SIGINT)
Returns -1 on interruption, 0 on success
Modified apply_heredocs() function: Now it:

Forks a child process for each heredoc
The child handles user input with proper signal handling
The parent waits for the child and checks if it was interrupted
Returns -1 if Ctrl+C was pressed, allowing cleanup to occur
Modified collect_heredocs() function: Now it:

Sets up parent signal handling (ignores SIGINT) before processing heredocs
Restores normal signal handling (via init_signals()) after all heredocs are done
Properly cleans up and propagates errors
How It Works:
When a heredoc is encountered, the parent process forks
The child process reads heredoc input with default signal handling, 
	so Ctrl+C will terminate it
The parent process ignores signals while waiting, preventing interference
When Ctrl+C is pressed in the child, it exits with status 130
The parent detects this and returns an error, aborting the heredoc collection
After heredocs are done (or aborted), normal signal handling is restored
This approach preserves your existing memory management and 
	cleanup logic while adding proper Ctrl+C support!

*/
/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted,
   used to decide whether expansion is disabled in heredoc.
*/
int	delimiter_was_quoted(const char *delimiter)
{
	int	s_quote;
	int	d_quote;

	s_quote = 0;
	d_quote = 0;
	while (*delimiter)
	{
		if (*delimiter == '\'')
			s_quote++;
		if (*delimiter == '"')
			d_quote++;
		delimiter++;
	}
	if ((s_quote != 0 && (s_quote % 2 == 0))
		|| (d_quote != 0 && (d_quote % 2 == 0)))
		return (1);
	return (0);
}

/* handle_heredoc_eof:
 	Checks if readline returned NULL (EOF or Ctrl+D).
*/
static int	handle_heredoc_eof(char *line, char *trimmed_delim)
{
	if (!line)
	{
		ft_putstr_fd("warning: here-document delimited by ", STDERR_FILENO);
		ft_putstr_fd("end-of-file (wanted `", STDERR_FILENO);
		ft_putstr_fd(trimmed_delim, STDERR_FILENO);
		ft_putendl_fd("')", STDERR_FILENO);
		return (1);
	}
	return (0);
}

/* read_heredoc_line:
	Reads a line for heredoc - uses get_next_line for non-tty,
	readline for interactive.
 */
static char	*read_heredoc_line(void)
{
	char	*line;
	char	*trimmed;

	if (isatty(STDIN_FILENO))
		return (readline("> "));
	line = get_next_line(STDIN_FILENO);
	if (!line)
		return (NULL);
	trimmed = ft_strtrim(line, "\n");
	free(line);
	return (trimmed);
}

/* write_heredoc:
	Reads user input line by line until the delimiter is found
	or SIGINT occurs. Expands variables if delimiter was unquoted, 
	writes lines to pipe.
 */
int	write_heredoc(char *delim, int write_fd, int status)
{
	char	*line;
	char	*trimmed_delim;
	int		expand;

	trimmed_delim = remove_quotes(delim);
	expand = !delimiter_was_quoted(delim);
	g_sigint_received = 0;
	while (1)
	{
		line = read_heredoc_line();
		if (handle_heredoc_eof(line, trimmed_delim))
			break ;
		if (handle_sigint_in_heredoc(line, trimmed_delim))
			return (-1);
		if (is_delimiter_line(line, trimmed_delim))
		{
			free(line);
			break ;
		}
		process_and_write_line(line, expand, status, write_fd);
	}
	free(trimmed_delim);
	return (0);
}

/* heredoc_sigint_handler:
   Signal handler for heredoc child - just exits with 130.
*/
static void	heredoc_sigint_handler(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	exit(130);
}

/* heredoc_child_process:
   Handles heredoc reading in child process.
   Exits with status 0 on success, 130 on SIGINT.
*/
static void	heredoc_child_process(char *delim, int write_fd, int status)
{
	struct sigaction	sa;
	int					result;

	enable_ctrl_echo();
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = heredoc_sigint_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		error("error with sigint_handler");
	result = write_heredoc(delim, write_fd, status);
	close(write_fd);
	if (result == -1)
		exit(130);
	exit(0);
}

/* wait_for_heredoc_child:
   Waits for child process and returns -1 if interrupted (Ctrl+C),
   0 otherwise.
*/
static int	wait_for_heredoc_child(pid_t pid, int hd_pipe[2])
{
	int	status;

	close(hd_pipe[1]);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 130)
	{
		close(hd_pipe[0]);
		return (-1);
	}
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		close(hd_pipe[0]);
		return (-1);
	}
	return (0);
}

/* apply_heredocs:
 Sets up pipes for each heredoc, writes contents (forks only if interactive), 
 and stores read ends in command struct.*/
int	apply_heredocs(t_command *cmd, int status)
{
	int		hd_pipe[2];
	int		i;
	pid_t	pid;
	int		result;

	if (!cmd->heredoc_count)
		return (0);
	i = -1;
	while (++i < cmd->heredoc_count)
	{
		if (pipe(hd_pipe) == -1)
			error("pipe heredoc");
		if (isatty(STDIN_FILENO))
		{
			pid = fork();
			if (pid == -1)
				error("fork heredoc");
			if (pid == 0)
				heredoc_child_process(cmd->heredoc_delim[i], hd_pipe[1], status);
			if (wait_for_heredoc_child(pid, hd_pipe) == -1)
				return (-1);
		}
		else
		{
			result = write_heredoc(cmd->heredoc_delim[i], hd_pipe[1], status);
			close(hd_pipe[1]);
			if (result == -1)
			{
				close(hd_pipe[0]);
				return (-1);
			}
		}
		cmd->heredoc_fds[i] = hd_pipe[0];
	}
	return (0);
}

/* collect_heredocs:
  Iterates over all commands, allocates arrays, and applies heredocs;
  stops on error. Parent ignores signals while child reads heredoc.*/
int	collect_heredocs(t_command *cmds, int status)
{
	t_command	*current;
	int			result;

	parent_signal_setup();
	current = cmds;
	result = 0;
	while (current)
	{
		if (current->heredoc_count > 0)
		{
			current->heredoc_fds = handle_malloc(sizeof(int)
					* current->heredoc_count);
			if (apply_heredocs(current, status) < 0)
			{
				free(current->heredoc_fds);
				current->heredoc_fds = NULL;
				result = -1;
				break ;
			}
		}
		current = current->next;
	}
	init_signals();
	return (result);
}
