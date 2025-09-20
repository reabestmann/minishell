/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/19 21:38:00 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// find_env_value:
//  - Called from: append_result
//  - Purpose: return value of env variable by key
//  - Calls: ft_strncmp, ft_strlen
//  - Returns: value string if found, "" if not
//  - Memory: does not free anything
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
	return (ft_strdup("")); //saying that key wasnt found so no only appending empty string
}

// set_result:
//  - Called from: append_result
//  - Purpose: concatenate result + val into new string
//  - Calls: malloc, ft_strlcpy, ft_strlcat
//  - Returns: new allocated string, NULL on malloc fail
//  - Memory: frees nothing
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
//  - Called from: expand_one_arg, set_key, append_rest, append_questionmark
//  - Purpose: append env value or last_status to result
//  - Calls: find_env_value, ft_itoa, set_result, ft_strlen, free
//  - Returns: new concatenated string, NULL on malloc fail
//  - Memory: frees old result and val if dynamically allocated
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
//  - Called from: expand_one_arg, set_key
//  - Purpose: check if arg[i] starts a valid env key
//  - Calls: ft_isalpha, ft_isalnum
//  - Returns: 1 if valid, 0 if not
//  - Memory: none
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

// set_key:
//  - Called from: expand_one_arg
//  - Purpose: extract key from arg and append its value to result
//  - Calls: is_valid_key, malloc, ft_strlcpy, append_result, free
//  - Returns: 0 on success, -1 on malloc fail
//  - Memory: frees temporary key buffer
int	set_key(t_env *head, char *arg, int *i, char **result)
{
	char	*key;
	int		key_len;

	if (is_valid_key(arg, *i, &key_len))
	{
		key = malloc(key_len + 1);
		if (!key)
			return (-1);
		ft_strlcpy(key, arg + *i, key_len + 1);
		*result = append_result(head, key, *result, -1); //i assume -1 is an invalid last_status
		free(key);
		*i += key_len;
	}
	return (0);
}

