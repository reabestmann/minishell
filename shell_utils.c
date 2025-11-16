/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_utils.c                                      :+:      :+:    :+:   */
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
	if (isatty(STDIN_FILENO))
		print_welcome();
	update_shlvl(env);
	update_last_command(env, "./minishell");
	init_signals();
}

/* removes control chars from input */
char	*remove_control_chars(const char *s)
{
	int		i;
	int		j;
	char	*cleaned;

	cleaned = malloc(ft_strlen(s) + 1);
	if (!cleaned)
		return (NULL);
	i = -1;
	j = 0;
	while (s[++i])
	{
		if (s[i] != '\t' && s[i] != '\b' && s[i] != '\v'
			&& s[i] != '\f' && s[i] != '\r' && s[i] != '\n')
			cleaned[j++] = s[i];
	}
	cleaned[j] = '\0';
	return (cleaned);
}

/*determines wether in noninteractive or interactive mode and reads the lines*/
void	read_line(char **input)
{
	char	*line;
	char	*prompt;

	prompt = "\033[38;5;182mmini$  \033[38;5;153m";

	if (isatty(STDIN_FILENO))
	{
		*input = readline(prompt);
		reset_color();
	}
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
