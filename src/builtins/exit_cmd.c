/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:49 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/23 19:10:32 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int number_check(char *arg)
{
	int i;

	if (!arg)
		return (0);
	i = 0;
	if (arg[i] == '-' || arg[i] == '+')
		i++;
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (0);
		i++;
	}
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
	int exit_status;

	exit_status = 0;
	if (!cmd || !cmd->args)
	{
		ft_putendl_fd("exit", 1);
		exit_cleanup(cmd, env);
		exit(0);
	}
	ft_putendl_fd("exit", 1);
	if (!cmd->args[1])
	{
		exit_cleanup(cmd, env);
		exit(0);
	}
	if(!number_check(cmd->args[1]))
	{
		ft_putendl_fd("exit: numeric argument required", 2);
		exit_cleanup(cmd, env);
		exit (2);
	}
	exit_status = ft_atoi(cmd->args[1]) & 0xFF;
	if(cmd->args[2])
	{
		ft_putendl_fd("exit: too many arguments", 2);
		return (1);
	}
	exit_cleanup(cmd, env);
	exit(exit_status);
}
