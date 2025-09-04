#include "../minishell.h"

/* rewire_input(t_command cmd)
	Opens cmd.infile read-only.
	- If opening fails, it prints an error and stops the program.
	- If opening works, it dup2’s the opened file onto STDIN (fd 0).
		That means: “from now on, reads come from this file instead of the keyboard.”
	Closes the temporary fd, because after dup2 the kernel keeps the connection using fd 0.
*/
void	rewire_input(t_command cmd)
{
	int new_fd;

	new_fd = -2;
	new_fd = open(cmd.infile, O_RDONLY);
	if (new_fd == -1)
	{
		perror("rewire:open infile");
		exit(1);
	}
	if (dup2(new_fd, STDIN_FILENO) == -1)
	{
		perror("rewire: dup2 infile");
		exit(1);
	}
	close(new_fd);
}
/* rewire_output(t_command cmd)
	Decides how to open cmd.outfile:
	- if cmd.append == 1: open with append (writes go to the end).
	- else: open with truncate (clears the file first).
	in both cases, create the file if it doesn’t exist (0644 permissions, subject to umask).
	If opening fails, it prints an error and stops the program.
	dup2 the opened file onto STDOUT (fd 1).
		That means: “from now on, prints go into this file instead of the terminal.”
	Close the temporary fd (same reason as above).
*/
void	rewire_output(t_command cmd)
{
	int new_fd;

	new_fd = -2;
	if (cmd.append == 1)
		new_fd = open(cmd.outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else 
		new_fd = open(cmd.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (new_fd == -1)
	{
		perror("rewire:open outfile");
		exit(1);
	}
	if (dup2(new_fd, STDOUT_FILENO) == -1)
	{
		perror("rewire: dup2 outfile");
		exit(1);
	}
	close(new_fd);
}
/* rewire_pipes(int pipe_fd[2], t_pipe_mode mode)
	- pipe_fd[0] is the read end of the pipe.
	- pipe_fd[1] is the write end.
	Depending on mode:
	- PIPE_IN: hook the pipe’s read end to STDIN (so we read from the pipe).
	- PIPE_OUT: hook the pipe’s write end to STDOUT (so we write into the pipe).
	- PIPE_BOTH: do both.
	After using dup2, close both original pipe fds in this process; the duplicated fds (0 or 1) stay open.
*/
void	rewire_pipes(int pipe_fd[2], t_pipe_mode mode)
{
	if (mode == PIPE_IN || mode == PIPE_BOTH)
	{
		if (dup2(pipe_fd[0], STDIN_FILENO) == -1)
		{
			perror("rewire: dup2 pipe read");
			exit(1);
		}
	}
	if (mode == PIPE_OUT || mode == PIPE_BOTH)
	{
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
		{
			perror("rewire: dup2 pipe write");
			exit(1);
		}
	}
	close(pipe_fd[0]);
	close(pipe_fd[1]);
}

/*	rewire(t_command cmd, int pipe_fd[2], t_pipe_mode mode)
	Calls the specific helpers to set up files and pipes for this command.
	Current order: input file → output file → pipes.
*/
void	rewire(t_command cmd, int pipe_fd[2], t_pipe_mode mode) //in = 1 means its input side of pipe 0 means its output side and -1 if not pipe
{
	if (mode != NO_PIPE)
		rewire_pipes(pipe_fd, mode);
	if (cmd.infile != NULL)
		rewire_input(cmd);
	if (cmd.outfile != NULL)
		rewire_output(cmd);
}
