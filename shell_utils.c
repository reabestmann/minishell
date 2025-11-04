/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 17:53:59 by rbestman          #+#    #+#             */
/*   Updated: 2025/10/30 20:15:51 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* update_shlvl:
	Increments the SHLVL environment variable by 1
	when a new shell is started.
	- If SHLVL is unset or >999, defaults to 1.
	- Updates **env with the new level.
*/
void	update_shlvl(t_env **env)
{
	char	*lvl_str;
	int		lvl;
	char	*new_lvl;
	char	*to_set;

	lvl_str = get_env_value(env, "SHLVL");
	lvl = 1;
	if (lvl_str)
		lvl = ft_atoi(lvl_str);
	if (lvl < 0)
		lvl = 0;
	else if (lvl >= 1000)
		lvl = 1;
	new_lvl = ft_itoa(lvl);
	if (!new_lvl)
		return ;
	to_set = ft_strjoin("SHLVL=", new_lvl);
	free(new_lvl);
	if (!to_set)
		return ;
	update_var(to_set, env);
	free(to_set);
}

/* updates the _= variable of env*/
void	update_last_command(t_env **env, char *last_cmd)
{
	char	*to_set;

	if (!last_cmd)
		return ;
	to_set = ft_strjoin("_=", last_cmd);
	if (!to_set)
		return ;
	update_var(to_set, env);
	free(to_set);
}

/* initiates env struct, signals, terminal setup */
void	init_main_vars(int *params, char **argv, t_env **env, char **envp)
{
	(void)params;
	(void)argv;
	*env = envp_to_struct(envp);
	if (!env)
		error("environment: struct not built.");
	update_shlvl(env);
	update_last_command(env, "./minishell");
	init_signals();
	disable_ctrl_echo();
}

/*determines wether in noninteractive or interactive mode and reads the lines*/
void	read_line(char **input)
{
	char	*line;

	if (isatty(STDIN_FILENO))
		*input = readline("mini$  ");
	else
	{
		line = get_next_line(STDIN_FILENO);
		if (line)
		{
			*input = ft_strtrim(line, "\n");
			free(line);
		}
		else
			*input = NULL;
	}
}
