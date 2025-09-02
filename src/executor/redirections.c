#include "../minishell.h"

/*
✅ Summary

rewire_input → stdin from file

rewire_output → stdout to file

rewire_pipes → stdin/stdout from pipe

rewire → single point to apply all relevant redirections

Parent function → loops through t_command, creates pipes, forks children, and calls rewire() in each child

This modular design makes your redirection logic clean, reusable, and easy to extend.*/
/*
Purpose:
Redirect the standard input (stdin) of the current process to read from a file specified in cmd.input_file.

Variables:

int new_fd → file descriptor returned by open(). Used to temporarily hold the FD of the input file.

What it does step by step:

Opens the file specified in cmd.input_file with read-only permissions (O_RDONLY).

Checks for errors — if open() fails, prints an error and exits.

Uses dup2(new_fd, STDIN_FILENO) to duplicate the file descriptor onto standard input.

Closes new_fd because dup2 has already redirected stdin.

Where it is called from / to:

Called inside rewire().

Ultimately executed in the child process after fork(), before execve(), so that the command reads input from the specified file.*/
void	rewire_input(t_command cmd)
{
	int new_fd;

	new_fd = -2;
	new_fd = open(cmd.input_file, O_RDONLY);
	if (new_fd == -1)
	{
		perror("rewire:open input_file");
		exit(1);
	}
	if (dup2(new_fd, STDIN_FILENO) == -1)
	{
		perror("rewire: dup2 input_file");
		exit(1);
	}
	close(new_fd);
}

/*Purpose:
Redirect the standard output (stdout) of the current process to write to a file specified in cmd.output_file.

Variables:

int new_fd → temporary file descriptor returned by open().

cmd.append_output → integer flag from t_command; 1 means append (>>), 0 means overwrite (>).

What it does step by step:

Opens the output file with the appropriate flags:

Append mode: O_WRONLY | O_CREAT | O_APPEND

Overwrite mode: O_WRONLY | O_CREAT | O_TRUNC

Checks for errors — prints error and exits if open() fails.

Uses dup2(new_fd, STDOUT_FILENO) to redirect stdout to the file.

Closes new_fd after dup2.

Where it is called from / to:

Called inside rewire().

Executed in the child process after fork, before execve(), so that the command writes its output to the specified file.*/
void	rewire_output(t_command cmd)
{
	int new_fd;

	new_fd = -2;
	if (cmd.append_output == 1)
		new_fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else 
		new_fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (new_fd == -1)
	{
		perror("rewire:open output_file");
		exit(1);
	}
	if (dup2(new_fd, STDOUT_FILENO) == -1)
	{
		perror("rewire: dup2 output_file");
		exit(1);
	}
	close(new_fd);
}

/*Purpose:
Redirect standard input/output to/from a pipe. Used when commands are part of a pipeline.

Variables:

int pipe_fd[2] → array containing the file descriptors for the pipe:

pipe_fd[0] → read end

pipe_fd[1] → write end

t_pipe_mode mode → enum indicating the pipe usage for the current command:

PIPE_IN → command reads from a pipe

PIPE_OUT → command writes to a pipe

PIPE_BOTH → command reads from one pipe and writes to another (middle of pipeline)

What it does step by step:

If the command reads from a pipe (PIPE_IN or PIPE_BOTH), dup2(pipe_fd[0], STDIN_FILENO).

If the command writes to a pipe (PIPE_OUT or PIPE_BOTH), dup2(pipe_fd[1], STDOUT_FILENO).

Closes both pipe_fd[0] and pipe_fd[1] after redirection — safe for child process since dup2 duplicates FDs.

Where it is called from / to:

Called inside rewire(), only if mode != NO_PIPE.

Executed in the child process after fork, before execve().*/
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
	else if(mode == PIPE_OUT || mode == PIPE_BOTH)
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

