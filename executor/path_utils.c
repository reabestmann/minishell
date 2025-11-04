/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:27:36 by aabelkis          #+#    #+#             */
/*   Updated: 2025/11/04 14:16:57 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

/* * is_special_dir:
 *  Returns 1 if arg is ".", "..", or "~"; otherwise 0.*/
int	is_special_dir(char *arg)
{
	return ((ft_strncmp(arg, ".", 2) == 0)
		|| (ft_strncmp(arg, "..", 3) == 0)
		|| (ft_strncmp(arg, "~", 2) == 0));
}
