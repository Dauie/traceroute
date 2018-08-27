/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   send_recv_echo.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/25 16:46:29 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/25 16:46:29 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

int						send_echo(t_mgr *mgr, int8_t *pkt, size_t pktlen)
{
	if (sendto(mgr->send_sock, pkt, pktlen, 0,
			(struct sockaddr *)&mgr->to, sizeof(struct sockaddr)) < 0)
	{
		dprintf(STDERR_FILENO, "Error sendto().\n");
		exit(FAILURE);
	}
	return (SUCCESS);
}

int						recv_echo(t_mgr *mgr, t_echopkt *msg,
									int8_t *respbuff, fd_set *readfds)
{
	ssize_t				ret;
	struct timeval		timeout;
	socklen_t			socklen;

	timeout = (struct timeval){DEF_WAIT_TIME, 0};
	FD_ZERO(readfds);
	FD_SET((uint)mgr->recv_sock, readfds);
	socklen = sizeof(struct sockaddr);
	ret = select(mgr->recv_sock + 1, readfds, NULL, NULL, &timeout);
	if (ret <= 0)
	{
		printf("  *");
		return (FAILURE);
	}
	else if (ret > 0 && FD_ISSET((uint)mgr->recv_sock, readfds))
	{
		if (recvfrom(mgr->recv_sock, respbuff, IP_MAXPACKET, 0,
					 (struct sockaddr *)&mgr->from, &socklen) < 0)
		{
			dprintf(STDERR_FILENO, "Error recvfrom().\n");
			exit(FAILURE);
		}
		gettimeofday(&msg->recvd, NULL);
	}
	return (SUCCESS);
}