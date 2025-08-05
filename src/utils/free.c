/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbestman <rbestman@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 15:10:03 by rbestman          #+#    #+#             */
/*   Updated: 2025/08/03 15:13:02 by rbestman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	free_array(char **array)
{
	int	i;

	i = -1;
	while (array[++i])
		free(array[i]);
	free(array);
}
