/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/22 22:02:58 by aabelkis         ###   ########.fr       */
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
/*find_env_value:
- Purpose: Return the value of an environment variable by key
- Returns: Duplicated value string if found, "" if not
- Memory: Caller must free the returned string*/
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

/*set_result:
- Purpose: Concatenate result + val into a new allocated string
- Returns: New allocated string or NULL on failure
- Memory: Does not free inputs
- Caller responsible for freeing previous result*/
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

void	free_val(int *last_status, char **val)
{
	if (*last_status != -1)
		free(*val);
}

/*append_result:
- Purpose: Append env value or last_status to result
- Memory: Frees old result and temporary value if dynamically allocated
- Notes: last_status == -1 means use env value, otherwise append last_status 
	as string
*/
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
	if (!new_result)
		return (free_val(&last_status, &val), NULL);
	if (last_status != -1)
		free(val);
	if (result)
		free(result);
	return (new_result);
}

/*is_valid_key:
- Purpose: Check if arg[i] starts a valid env key
- Returns: 1 if valid, 0 if not
- Side effect: sets *len to key length*/
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

/*append_normal_text:
- Appends literal text to result using set_result
- Frees old result
- Always used for literal characters including quotes*/
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
	if (!new_result)
	{
		free(result);
		return (NULL);
	}
	if (result)
		free(result);
	return (new_result);
}

/*set_state_one / set_state_two:
- Purpose: Track quote states and append quote characters
- Notes:
    - Single quotes inside double quotes are literal
    - Double quotes inside single quotes are literal
    - Unclosed quotes are allowed; processing stops safely at end of string
    - Always uses append_normal_text*/
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
	else if (arg[*i] == '\'' && *state == '"')
	{
		result = append_normal_text("'", result);
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
	else if (arg[*i] == '"' && *state == '\'')
	{
		result = append_normal_text("\"", result);
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
//- Allocates memory for key, frees it after use
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
//  - Calls: append_key_value, append_result, append_normal_text
//  - Returns: New allocated string with expansion
//- Handles $?, $VAR, and lone $ at end of string
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
//  - Calls: append_normal_text
//  - Returns: New result string or NULL on malloc failure
char	*append_expansion(char **sub, char *result)
{
	if (!*sub)
		return (free(result), NULL);
	result = append_normal_text(*sub, result);
	free(*sub);
	return (result);
}

/*copy_squote:
- Copies characters inside single quotes to result
- Stops safely if closing quote is missing*/
char	*copy_squote(int *i, char *arg, char *result)
{
	int		start;
	char	*sub;

	start = *i;
	while (arg[*i] && arg[*i] != '\'')
		(*i)++;
	sub = ft_substr(arg, start, *i - start);
	result = append_normal_text(sub, result);
	free(sub);
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

/*
set_vars:
- Initializes variables for expand_arg_keep_quotes*/
int	set_vars(int *i, char *state, char **result)
{
	*i = 0;
	*state = 0;
	*result = ft_strdup("");
	if (!*result)
		return (0);
	else
		return (1);
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
	char	*sub;

	if (!set_vars(&i, &state, &result))
		return (NULL);
	while (arg[i])
	{
		result = set_state(arg, &i, result, &state);
		if (state == '\'')
			result = copy_squote(&i, arg, result);
		else if (arg[i] == '$')
		{
			sub = expand_one_arg(arg, &i, head, last_status);
			result = append_expansion(&sub, result);
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
