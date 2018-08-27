/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useage.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/27 13:44:48 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/27 13:45:21 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

void			useage(void)
{
	printf("Usage:\ttraceroute [-fmpsiIh] destination\n"
				   "\t[-f first-ttl][-m max-ttl][-p udp port]\n"
				   "\t[-s ip address][-i interface address]\n"
				   "\t[-I ICMP instead of UDP][-q probe amount]\n"
				   "\t[-h help]\n");
	exit(SUCCESS);
}
