/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/22 18:14:14 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*
Call Diagram (Textual / Visual)
dollar_expansion(cmd)
    └─ expand_arg_keep_quotes(arg)
        ├─ set_state(arg, i, result, &state)
        │     ├─ set_state_one
        │     └─ set_state_two
        ├─ add_temp (for literal characters)
        └─ append_expansion(&sub, result)
              └─ expand_one_arg(arg, &i, head, last_status)
                    ├─ append_key_value (for $VAR)
                    │      └─ append_result
                    │            └─ find_env_value
                    └─ append_result (for $?)
*/
// find_env_value:
//  - Purpose: Return value of environment variable by key
//  - Calls: ft_strncmp, ft_strlen, ft_strdup
//  - Returns: value string if found, "" if not
//  - Memory: Caller must free returned string
char	*find_env_value(t_env *head, char *key)
{
	t_env	*temp;

	temp = head;
	while (temp)
	{
		if (ft_strncmp(temp->key, key, ft_strlen(key) + 1) == 0)
			return (ft_strdup(temp->value));
		temp = temp->next;
	}
	return (ft_strdup(""));
}
// set_result:
//  - Purpose: Concatenate result + val into a new string
//  - Calls: malloc, ft_strlcpy, ft_strlcat
//  - Returns: New allocated string or NULL on failure
//  - Memory: Does not free inputs
char	*set_result(int res_len, int val_len, char *result, char *val)
{
	char	*new_result;

	new_result = malloc((res_len + val_len + 1));
	if (!new_result)
		return (NULL);
	if (result)
		ft_strlcpy(new_result, result, res_len + 1);
	else
		new_result[0] = '\0';
	ft_strlcat(new_result, val, res_len + val_len + 1);
	return (new_result);
}

// append_result:
//  - Purpose: Append env value or last_status to result
//  - Calls: find_env_value, ft_itoa, set_result, ft_strlen, free
//  - Returns: New concatenated string or NULL on malloc failure
//  - Memory: Frees old result and temporary value if dynamically allocated
char	*append_result(t_env *head, char *key, char *result, int last_status)
{
	char	*val;
	int		res_len;
	int		val_len;
	char	*new_result;

	if (last_status == -1)
		val = find_env_value(head, key);
	else
		val = ft_itoa(last_status);
	if (!val)
		return (NULL);
	if (result)
		res_len = ft_strlen(result);
	else
		res_len = 0;
	val_len = ft_strlen(val);
	new_result = set_result(res_len, val_len, result, val);
	if (last_status != -1)
		free(val);
	if (result)
		free(result);
	return (new_result);
}
// is_valid_key:
//  - Purpose: Check if arg[i] starts a valid env key
//  - Calls: ft_isalpha, ft_isalnum
//  - Returns: 1 if valid, 0 if not
//  - Side effect: sets *len to key length
int	is_valid_key(char *arg, int i, int *len)
{
	int	start;

	start = i;
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
		return (0);
	i++;
	while (arg[i] != '\0' && (ft_isalnum(arg[i]) || arg[i] == '_'))
		i++;
	*len = i - start;
	return (1);
}

// append_normal_text:
//  - Purpose: Append literal text to result
//  - Calls: set_result, ft_strlen, free
//  - Returns: New concatenated string or NULL on malloc failure
//  - Memory: Frees old result
char	*append_normal_text(char *text, char *result)
{
	int		res_len;
	int		val_len;
	char	*new_result;

	if (!text)
		return (result);
	if (result)
		res_len = ft_strlen(result);
	else
		res_len = 0;
	val_len = ft_strlen(text);
	new_result = set_result(res_len, val_len, result, text);
	if (result)
		free(result);
	return (new_result);
}

// set_state_one / set_state_two / set_state:
//  - Purpose: Track single and double quote states and append quote characters
//  - Calls: append_normal_text
//  - Returns: Updated result string
char	*set_state_one(char *arg, int *i, char *result, char *state)
{
	if (arg[*i] == '\'' && *state == 0)
	{
		result = append_normal_text("'", result);
		*state = '\'';
		(*i)++;
	}
	else if (arg[*i] == '\'' && *state == '\'')
	{
		result = append_normal_text("'", result);
		*state = 0;
		(*i)++;
	}
	return (result);
}

