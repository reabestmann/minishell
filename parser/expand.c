/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/13 16:13:32 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int is_ifs_char(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

static int count_words(const char *s)
{
    int count = 0, in_word = 0;

    while (*s)
    {
        if (is_ifs_char(*s))
            in_word = 0;
        else if (!in_word)
        {
            in_word = 1;
            count++;
        }
        s++;
    }
    return count;
}

static char *dup_word(const char *start, const char *end)
{
    size_t len = end - start;
    char *word = malloc(len + 1);
    if (!word)
        return NULL;
    ft_memcpy(word, start, len);
    word[len] = '\0';
    return word;
}

int args_len(char **args)
{
    int len = 0;
    while (args && args[len])
        len++;
    return len;
}

char **ft_split_whitespace(const char *s)
{
    char **result;
    int i = 0;
    const char *start;

    if (!s)
        return NULL;

    result = malloc((count_words(s) + 1) * sizeof(char *));
    if (!result)
        return NULL;

    while (*s)
    {
        while (*s && is_ifs_char(*s))
            s++;
        if (*s)
        {
            start = s;
            while (*s && !is_ifs_char(*s))
                s++;
            result[i++] = dup_word(start, s);
            if (!result[i - 1])
            {
                while (i > 0)
                    free(result[--i]);
                free(result);
                return NULL;
            }
        }
    }

    result[i] = NULL;
    return result;
}


int replace_arg_with_splits(t_command *cmd, int idx, char **parts)
{
    char **old_args = cmd->args;
    int old_len = 0;
    int parts_len = 0;
    int new_len;
    char **new_args;
    int i, dst;

    /* count old_len and parts_len safely */
    while (old_args && old_args[old_len])
        old_len++;
    while (parts && parts[parts_len])
        parts_len++;

    new_len = old_len - 1 + parts_len;
    if (new_len < 0)
        new_len = 0;

    new_args = malloc((new_len + 1) * sizeof(char *));
    if (!new_args)
        return 1;

    dst = 0;
    /* copy args before idx */
    for (i = 0; i < idx && i < old_len; i++)
        new_args[dst++] = old_args[i];

    /* free the old arg being replaced */
    free(old_args[idx]);

    /* copy the split parts (take ownership directly) */
    for (i = 0; i < parts_len; i++)
        new_args[dst++] = parts[i];

    /* copy args after idx */
    for (i = idx + 1; i < old_len; i++)
        new_args[dst++] = old_args[i];

    new_args[dst] = NULL;
    free(old_args);
    cmd->args = new_args;
    return 0;
}

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
char *expand_arg_keep_quotes(char *arg, t_env *head, int last_status, int *had_unquoted)
{
    int i = 0;
    char state = 0; // 0 = no quote, '\'' = single, '"' = double
    char *result = ft_strdup("");
    char *tmp;

    if (!result)
        return NULL;
    if (had_unquoted)
        *had_unquoted = 0;

    while (arg[i])
    {
        if (arg[i] == '\'' || arg[i] == '"')
        {
            // Toggle quote state but do NOT remove the quote yet
            if (state == 0)
                state = arg[i];
            else if (state == arg[i])
                state = 0;

            // Copy the quote literally
            char c[2] = {arg[i], '\0'};
            result = append_normal_text(c, result);
            i++;
        }
        else if (arg[i] == '$' && state != '\'')
        {
            // Expansion occurs
            tmp = expand_one_arg(arg, &i, head, last_status);
            if (!tmp)
            {
                free(result);
                return NULL;
            }

            // Only mark had_unquoted if this $ is **outside quotes**
            if (state == 0 && had_unquoted)
            {
                // Check if expansion contains whitespace
                if (ft_strchr(tmp, ' ') || ft_strchr(tmp, '\t') || ft_strchr(tmp, '\n'))
                    *had_unquoted = 1;
            }

            result = append_normal_text(tmp, result);
            free(tmp);
        }
        else
        {
            // Normal character, copy literally
            char c[2] = {arg[i], '\0'};
            result = append_normal_text(c, result);
            i++;
        }
    }

    return result;
}


/*dollar_expansion:
- Applies expansion to all command arguments
- Frees old argument strings and replaces with expanded versions*/
void dollar_expansion(t_command *cmd, t_env **head, int last_status)
{
    int i = 0;
    char *expanded;
    char **parts;
    int had_unquoted;

    while (cmd->args && cmd->args[i])
    {
        had_unquoted = 0;
        expanded = expand_arg_keep_quotes(cmd->args[i], *head, last_status, &had_unquoted);
        if (!expanded)
        {
            i++;
            continue;
        }

        // Only split if there are unquoted spaces (IFS chars) outside quotes
        if (had_unquoted && (ft_strchr(expanded, ' ') 
                         || ft_strchr(expanded, '\t') 
                         || ft_strchr(expanded, '\n')))
        {
            parts = ft_split_whitespace(expanded);
            free(expanded);
            if (!parts)
                return;

            int added = args_len(parts);
            if (replace_arg_with_splits(cmd, i, parts) != 0)
            {
                free(parts);
                return;
            }
            free(parts);  // free container only
            i += added;
        }
        else
        {
            free(cmd->args[i]);
            cmd->args[i] = expanded;
            i++;
        }
    }
}
