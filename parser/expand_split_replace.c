/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split_replace.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:54:54 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 13:54:55 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* replace_arg_with_splits:
**   Replaces cmd->args[idx] with multiple parts (from ft_split_whitespace),
**   preserving the other arguments. Frees the old argument being replaced.
*/

int	init_split_replace(t_split_replace *c, t_command *cmd, char **parts)
{
	c->old_args = cmd->args;
	c->old_len = 0;
	c->parts_len = 0;
	while (c->old_args && c->old_args[c->old_len])
		c->old_len++;
	while (parts && parts[c->parts_len])
		c->parts_len++;
	c->new_len = c->old_len - 1 + c->parts_len;
	if (c->new_len < 0)
		c->new_len = 0;
	c->new_args = malloc((c->new_len + 1) * sizeof(char *));
	if (!c->new_args)
		return (1);
	c->dst = 0;
	c->i = 0;
	return (0);
}

/* copy_before_index
**   Copies all arguments from cmd->args before index idx into new_args.
*/
void	copy_before_index(t_split_replace *c, int idx)
{
	while (c->i < idx && c->i < c->old_len)
		c->new_args[c->dst++] = c->old_args[c->i++];
}

/* insert_parts
**   Inserts the split parts into new_args at the replacement position.
*/
void	insert_parts(t_split_replace *c, char **parts)
{
	int	i;

	i = 0;
	while (i < c->parts_len)
		c->new_args[c->dst++] = parts[i++];
}

/* copy_after_index
**   Copies all arguments from cmd->args after index idx into new_args.
*/
void	copy_after_index(t_split_replace *c, int idx)
{
	c->i = idx + 1;
	while (c->i < c->old_len)
		c->new_args[c->dst++] = c->old_args[c->i++];
}

/* replace_arg_with_splits
**   Replaces cmd->args[idx] with multiple split parts.
**   Frees the original argument and old args array.
**   Returns 0 on success, 1 on allocation failure.
*/
int	replace_arg_with_splits(t_command *cmd, int idx, char **parts)
{
	t_split_replace	c;

	if (init_split_replace(&c, cmd, parts))
		return (1);
	copy_before_index(&c, idx);
	free(c.old_args[idx]);
	insert_parts(&c, parts);
	copy_after_index(&c, idx);
	c.new_args[c.dst] = NULL;
	free(c.old_args);
	cmd->args = c.new_args;
	return (0);
}