char	*set_state_two(char *arg, int *i, char *result, char *state)
{
	if (arg[*i] == '"' && *state == 0)
	{
		result = append_normal_text("\"", result);
		*state = '"';
		(*i)++;
	}
	else if (arg[*i] == '"' && *state == '"')
	{
		result = append_normal_text("\"", result);
		*state = 0;
		(*i)++;
	}
	return (result);
}

char	*set_state(char *arg, int *i, char *result, char *state)
{
	result = set_state_one(arg, i, result, state);
	result = set_state_two(arg, i, result, state);
	return (result);
}

// append_key_value:
//  - Purpose: Append the value of $VAR to result
//  - Calls: is_valid_key, malloc, ft_strlcpy, append_result, free
//  - Returns: New result string or NULL on malloc failure
char	*append_key_value(t_env *head, char *arg, int *i, char *result)
{
	char	*key;
	int		key_len;

	if (!is_valid_key(arg, *i, &key_len))
		return (append_normal_text("$", result));
	key = malloc(key_len + 1);
	if (!key)
		return (free(result), NULL);
	ft_strlcpy(key, arg + *i, key_len + 1);
	result = append_result(head, key, result, -1);
	free(key);
	*i += key_len;
	return (result);
}


// expand_one_arg:
//  - Purpose: Expand one $ token (VAR or ?)
//  - Calls: append_key_value, append_result
//  - Returns: New allocated string with expansion
char	*expand_one_arg(char *arg, int *i, t_env *head, int last_status)
{
	char	*result;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	if (arg[*i] == '\0')
		return (result);
	(*i)++;
	if (arg[*i] == '\0')
		return (append_normal_text("$", result));
	if (arg[*i] == '?')
	{
		result = append_result(head, NULL, result, last_status);
		(*i)++;
	}
	else
		result = append_key_value(head, arg, i, result);
	return (result);
}


// append_expansion:
//  - Purpose: Append expanded $ token to result
//  - Calls: expand_one_arg, append_normal_text
//  - Returns: New result string or NULL on malloc failure
char	*append_expansion(char **sub, char *result)
{
	if (!*sub)
		return (free(result), NULL);
	result = append_normal_text(*sub, result);
	free(*sub);
	return (result);
}

// add_temp:
//  - Purpose: Append a single literal character to result
//  - Calls: append_normal_text
//  - Returns: Updated result string
char	*add_temp(char *arg, char *result, int *i)
{
	char	temp[2];

	temp[0] = arg[*i];
	temp[1] = '\0';
	result = append_normal_text(temp, result);
	(*i)++;
	return (result);
}

// expand_arg_keep_quotes:
//  - Purpose: Expand all $VAR and $? in a single argument, keeping quotes
//  - Calls: set_state, init_expansion, add_temp, expand_one_arg
//  - Returns: New allocated string with expansions
// 0 = unquoted, ' = single, " = double
char	*expand_arg_keep_quotes(char *arg, t_env *head, int last_status)
{
	int		i;
	char	state;
	char	*result;
	char	*sub;

	i = 0;
	state = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	while (arg[i])
	{
		result = set_state(arg, &i, result, &state);
		if (!arg[i])
			break ;
		if (arg[i] == '$' && state != '\'')
		{
			sub = expand_one_arg(arg, &i, head, last_status);
			result = append_expansion(&sub, result);
			continue ;
		}
		else
			result = add_temp(arg, result, &i);
	}
	return (result);
}


// dollar_expansion:
//  - Purpose: Apply $ expansion to all arguments of a command
//  - Calls: expand_arg_keep_quotes
//  - Memory: Frees old argument strings, replaces with expanded versions
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
			return ;
		free(cmd->args[i]);
		cmd->args[i] = expanded;
		i++;
	}
}
