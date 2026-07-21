/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   single_coder.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yel-azim <yel-azim@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 18:23:41 by yel-azim          #+#    #+#             */
/*   Updated: 2026/06/19 18:23:44 by yel-azim         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "codexion.h"

void	*single_coder_cycle(t_coder *coder)
{
	add_to_queue(coder, coder->left_dongle);
	wait_dongle(coder, coder->left_dongle);
	if (!get_is_over(coder->simulation))
		print_status(coder->simulation, coder->coder_id, "has taken a dongle");
	while (!get_is_over(coder->simulation))
		usleep(1000);
	return (NULL);
}
