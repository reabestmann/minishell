/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 17:34:46 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:55:31 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/*Dollar Expansion Overview:
- Processes all arguments in cmd->args for $VAR and $? expansions.
- Preserves quotes in the original string.
- Handles splitting expanded values into multiple arguments if needed.


split_and_replace:
- Splits expanded string by whitespace (IFS) and replaces the original arg.
- Frees the old argument and temporary split array.
- Updates index to account for added arguments.*/
static int	split_and_replace(t_command *cmd, int *i, char *expanded)
{
	char	**parts;
	int		added;

	parts = ft_split_whitespace(expanded);
	free(expanded);
	if (!parts)
		return (-1);
	added = args_len(parts);
	if (replace_arg_with_splits(cmd, *i, parts) != 0)
	{
		free(parts);
		return (-1);
	}
	free(parts);
	*i += added;
	return (added);
}

/*
replace_single_arg:
- Replaces cmd->args[i] with a single expanded string.*/
static void	replace_single_arg(t_command *cmd, int *i, char *expanded)
{
	free(cmd->args[*i]);
	cmd->args[*i] = expanded;
	(*i)++;
}

/*handle_empty_first_arg:
- Handles edge case: first argument expands to empty and no other args exist.
- Frees the command array and marks expand_empty.*/
static int	handle_empty_first_arg(t_command *cmd, int i, char *exp)
{
	if (i == 0 && exp && exp[0] == '\0' && !cmd->args[1])
	{
		free_array(cmd->args);
		cmd->args = NULL;
		cmd->expand_empty = 1;
		free(exp);
		return (1);
	}
	return (0);
}

/*process_expansion_arg:
- Expands a single cmd argument ($VAR, $?) respecting quotes.
- Splits argument if unquoted whitespace is present, else replaces in-place.
- Returns 1 normally, 0 if first arg becomes empty.
*/
static int	process_expansion_arg(t_command *cmd, t_env **head,
		int last_status, int *i)
{
	int		unq;
	char	*exp;
	int		added;

	unq = 0;
	exp = expand_arg_keep_quotes(cmd->args[*i], *head, last_status, &unq);
	if (!exp)
	{
		(*i)++;
		return (1);
	}
	if (handle_empty_first_arg(cmd, *i, exp))
		return (0);
	if (unq && (ft_strchr(exp, ' ') || ft_strchr(exp, '\t')
			|| ft_strchr(exp, '\n')))
	{
		added = split_and_replace(cmd, i, exp);
		if (added == -1)
			return (0);
	}
	else
		replace_single_arg(cmd, i, exp);
	return (1);
}

/*dollar_expansion:
- Iterates through all arguments in the command.
- Expands each argument using process_expansion_arg.
- Stops early if expansion fails for an argument.
terates over cmd arguments and expands $VAR/$? for each.
- Handles splitting on unquoted whitespace and keeps quotes as needed.*/
void	dollar_expansion(t_command *cmd, t_env **head, int last_status)
{
	int	i;

	i = 0;
	while (cmd->args && cmd->args[i])
	{
		if (!process_expansion_arg(cmd, head, last_status, &i))
			return ;
	}
}
