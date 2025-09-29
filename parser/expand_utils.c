/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 23:09:15 by rbestman          #+#    #+#             */
/*   Updated: 2025/09/26 14:11:07 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*append_normal_text:
- Appends literal text to result using set_result
- Frees old result
- Always used for literal characters including quotes*/
char	*append_normal_text(char *text, char *result)
{
	char	*new_result;

	if (!text)
		return (result);
	if (!result)
		return (ft_strdup(text));
	new_result = ft_strjoin(result, text);
	free(result);
	return (new_result);
}

/* find_env_value:
- Purpose: Return the value of an environment variable by key
- Returns: Duplicated value string if found, "" if not
- Memory: Caller must free the returned string*/
char	*find_env_value(t_env *head, char *key)
{
	t_env	*temp;

	temp = head;
	while (temp)
	{
		if (str_equals(temp->key, key))
			return (ft_strdup(temp->value));
		temp = temp->next;
	}
	return (ft_strdup(""));
}

/* append_result:
- Purpose: Append env value or last_status to result
- Memory: Frees old result and temporary value if dynamically allocated
- Notes: last_status == -1 means use env value, otherwise append last_status 
	as string
*/
char	*append_result(t_env *head, char *key, char *result, int last_status)
{
	char	*val;

	if (last_status == -1)
		val = find_env_value(head, key);
	else
		val = ft_itoa(last_status);
	if (!val)
	{
		free(val);
		return (NULL);
	}
	result = append_normal_text(val, result);
	free(val);
	return (result);
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

/* append_key_value:
 - Purpose: Append the value of $VAR to result
 - Calls: is_valid_key, malloc, ft_strlcpy, append_result, free
 - Returns: New result string or NULL on malloc failure
 - Allocates memory for key, frees it after use
*/
char	*append_key_value(t_env *head, char *arg, int *i, char *result)
{
	char	*key;
	int		key_len;

	if (!is_valid_key(arg, *i, &key_len))
		return (append_normal_text("$", result));
	key = handle_malloc(key_len + 1);
	ft_strlcpy(key, arg + *i, key_len + 1);
	result = append_result(head, key, result, -1);
	free(key);
	*i += key_len;
	return (result);
}
