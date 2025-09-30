/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:49 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/30 19:05:16 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* atoi_and_overflow_check
   Converts a numeric string into a long while checking for overflow.
   @str: numeric string to parse
   @result: pointer to store intermediate result
   @sign: +1 or -1, indicates sign of the number
   Validates:
     - All characters must be digits
     - Result must stay within INT_MIN..INT_MAX
   Returns: 1 if valid and within range, 0 otherwise
*/
int	atoi_and_overflow_check(const char *str, long *result, int sign)
{
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		*result = *result * 10 + (*str - '0');
		if (sign == 1 && *result > LONG_MAX)
			return (0);
		if (sign == -1 && -(*result) < LONG_MIN)
			return (0);
		str++;
	}
	return (1);
}

/* safe_atoi
   Converts a string into an int safely, with overflow protection.
   @str: numeric string
   @out: pointer to store the final int result
   Handles optional '+' or '-' signs.
   Returns: 1 on success, 0 on invalid input or overflow
*/
static int	safe_atoi(const char *str, int *out)
{
	long	result;
	int		sign;

	result = 0;
	sign = 1;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (!*str)
		return (0);
	if (atoi_and_overflow_check(str, &result, sign) == 0)
		return (0);
	*out = (sign * result);
	return (1);
}

/* exit_cleanup
   Frees memory and cleans up before exiting.
   @cmd: pointer to the current command structure
   @env: pointer to the environment list
   Frees env and commands if allocated.
*/
void	exit_cleanup(t_command *cmd, t_env **env)
{
	if (*env)
		free_env_struct(*env);
	if (cmd)
		free_commands(cmd);
}

/* numeric_problem
   Handles invalid numeric arguments to "exit".
   @cmd: pointer to the current command structure
   @env: pointer to the environment list
   Prints an error, frees memory, and prepares for exit(2).
*/
void	numeric_problem(t_command *cmd, t_env **env)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(cmd->args[1], 2);
	ft_putendl_fd(": numeric argument required", 2);
	exit_cleanup(cmd, env);
}

/* exit_cmd
   Builtin implementation of the "exit" command.
   @cmd: parsed command structure
   @env: environment list
   Behavior:
     - No argument → exit(0)
     - One numeric argument → exit with that value (mod 256)
     - Invalid argument → error + exit(2)
     - More than one argument → error but do not exit
   Always prints "exit" before terminating.
   Returns: 1 if too many arguments, otherwise does not return
*/
int	exit_cmd(t_command *cmd, t_env **env, int last_status)
{
	int	exit_status;

	//ft_putendl_fd("exit", 1);
	if (!cmd || !cmd->args || !cmd->args[1])
	{
		exit_cleanup(cmd, env);
		exit(last_status);
	}
	if (!safe_atoi(cmd->args[1], &exit_status))
	{
		numeric_problem(cmd, env);
		exit(2);
	}
	if (cmd->args[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		return (1);
	}
	exit_status = (exit_status % 256 + 256) % 256;
	exit_cleanup(cmd, env);
	exit(exit_status);
}
