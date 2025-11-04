/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/11/04 15:02:35 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*expand_arg_keep_quotes_simple(char *arg, t_env *head, int last_status)
{
	int		i;
	char	state;
	char	*result;
	char	*tmp;

	i = 0;
	state = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (arg[i])
	{
		result = set_state(arg, &i, result, &state);
		if (arg[i] == '$' && state != '\'')
		{
			tmp = expand_one_arg(arg, &i, head, last_status);
			if (!tmp)
				return (free(result), NULL);
			result = append_normal_text(tmp, result);
			free(tmp);
		}
		else
			result = handle_normal_txt(&i, arg, result);
	}
	return (result);
}
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

/* helper: reads all content from src_fd and writes it to dest_fd */
static void	merge_fd_into_pipe(int src_fd, int dest_fd)
{
	char	*line;

	line = get_next_line(src_fd);
	while (line)
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
- If outfile/errfile exists:
	append == 1 → >> append mode
	append == 2 → > truncate/overwrite mode
	dup2 to STDOUT
*/
void	apply_redirections(t_command *cmd, t_env **env, int last_status)
{
	char	*expanded;

	if (cmd->heredoc_count)
		handle_heredocs(cmd);
	if (cmd->infile)
		handle_infile(&cmd->infile);
	if (cmd->outfile)
	{
		expanded = expand_arg_keep_quotes_simple(cmd->outfile,
				*env, last_status);
		free(cmd->outfile);
		cmd->outfile = remove_quotes(expanded);
		free(expanded);
		handle_redir_file(cmd->outfile, cmd->append, cmd->fd_type);
	}
	if (cmd->errfile)
	{
		expanded = expand_arg_keep_quotes_simple(cmd->errfile,
				*env, last_status);
		free(cmd->errfile);
		cmd->errfile = remove_quotes(expanded);
		free(expanded);
		handle_redir_file(cmd->errfile, cmd->append_err, cmd->fd_type);
	}
}

void	parse_redirection(t_command *cmd, t_token **cpy)
{
	t_token	*next;
	int		append_type;

	if (!*cpy)
		return ;
	next = (*cpy)->next;
	append_type = 2;
	if (is_append_type((*cpy)->type))
		append_type = 1;
	set_fd_type(cmd, *cpy);
	if ((*cpy)->type == TOKEN_REDIR_IN)
		handle_input_redir(cmd, next);
	else if (is_output_redir((*cpy)->type))
		set_redirection(cmd, next, append_type);
	else if ((*cpy)->type == TOKEN_HEREDOC)
	{
		if (next && next->type == TOKEN_WORD)
			add_heredoc(cmd, next->val);
	}
	if (next)
		*cpy = next;
}
