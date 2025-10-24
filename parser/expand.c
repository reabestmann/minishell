/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/23 21:48:59 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* is_ifs_char:
**   Checks if a character is considered whitespace (space, tab, newline).
*/
static int	is_ifs_char(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

/* count_words:
**   Counts the number of words in a string, separated by IFS characters.
*/
static int	count_words(const char *s)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
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
	return (count);
}

/* dup_word:
**   Duplicates a substring given start and end pointers.
*/
static char	*dup_word(const char *start, const char *end)
{
	size_t	len;
	char	*word;

	len = end - start;
	word = malloc(len + 1);
	if (!word)
		return (NULL);
	ft_memcpy(word, start, len);
	word[len] = '\0';
	return (word);
}

/* args_len:
**   Returns the length of a NULL-terminated argument array.
*/
int	args_len(char **args)
{
	int	len;

	len = 0;
	while (args && args[len])
		len++;
	return (len);
}

/* ft_split_whitespace:
**   Splits a string into words based on whitespace (IFS).
**   Returns a NULL-terminated array of strings.
*/
char	**ft_split_whitespace(const char *s)
{
	char		**result;
	int			i;
	const char	*start;

	i = 0;
	if (!s)
		return (NULL);
	result = malloc((count_words(s) + 1) * sizeof(char *));
	if (!result)
		return (NULL);
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
				return (NULL);
			}
		}
	}
	result[i] = NULL;
	return (result);
}

/* replace_arg_with_splits:
**   Replaces cmd->args[idx] with multiple parts (from ft_split_whitespace),
**   preserving the other arguments. Frees the old argument being replaced.
*/

int	replace_arg_with_splits(t_command *cmd, int idx, char **parts)
{
	t_split_replace	c;

	c.old_args = cmd->args;
	c.old_len = 0;
	c.parts_len = 0;
	while (c.old_args && c.old_args[c.old_len])
		c.old_len++;
	while (parts && parts[c.parts_len])
		c.parts_len++;
	c.new_len = c.old_len - 1 + c.parts_len;
	if (c.new_len < 0)
		c.new_len = 0;
	c.new_args = malloc((c.new_len + 1) * sizeof(char *));
	if (!c.new_args)
		return (1);
	c.dst = 0;
	c.i = 0;
	while (c.i < idx && c.i < c.old_len)
	{
		c.new_args[c.dst++] = c.old_args[c.i];
		c.i++;
	}
	free(c.old_args[idx]);
	c.i = 0;
	while (c.i < c.parts_len)
	{
		c.new_args[c.dst++] = parts[c.i];
		c.i++;
	}
	c.i = idx + 1;
	while (c.i < c.old_len)
	{
		c.new_args[c.dst++] = c.old_args[c.i];
		c.i++;
	}
	c.new_args[c.dst] = NULL;
	free(c.old_args);
	cmd->args = c.new_args;
	return (0);
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
		{
			tmp = append_normal_text("", result);
			if (!tmp)
				return (free(result), NULL);
		}
		result = tmp;
	}
	return (result);
}

/*OG*/
/*char	*expand_one_arg(char *arg, int *i, t_env *head, int last_status)
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
}*/

/*handle_normal_txt:
- Copies literal characters until a special character ($, ', ")
- Uses append_normal_text*/
/* handle_normal_txt:
**   Appends literal text from arg starting at *i until a special character
**   ($, single or double quote) is found.
*/
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
char	*expand_arg_keep_quotes(char *arg, t_env *head,
	int last_status, int *had_unquoted)
{
	int		i;
	char	state;
	char	*result;
	char	*tmp;
	char	c[2];

	i = 0;
	state = 0;
	result = ft_strdup("");
	if (!result)
		return (NULL);
	if (had_unquoted)
		*had_unquoted = 0;
	while (arg[i])
	{
		if (arg[i] == '\'' || arg[i] == '"')
		{
			if (state == 0)
				state = arg[i];
			else if (state == arg[i])
				state = 0;
			c[0] = arg[i];
			c[1] = '\0';
			result = append_normal_text(c, result);
			i++;
		}
		else if (arg[i] == '$' && state != '\'')
		{
			tmp = expand_one_arg(arg, &i, head, last_status);
			if (!tmp)
			{
				free(result);
				return (NULL);
			}
			if (state == 0 && had_unquoted)
			{
				if (ft_strchr(tmp, ' ') || ft_strchr(tmp, '\t')
					|| ft_strchr(tmp, '\n'))
					*had_unquoted = 1;
			}
			result = append_normal_text(tmp, result);
			free(tmp);
		}
		else
		{
			c[0] = arg[i];
			c[1] = '\0';
			result = append_normal_text(c, result);
			i++;
		}
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
	char	**parts;
	int		had_unquoted;
	int		added;

	i = 0;
	while (cmd->args && cmd->args[i])
	{
		had_unquoted = 0;
		expanded = expand_arg_keep_quotes(cmd->args[i], *head,
				last_status, &had_unquoted);
		if (!expanded)
		{
			i++;
			continue ;
		}
		if (had_unquoted && (ft_strchr(expanded, ' ')
				|| ft_strchr(expanded, '\t')
				|| ft_strchr(expanded, '\n')))
		{
			parts = ft_split_whitespace(expanded);
			free(expanded);
			if (!parts)
				return ;
			added = args_len(parts);
			if (replace_arg_with_splits(cmd, i, parts) != 0)
			{
				free(parts);
				return ;
			}
			free(parts);
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
