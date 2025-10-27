/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_heredoc1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 21:29:48 by aabelkis          #+#    #+#             */
/*   Updated: 2025/10/23 16:46:50 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* delimiter_was_quoted:
   Checks if a heredoc delimiter is quoted.
   - Returns 1 if delimiter starts/ends with matching quotes.
   - Used to decide whether expansion is disabled in heredoc.
*/
int	delimiter_was_quoted(const char *delimiter)
{
	int	s_quote;
	int	d_quote;

	s_quote = 0;
	d_quote = 0;
	while (*delimiter)
	{
		if (*delimiter == '\'')
			s_quote++;
		if (*delimiter == '"')
			d_quote++;
		delimiter++;
	}
	if ((s_quote != 0 && (s_quote % 2 == 0))
		|| (d_quote != 0 && (d_quote % 2 == 0)))
		return (1);
	return (0);
}

void	add_heredoc(t_command *cmd, const char *delimiter)
{
	char	**arr;
	int		i;
	int		j;

	i = 0;
	while (cmd->heredoc_delim && cmd->heredoc_delim[i])
		i++;
	arr = handle_malloc(sizeof(char *) * (i + 2));
	j = 0;
	while (j < i)
	{
		arr[j] = ft_strdup(cmd->heredoc_delim[j]);
		j++;
	}
	arr[i] = ft_strdup(delimiter);
	arr[i + 1] = NULL;
	if (cmd->heredoc_delim)
		free_array(cmd->heredoc_delim);
	cmd->heredoc_delim = arr;
	cmd->heredoc_count = i + 1;
}