char *append_normal_text(char *text, char *result)
{
	int res_len;
	int val_len;
	char *new_result;

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

// append_rest:
//  - Called from: expand_one_arg
//  - Purpose: append literal text (not part of $VAR or $?) to result
//  - Calls: malloc, ft_strlcpy, append_result, free
//  - Returns: 0 on success, -1 on malloc fail
//  - Memory: frees temporary buffer for literal
int	append_rest(int *i, char *arg, char **result)
{
	int		start;
	char	*rest;
	
	if (!arg || !result || !i)
		return (-1);
	start = *i;
	while (arg[*i] && arg[*i] != '$')
		(*i)++;
	rest = malloc(*i - start + 1);
	if (!rest)
		return (-1);
	ft_strlcpy(rest, arg + start, *i - start + 1);
	*result = append_normal_text(rest, *result);
	free(rest);
	return (0);
}

// append_questionmark:
//  - Called from: expand_one_arg
//  - Purpose: append last_status ($?) to result
//  - Calls: append_result
//  - Returns: void
//  - Memory: frees nothing
void	append_questionmark(char **result, t_env *head, int *i, int last_status)
{
	*result = append_result(head, NULL, *result, last_status);
	(*i)++;
}

// expand_one_arg:
//  - Called from: dollar_expansion
//  - Purpose: expand all $VAR and $? in a single argument string
//  - Calls: append_questionmark, is_valid_key, set_key, append_rest, 
//			append_result, ft_strdup, free
//  - Returns: new allocated string with expansions, NULL on malloc fail
//  - Memory: frees intermediate result on error
char	*expand_one_arg(char *arg, t_env *head, int last_status)
{
	char	*result;/*dont forget to free result later*/
	int		key_len;
	int		i;

	i = 0;
	result = ft_strdup("");
	while (arg[i] != '\0')
	{
		if (arg[i] == '$')
		{
			i++;
			if (arg[i] == '?') //
				append_questionmark(&result, head, &i, last_status);
			else if (is_valid_key(arg, i, &key_len))
			{
				if (set_key(head, arg, &i, &result) == -1)
					return (free(result), NULL);
			}
			else
				result = append_result(head, "$", result, -1);
		}
		else
		{
			if (append_rest(&i, arg, &result) == -1)
				return (free(result), NULL);
		}
	}
	return (result);
}

//
char *join_and_free(char *dst, char *src)
{
    char *joined;
    int len1 = dst ? ft_strlen(dst) : 0;
    int len2 = src ? ft_strlen(src) : 0;

    joined = malloc(len1 + len2 + 1);
    if (!joined)
        return (free(dst), free(src), NULL);

    if (dst)
        ft_strlcpy(joined, dst, len1 + 1);
    else
        joined[0] = '\0';

    if (src)
        ft_strlcat(joined, src, len1 + len2 + 1);

    free(dst);
    free(src);
    return joined;
}

/*
 * expand_arg:
 *  - Handles $ expansion in unquoted or double-quoted parts
 *  - Skips expansion in single quotes
 *  - Returns fully expanded string (malloc'd)
 */
///// this one almost worked
/*char *expand_arg(const char *arg, t_env *head, int last_status)
{
    char state = 0;   // 0 = unquoted, ' = single, " = double
    int i = 0, start = 0;
    char *result = ft_strdup("");
    if (!result)
        return NULL;

    while (arg[i]) {
        if (state == 0) {
            if (arg[i] == '\'') {
                // expand everything before the quote
                if (i > start) {
                    char *chunk = ft_substr(arg, start, i - start);
                    if (!chunk)
                        return (free(result), NULL);
                    result = join_and_free(result,
                        expand_one_arg(chunk, head, last_status));
                }
                state = '\''; // enter single quote mode
                start = i + 1;
            }
            else if (arg[i] == '"') {
                if (i > start) {
                    char *chunk = ft_substr(arg, start, i - start);
                    if (!chunk)
                        return (free(result), NULL);
                    result = join_and_free(result,
                        expand_one_arg(chunk, head, last_status));
                }
                state = '"'; // enter double quote mode
                start = i + 1;
            }
        }
        else if (state == '\'') {
            if (arg[i] == '\'') {
                // inside single quotes → copy literally
                char *chunk = ft_substr(arg, start, i - start);
                if (!chunk)
                    return (free(result), NULL);
                result = join_and_free(result, chunk);
                state = 0;
                start = i + 1;
            }
        }
        else if (state == '"') {
            if (arg[i] == '"') {
                // inside double quotes → expand
                char *chunk = ft_substr(arg, start, i - start);
                if (!chunk)
                    return (free(result), NULL);
                result = join_and_free(result,
                    expand_one_arg(chunk, head, last_status));
                state = 0;
                start = i + 1;
            }
        }
        i++;
    }

    // flush last segment
    if (start < i) {
        char *chunk = ft_substr(arg, start, i - start);
        if (!chunk)
            return (free(result), NULL);
        if (state == 0)
            result = join_and_free(result,
                expand_one_arg(chunk, head, last_status));
        else if (state == '\'')
            result = join_and_free(result, chunk); // literal
        else if (state == '"')
            result = join_and_free(result,
                expand_one_arg(chunk, head, last_status));
    }

    if (state != 0) {
        // unterminated quote → error
        free(result);
        return NULL;
    }
    return result;
}*/
/*
char *expand_arg(const char *arg, t_env *head, int last_status)
{
    char state = 0;   // 0 = unquoted, ' = single, " = double
    int i = 0, start = 0;
    char *result = ft_strdup("");

    while (arg[i]) {
        if (state == 0) {
            if (arg[i] == '\'') {
                // expand everything before the quote
                if (i > start)
                    result = join_and_free(result,
                        expand_one_arg(ft_substr(arg, start, i - start), head, last_status));
                state = '\'';
                start = i + 1;
            }
            else if (arg[i] == '"') {
                if (i > start)
                    result = join_and_free(result,
                        expand_one_arg(ft_substr(arg, start, i - start), head, last_status));
                state = '"';
                start = i + 1;
            }
        }
        else if (state == '\'') {
            if (arg[i] == '\'') {
                // inside single quotes: copy literally
                result = join_and_free(result,
                    ft_substr(arg, start, i - start));
                state = 0;
                start = i + 1;
            }
        }
        else if (state == '"') {
            if (arg[i] == '"') {
                // inside double quotes: expand
                result = join_and_free(result,
                    expand_one_arg(ft_substr(arg, start, i - start), head, last_status));
                state = 0;
                start = i + 1;
            }
        }
        i++;
    }

    // flush last segment
    if (start < i) {
        if (state == 0)
            result = join_and_free(result,
                expand_one_arg(ft_substr(arg, start, i - start), head, last_status));
        else if (state == '\'')
            result = join_and_free(result,
                ft_substr(arg, start, i - start));
        else if (state == '"')
            result = join_and_free(result,
                expand_one_arg(ft_substr(arg, start, i - start), head, last_status));
    }
	if (state != 0)
	{
		// unterminated quote
		free(result);
		return NULL; // signal error
	}
    return result;
}*/
/*
char *expand_arg(const char *arg, t_env *head, int last_status)
{
    char state = 0;
    int i = 0, start = 0;
    char *result = ft_strdup("");

    while (arg[i]) {
        if (state == 0) {
            if (arg[i] == '\'') {
                // expand the part before the quote
                if (i > start)
                    result = str_join_free(result,
                        expand_one_arg(ft_substr(arg, start, i - start), head, last_status));

                state = '\'';
                start = i + 1;
            }
        } else if (state == '\'') {
            if (arg[i] == '\'') {
                // copy literal chunk (inside single quotes)
                result = str_join_free(result,
                        ft_substr(arg, start, i - start));
                state = 0;
                start = i + 1;
            }
        }
        i++;
    }

    // flush remainder (expand if outside quotes, literal if still inside)
    if (start < i) {
        if (state == 0)
            result = str_join_free(result,
                expand_one_arg(ft_substr(arg, start, i - start), head, last_status));
        else
            result = str_join_free(result,
                ft_substr(arg, start, i - start));
    }

    return result;
}

*/
// dollar_expansion:
//  - Called from: main execution loop 
//			(one command at a time)
//  - Purpose: expand $VAR and $? in all arguments 
//			of a command
//  - Calls: expand_one_arg, free
//  - Returns: void
//  - Memory: frees old argument strings and replaces with
//			expanded versions
/*
void	dollar_expansion(t_command *cmd, t_env **head, int last_status) // just sending 1 cmd at a time and assumes it wasnt in single quotes
{
	int		i;
	//int		j;
	char	*expanded;
	//size_t len;

	i = 1;
	//j = 0;
	while (cmd->args[i] != NULL)
	{
		//plug in checker here - might need to modify expand_one_arg a bit
		expanded = expand_arg(cmd->args[i], *head, last_status);
		if (!expanded)*/
		/*{
			while(j < i)
			{
				free(cmd->args[j]);
				j++;
			}*/
			/*return ;
		//}do we want to free already set expansions if one malloc fails?
		free(cmd->args[i]);
		cmd->args[i] = expanded;
		i++;
	}
	return ;
}
*/
char *expand_arg_keep_quotes(const char *arg, t_env *head, int last_status)
{
    int i = 0;
    char state = 0; // 0 = unquoted, ' = single, " = double
    char *result = ft_strdup("");
    if (!result)
        return NULL;

    while (arg[i])
    {
        if (arg[i] == '\'' && state == 0)
        {
            result = append_normal_text("'", result); // keep the quote
            state = '\'';
        }
        else if (arg[i] == '\'' && state == '\'')
        {
            result = append_normal_text("'", result); // keep the quote
            state = 0;
        }
        else if (arg[i] == '"' && state == 0)
        {
            result = append_normal_text("\"", result); // keep the quote
            state = '"';
        }
        else if (arg[i] == '"' && state == '"')
        {
            result = append_normal_text("\"", result); // keep the quote
            state = 0;
        }
        else if (arg[i] == '$' && state != '\'') // expand only outside single quotes
        {
            i++; // skip '$'
            if (arg[i] == '?')
            {
                result = append_result(head, NULL, result, last_status);
                i++;
            }
            else
            {
                int key_len;
                if (is_valid_key((char *)arg, i, &key_len))
                {
                    char *key = ft_substr(arg, i, key_len);
                    if (!key)
                        return free(result), NULL;
                    result = append_result(head, key, result, -1);
                    free(key);
                    i += key_len;
                }
                else
                {
                    result = append_result(head, "$", result, -1);
                }
            }
            continue;
        }
        else
        {
            char tmp[2] = {arg[i], '\0'};
            result = append_normal_text(tmp, result);
        }
        i++;
    }

    return result;
}

void dollar_expansion(t_command *cmd, t_env **head, int last_status)
{
    for (int i = 1; cmd->args[i]; i++)
    {
        char *expanded = expand_arg_keep_quotes(cmd->args[i], *head, last_status);
        if (!expanded)
            return;
        free(cmd->args[i]);
        cmd->args[i] = expanded;
    }
}

/*char *strip_quotes(const char *token)
{
    int i = 0, j = 0;
    char state = 0;
    char *clean = malloc(strlen(token) + 1);
    if (!clean) return NULL;

    while (token[i]) { //going thoruhg token looking for ' or " when we find it change 
        if (state == 0) {
            if (token[i] == '\'') state = '\'' clean[j++] = token[i];
            else if (token[i] == '"') state = '"'clean[j++] = token[i]; ;
            else clean[j++] = token[i]; //send to expander
        } else if (state == '\'') { //if state is ' then keep adding until find another and dont expand
            if (token[i] == '\'') state = 0;
            else clean[j++] = token[i];
			//dont send to expander
        } else if (state == '"') {
            if (token[i] == '"') state = 0;
            else clean[j++] = token[i]; //send to expander - building up piece by piece
        }
        i++;
    }
	  if (state != 0) {
		printf(">");
        // unclosed quote detected
        free(clean);
        return NULL; //let main know to do >
    clean[j] = '\0';
    return clean;
}
}

void trim_quotes_for_execution(char **tokens) //sendng each arg to strip quotes 
{
    for (int i = 0; tokens[i]; i++) {
        char *cleaned = strip_quotes(tokens[i]);
        free(tokens[i]);
        tokens[i] = cleaned;
    }
}*/
/*
| Approach           | Frees old args on failure? | Pros                      | Cons                            |
| ------------------ | -------------------------- | ------------------------- | ------------------------------- |
| Leave originals    | No                         | Original arguments remain | Partial expansion not reflected |
| Free replaced args | Yes                        | No memory leaks           | Previously expanded args lost   |
*/
/*| String                  | Allocated by     | Freed by                                      |
| ----------------------- | ---------------- | --------------------------------------------- |
| `temp->value`           | `t_env`          | Never (owned by env list)                     |
| `ft_itoa(last_status)`  | `append_result`  | Freed in `append_result`                      |
| `result`                | `expand_one_arg` | Replaced by `append_result` or freed on error |
| `key` in `set_key`      | `set_key`        | Freed after appending                         |
| `rest` in `append_rest` | `append_rest`    | Freed after appending                         |
*/