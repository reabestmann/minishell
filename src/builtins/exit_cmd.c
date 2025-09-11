/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aabelkis <aabelkis@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/10 15:02:49 by aabelkis          #+#    #+#             */
/*   Updated: 2025/09/11 16:11:32 by aabelkis         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int exit_cmd(t_command *cmd)
{
    if (!cmd)
        return (1);
	//exit and clean up everything - think this one through 
    printf("exit is not available yet\n");
    return (0);
}