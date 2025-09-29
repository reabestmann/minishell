/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/24 12:09:20 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Call Diagram (Textual / Visual)
dollar_expansion(cmd)
    └─ expand_arg_keep_quotes(arg)
        ├─ set_state(arg, i, result, &state)
        │     ├─ set_state_one  // handles single quotes
        │     └─ set_state_two  // handles double quotes
        │     Notes:
        │       - Single quotes inside double quotes are treated as literal
        │       - Double quotes inside single quotes are treated as literal
        │       - Unclosed quotes are allowed; infinite loops avoided
        ├─ append_normal_text // used to append literal characters
        └─ append_expansion(&sub, result)
              └─ expand_one_arg(arg, &i, head, last_status)
                    ├─ append_key_value (for $VAR)
                    │      └─ append_result
                    │            └─ find_env_value
                    └─ append_result (for $?)

Quote Handling Rules:
- 0 = unquoted, ' = single, " = double
- Single quotes inside double quotes are appended literally
- Double quotes inside single quotes are appended literally
- $ expansion occurs:
    - Inside double quotes
    - Outside quotes
    - Not inside single quotes
- Unclosed quotes are allowed; processing stops safely at string end
*/

/* expand_one_arg:
- Purpose: Expand one $ token (VAR or ?)
- Calls: append_key_value, append_result, append_normal_text
- Returns: New allocated string with expansion
- Handles $?, $VAR, and lone $ at end of string
*/
char	*expand_one_arg(char *arg, int *i, t_env *head, int last_status)
{
	char	*result;
	char	*tmp;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	(*i)++;
	if (!arg[*i])
		return (append_normal_text("$", result));
	if (arg[*i] == '?')
	{
		result = append_result(head, NULL, result, last_status);
		(*i)++;
	}
	else
	{
		tmp = append_key_value(head, arg, i, result);
		if (!tmp)
			return (free(result), NULL);
		result = tmp;
	}
	return (result);
}

/*handle_normal_txt:
- Copies literal characters until a special character ($, ', ")
- Uses append_normal_text*/
char	*handle_normal_txt(int *i, char *arg, char *result)
{
	int		start;
	char	*sub;

	start = *i;
	while (arg[*i] && arg[*i] != '$' && arg[*i] != '"' && arg[*i] != '\'')
		(*i)++;
	sub = ft_substr(arg, start, *i - start);
	result = append_normal_text(sub, result);
	free(sub);
	return (result);
}

/*expand_arg_keep_quotes:
- Expands all $VAR and $? in a single argument while keeping quotes
- Replaces result string step by step
- Uses append_normal_text for all literals*/
char	*expand_arg_keep_quotes(char *arg, t_env *head, int last_status)
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

/*dollar_expansion:
- Applies expansion to all command arguments
- Frees old argument strings and replaces with expanded versions*/
void	dollar_expansion(t_command *cmd, t_env **head, int last_status)
{
	int		i;
	char	*expanded;

	i = 0;
	expanded = NULL;
	while (cmd->args[i])
	{
		expanded = expand_arg_keep_quotes(cmd->args[i], *head, last_status);
		if (!expanded)
		{
			while (i >= 0)
			{
				free(cmd->args[i]);
				cmd->args[i] = NULL;
				i--;
			}
			return ;
		}
		free(cmd->args[i]);
		cmd->args[i] = expanded;
		i++;
	}
}
