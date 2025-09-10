/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:03:05 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/10 20:31:34 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int unset_cmd(t_command *cmd, t_env **env)
{
    if (!cmd || !env)
        return (1);
    printf("unset is not available yet\n");
    return (0);
}