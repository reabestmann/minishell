/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:49 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/24 22:06:42 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int safe_atoi(const char *str, int *out)
{
    long result = 0;
    int sign = 1;
    
    if (*str == '-' || *str == '+')
    {
        if (*str == '-') sign = -1;
        str++;
    }

    if (!*str) // string was only '+' or '-'
        return (0);

    while (*str)
    {
        if (!ft_isdigit(*str))
            return (0);

        result = result * 10 + (*str - '0');
        if (sign == 1 && result > INT_MAX)
            return (0); // overflow
        if (sign == -1 && -result < INT_MIN)
            return (0); // underflow

        str++;
    }

    *out = (int)(sign * result);
    return (1);
}

void exit_cleanup(t_command *cmd, t_env **env)
{
	if(*env)
		free_env_struct(*env);
	if (cmd)
		free_commands(cmd);
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

	// Use safe_atoi to validate numeric argument and detect overflow
	if (!safe_atoi(cmd->args[1], &exit_status))
	{
		ft_putstr_fd("exit: ", 2);
		ft_putstr_fd(cmd->args[1], 2);
		ft_putendl_fd(": numeric argument required", 2);
		exit_cleanup(cmd, env);
		exit(2);
	}

	if (cmd->args[2])
	{
		ft_putendl_fd("exit: too many arguments", 2);
		return (1); // Don't exit, like Bash
	}

	exit_status &= 0xFF; // wrap into 0–255
	exit_cleanup(cmd, env);
	exit(exit_status);
}
