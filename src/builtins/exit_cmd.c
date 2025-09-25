/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:49 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/25 18:28:53 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	atoi_and_overflow_check(const char *str, long *result, int sign)
{
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (0);
		*result = *result * 10 + (*str - '0');
		if (sign == 1 && *result > INT_MAX)
			return (0);
		if (sign == -1 && -(*result) < INT_MIN)
			return (0);
		str++;
	}
	return (1);
}

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
	*out = (int)(sign * result);
	return (1);
}

void	exit_cleanup(t_command *cmd, t_env **env)
{
	if (*env)
		free_env_struct(*env);
	if (cmd)
		free_commands(cmd);
}

void	numeric_problem(t_command *cmd, t_env **env)
{
	ft_putstr_fd("exit: ", 2);
	ft_putstr_fd(cmd->args[1], 2);
	ft_putendl_fd(": numeric argument required", 2);
	exit_cleanup(cmd, env);
}

int	exit_cmd(t_command *cmd, t_env **env)
{
	int	exit_status;

	ft_putendl_fd("exit", 1);
	if (!cmd || !cmd->args || !cmd->args[1])
	{
		exit_cleanup(cmd, env);
		exit(0);
	}
	if (!safe_atoi(cmd->args[1], &exit_status))
	{
		numeric_problem(cmd, env);
		exit(2);
	}
	if (cmd->args[2])
	{
		ft_putendl_fd("exit: too many arguments", 2);
		return (1);
	}
	exit_status &= 0xFF;
	exit_cleanup(cmd, env);
	exit(exit_status);
}