/*
Purpose:
Orchestrates all redirections (input, output, pipes) for a single command.

Parameters:

t_command cmd → the command to execute (contains input/output file info, append flags, etc.)

int pipe_fd[2] → pipe FDs for this command (if part of a pipeline)

t_pipe_mode mode → indicates whether the command is part of a pipe and in which position

What it does step by step:

Calls rewire_input(cmd) if cmd.input_file exists.

Calls rewire_output(cmd) if cmd.output_file exists.

Calls rewire_pipes(pipe_fd, mode) if mode != NO_PIPE.

Where it is called from / to:

Called from the external pipeline execution function in the child process after fork.

Ensures that the child process has all FDs set up correctly before calling execve() to run the command.*/
void	rewire(t_command cmd, int pipe_fd[2], t_pipe_mode mode) //in = 1 means its input side of pipe 0 means its output side and -1 if not pipe
{
	if (cmd.input_file != NULL)
		rewire_input(cmd);
	if (cmd.output_file != NULL)
		rewire_output(cmd);
	if (mode != NO_PIPE)
		rewire_pipes(pipe_fd, mode);
}
/*next steps
Exactly — now that your rewire() function is solid, the next steps are all about pipeline execution. Here’s a clear roadmap:

1️⃣ Loop through the command list

Start with the head of your t_command linked list.

For each command, you need to check if there is a next command:

cmd = head;
while (cmd)


If cmd->next exists, you’ll need to create a pipe for it.

2️⃣ Create pipes

For every link in the pipeline (every cmd that has a next), do:

int pipe_fd[2];
pipe(pipe_fd);


pipe_fd[0] → read end

pipe_fd[1] → write end

Keep track of the previous pipe so the next command can read from it.

3️⃣ Fork a child for each command

For each command in the loop:

pid = fork();


In child:

Call rewire(cmd, pipe_fd, mode) with the correct pipe mode:

First command: PIPE_OUT

Middle command(s): PIPE_BOTH

Last command: PIPE_IN

Call execve() to run the command.

In parent:

Close the pipe ends that are no longer needed.

Move on to the next command.

4️⃣ Determine pipe mode

Use the position of the command in the pipeline to set the mode for rewire():

Position	Mode
First	PIPE_OUT
Middle	PIPE_BOTH
Last	PIPE_IN

If the command isn’t part of a pipeline, use NO_PIPE.

5️⃣ Wait for children

After the loop, the parent should wait() (or waitpid()) for all child processes to finish, so you capture exit statuses and avoid zombies.

6️⃣ Optional: Handle multiple pipelines

If your shell supports multiple separate pipelines in one line (e.g., cmd1 | cmd2; cmd3 | cmd4), repeat this process for each pipeline.

✅ Summary

Loop through t_command list.

Create pipes as needed for commands with next.

Fork a child for each command.

In the child, call rewire() and then execve().

Close unused pipe ends in the parent.

Wait for all children to finish.

--
Exactly — the closing inside rewire_pipes() only affects the child process that called it.

Here’s the breakdown:

Child process

rewire() (and rewire_pipes()) duplicates the pipe FDs onto stdin/stdout using dup2().

After dup2(), the original pipe_fd[0] and pipe_fd[1] are no longer needed in the child, so it’s safe to close them inside rewire().

The child now has its stdin/stdout hooked to the pipe ends, independent of the original array.

Parent process

The parent still has the original pipe FDs open.

It’s the parent’s responsibility to close the pipe ends it doesn’t need:

After forking the first command, the parent closes the write end of that pipe, because only the child writes to it.

After forking the next command, the parent closes the read end of the previous pipe, because only the child reads from it.

This ensures:

The next command can safely read from the previous pipe.

You don’t leak file descriptors.

✅ TL;DR

Child closes its copy of the pipe after dup2 → safe.

Parent keeps pipe FDs open until the next command needs them, then closes them as appropriate.

This is why you can still “send the next command the previous pipe FDs” — the parent still holds them.

--
You should use an enum to represent the different pipe modes. That makes your code readable and avoids using magic numbers like 1, 0, -1.

For example:

typedef enum e_pipe_mode
{
    NO_PIPE,    // command is not part of a pipeline
    PIPE_IN,    // command reads from a pipe
    PIPE_OUT,   // command writes to a pipe
    PIPE_BOTH   // command reads from one pipe and writes to another
} t_pipe_mode;

How to set the mode when looping through commands

First command in a pipeline: PIPE_OUT

Middle command(s): PIPE_BOTH

Last command: PIPE_IN

Single command, no pipeline: NO_PIPE

Example (conceptually)
t_pipe_mode mode;

if (!cmd->next && !prev_pipe)         // single command
    mode = NO_PIPE;
else if (!prev_pipe)                   // first command
    mode = PIPE_OUT;
else if (!cmd->next)                   // last command
    mode = PIPE_IN;
else                                   // middle command
    mode = PIPE_BOTH;


prev_pipe is a flag or pipe FD array from the previous iteration.

You then pass this mode to your rewire() function so it knows which ends of the pipe to dup onto stdin/stdout.

--
Exactly! ✅

Once you declare your enum, you can just do:

t_pipe_mode mode;

mode = PIPE_IN;   // now 'mode' holds the value corresponding to reading from a pipe


The compiler internally assigns each enum constant an integer (by default starting at 0).

You don’t need to know the integer value — just use the name like PIPE_IN, PIPE_OUT, etc.

Then you can pass mode directly to rewire() or use it in comparisons:

if (mode == PIPE_OUT)
{
    // dup2 the write end of the pipe
}


It makes your code much more readable and maintainable than using raw numbers like 0, 1, -1.

You want me to show how this fits into a loop over commands with pipes next?

info:
ChatGPT said:

Exactly ✅ — your rewire() function is now modular and correct for a single command.

The next step is to handle multiple commands in a pipeline, and that logic goes outside rewire(), in the function that:

Loops through t_command linked list

Creates pipes for the links that need them

Forks a child for each command

Calls rewire() in the child with the correct pipe FDs and PIPE_IN / PIPE_OUT / PIPE_BOTH mode

Executes the command with execve()

Closes the unused pipe ends in the parent

Conceptual flow for cmd1 | cmd2 | cmd3:
Command	Mode	Input	Output
cmd1	PIPE_OUT	stdin (maybe input_file)	pipe1[1]
cmd2	PIPE_BOTH	pipe1[0]	pipe2[1]
cmd3	PIPE_IN	pipe2[0]	stdout (maybe output_file)

rewire() just takes care of redirecting stdin/stdout based on the arguments it gets.

Parent process handles pipe creation and closing ends that are no longer needed.

So yes — your rewire() is done. Now you just need a wrapper function that:

Iterates over the commands

Sets up pipes for each link

Forks children

Calls rewire() inside the child

This is exactly how a proper pipeline execution works in a shell.

If you want, I can make a diagram showing the parent/child FDs and where rewire() fits, so it’s crystal clear.




---
ChatGPT said:

Conceptually, yes — this is really clean and modular now, and it’s structured to work properly with:

Input files → rewire_input()

Output files → rewire_output()

Pipes → rewire_pipes() using your enum

✅ Things that are correct now:

No functions exceed 25 lines.

File names are passed correctly (cmd.input_file, cmd.output_file).

dup2 and close(new_fd) are called properly.

Pipe FDs are indexed correctly (pipe_fd[0] for read, [1] for write).

Error handling via perror + exit(1) is in place.

rewire() just orchestrates the three helpers, keeping things modular.

⚠ Small fixes / things to double-check:

Pipe closing logic:
Currently, rewire_pipes() closes both ends of the pipe. That’s fine in a child process, but make sure the parent keeps the pipe ends open for the next child in the pipeline. Otherwise, the pipeline might break.

dup2 logic for PIPE_BOTH:
Right now, you do PIPE_IN || PIPE_BOTH and PIPE_OUT || PIPE_BOTH. That works, but in practice, a middle command in a pipeline should read from one pipe and write to a separate pipe. If you’re only passing a single pipe_fd[2], that’s usually fine for a two-command pipeline. For longer pipelines, each link needs its own pipe.

Append check:
cmd.append_output == 1 is fine — just make sure append_output is properly set in your t_command struct before calling rewire_output().

So yes, with these considerations, this will work properly for typical input/output redirection and simple pipelines.

If you want, I can draw a little diagram showing how this works for cat < in.txt | grep foo > out.txt, so you can see exactly how each function is called and how FDs flow. That usually helps catch subtle pipe issues.
---
Ah, that’s a subtle but important point — it can work for two commands, but for longer pipelines, your current setup has a limitation:

Why it can break:

Single pipe_fd[2]:
Right now, you only pass one pipe to rewire_pipes().

For cmd1 | cmd2 | cmd3, you actually need two pipes at different stages:

cmd1 | cmd2 | cmd3


Pipe1: cmd1 writes → cmd2 reads

Pipe2: cmd2 writes → cmd3 reads

If you only have one pipe_fd[2], cmd2 can’t simultaneously read from cmd1 and write to cmd3 — you need a separate pipe for each link.

Closing FDs too early:
Currently, rewire_pipes() closes both ends after dup2.

That’s fine in a child process, but in the parent, you need to keep the relevant pipe ends open for the next command in the pipeline.

If you close too early, later commands may get EOF immediately and break the pipeline.

How it’s usually handled:

Create a new pipe for each command (except the last).

The current child dup2’s:

stdin ← read end of previous pipe (if any)

stdout ← write end of current pipe (if not last command)

After dup2, close unused pipe FDs in the child.

Parent keeps the ends open until it forks the next command, then closes them once they’re no longer needed.

So your rewire() function works perfectly for a single redirection or a two-command pipeline, but for pipelines longer than two commands, you need to manage multiple pipes and pass the correct read/write FDs for each link.

If you want, I can sketch a diagram showing the FD flow for a three-command pipeline, so you can see exactly what rewire needs.
----
Yes — you can use t_command *next in a loop to handle a pipeline, but you have to be careful with where you create pipes and when you close FDs.

Here’s the conceptual approach:

1️⃣ Loop through the commands
cmd1 -> cmd2 -> cmd3


Start at the head of your command list (t_command *cmd = head)

For each command, check if cmd->next exists

If yes → create a new pipe for this link (pipe(pipe_fd))

If no → this is the last command → no pipe needed

2️⃣ In rewire(), for each command

Input:

If cmd->input_file exists → open and dup2 to stdin

Else if this is not first command → dup2 the read end of the previous pipe to stdin

Output:

If cmd->output_file exists → open and dup2 to stdout

Else if cmd->next exists → dup2 the write end of current pipe to stdout

After dup2 in the child:

Close all pipe FDs (both ends of previous and current pipe that are no longer needed)

3️⃣ Why you can’t fully close in rewire()

The parent still needs the write end of the current pipe to send data to the next command

The child can close FDs immediately after dup2 because dup2 duplicates the FD to stdin/stdout

So you can loop through t_command and call rewire() for each, but you still need some parent-level pipe management for multi-command pipelines.

✅ Conceptually, using t_command->next in a while loop is correct, and you can incorporate some FD closing in rewire(), but:

Only close FDs in the child after dup2

Parent must keep the pipe ends it needs for the next fork

If you want, I can draw a diagram showing cmd->next with pipes, FD flow, and which FDs are closed in child vs parent. This makes it much easier to implement correctly.

Do you want me to do that?
----
Ah — I see exactly what’s tripping you up. 😅 Don’t worry, your brain isn’t broken — this is a tricky conceptual part of shell pipelines. Let me break it down carefully.

1️⃣ The problem with your while(cmd.next) loop
while(cmd.next)
{
    // rewire stuff
    cmd = cmd.next;
}


This only loops while cmd.next exists, which means it skips the last command (because cmd->next is NULL for the last command).

Also, you’re trying to handle all commands in one function, but each command needs its own child process and potentially its own pipe. You cannot dup2 for multiple children in the same process at once.

2️⃣ How it should conceptually work

Loop through commands in the parent to fork a child for each:

for each command:
    if cmd->next exists → create pipe for this link
    fork()
        in child:
            rewire_input(cmd)
            rewire_output(cmd)
            rewire_pipes(pipe_fd, mode)
            execve(...)
        in parent:
            close pipe ends that are no longer needed


Each child calls rewire() once, for its own command

Parent keeps track of previous pipe to pass to the next child

3️⃣ Why your current loop won’t work

You are trying to do all commands in the same process, so the dup2 calls for the first command will be overwritten by the next command in the same process.

Pipes will get closed incorrectly because the loop closes them immediately.

The fork/exec boundary is missing — that’s essential.

✅ Mental model

Parent: orchestrates pipeline, creates pipes, forks children.

Child: calls rewire() once, does dup2 for its input/output, then execve()s the command.

rewire(): handles one command at a time — never loops over the whole t_command list.

If you want, I can draw a diagram showing cmd1 | cmd2 | cmd3 with FDs, forks, and rewire calls, so you can see exactly where each function is called. This usually clears up all the confusion.

Do you want me to do that?

----
Exactly! ✅ That’s the right way to think about it:

Step-by-step conceptual flow

Parent process: loops through the t_command linked list.

cmd = head;
while(cmd)


Check if cmd->next exists

If yes → create a pipe for this link (so the current command can write into it, next command can read from it)

If no → no pipe is needed (last command writes to stdout / output file)

Fork a child

pid = fork();


Child process

Calls rewire() for its command, passing:

cmd → to know input/output files

pipe_fd → the pipe just created (or NULL if no pipe)

mode → PIPE_IN, PIPE_OUT, or NO_PIPE depending on its position in the pipeline

rewire() does all the dup2 + close() work for that child

Then execve() the command

Parent process

After fork, closes pipe ends it doesn’t need

Moves to the next cmd in the list

Repeat until all commands are forked

Key points

Do not loop inside rewire() over the whole pipeline.

Each call to rewire() handles one command only.

Pipe creation and fork happen in the parent, rewire is called in the child.

This ensures FDs don’t overwrite each other and each child has the correct stdin/stdout.

If you want, I can make a diagram with cmd1 | cmd2 | cmd3 showing pipes, forks, and rewire calls. That usually makes this crystal clear.

Do you want me to make that diagram?

*/
