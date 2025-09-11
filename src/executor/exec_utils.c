/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 16:02:47 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/11 12:51:56 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* str_equals: 
	compares a string to a target for exact equality.
	Uses ft_strncmp and also checks that str has no extra characters.
	Returns 1 if equal, otherwise 0.
*/
int	str_equals(const char *str, const char *target)
{
	size_t	len;

	if (!str || !target)
		return (0);
	len = 0;
	while (target[len])
		len++;
    if (ft_strncmp(str, target, len) == 0 && str[len] == '\0')
        return (1);
    return (0);
}

/* run_builtin: 
	checks if 'cmd' matches any builtin name.
	Calls str_equals for each of the 7 builtin commands.
	Calls the function directly and returns its result.
	-> 0 = success, >0 = Error.
	returns -1 if command is not a builtin.
*/
int run_builtin(t_command *cmd, t_env **env)
{
    if (str_equals(cmd->args[0], "echo")) 
		return (echo_cmd(cmd));
    if (str_equals(cmd->args[0], "cd"))
		return (cd_cmd(cmd, env));
    if (str_equals(cmd->args[0], "pwd"))
		return (pwd_cmd(env));
    if (str_equals(cmd->args[0], "export"))
		return (export_cmd(cmd, env));
    if (str_equals(cmd->args[0], "unset"))
		return (unset_cmd(cmd, env));
    if (str_equals(cmd->args[0], "env"))
		return (env_cmd(env));
    if (str_equals(cmd->args[0], "exit"))
		return (exit_cmd(cmd));
    return (-1);
}

/* fd_check:
	Checks if 'fd' is valid.
	Duplicates it to the given standart fd (stdin/stdout).
	Closes the orinigal fd and handles errors.
*/
static void	fd_check(int fd, int std_fd, char *file)
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

/* apply_redirections:
	Applies input/output redirections for a command.
	Redirects stdin from infile, stdout to outfile 
	- if append = 1, it appends the outfile (>>)
	- if append = 2, it overwrites/truncates the outfile (>)
	Calls fd_check for safe file descriptor handling.
	file permission '0644':
	0 6 4 4
	│ │ │ │
	│ │ │ └─ others: read only (4)
	│ │ └── group: read only (4)
	│ └── owner: read + write (6)
	└── special flag (0)
*/
void	apply_redirections(t_command *cmd)
{
	int	fd;

	if (cmd->infile)
	{
		fd = open(cmd->infile, O_RDONLY);
		fd_check(fd, STDIN_FILENO, cmd->infile);
	}
	if (cmd->outfile)
	{
		if (cmd->append == 1)	// >>
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644); 
		else if (cmd->append == 2) // >
			fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		/* todo:
		else if (cmd->append == 3) // <<
			handle_heredoc(cmd); */
		fd_check(fd, STDOUT_FILENO, cmd->outfile);
	}
}
