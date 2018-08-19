/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/18 13:46:00 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/19 01:31:11 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"
#include "../libft/incl/ping.h"

int				traceroute(t_mgr *mgr)
{
	t_echopkt	msg;

	ft_setip_hdr(&msg.iphdr, mgr->init_ttl,
		mgr->flags.icmp ? IPPROTO_ICMP : IPPROTO_UDP, (int)ft_strlen(MSG_DATA));
	mgr->flags.icmp == TRUE ? ft_seticmp_hdr(&msg.phdr.icmp, 1, getpid()) :
	ft_setudp_hdr(&msg.phdr.udp, mgr->sin.sin_port, (int)ft_strlen(MSG_DATA));
	//TODO: go into ping loop, limit timout with select(), manage output.
	return (SUCCESS);
}

static void				fill_packet(t_mgr *mgr, u_int8_t *packet, t_echopkt *msg)
{
	ft_memcpy(packet, &msg->iphdr, IPV4_HDRLEN);
	packet += IPV4_HDRLEN;
	if (mgr->flags.icmp == TRUE)
		ft_memcpy(packet , &msg->phdr.icmp, ICMP_HDRLEN);
	else
		ft_memcpy(packet, &msg->phdr.udp, UDP_HDRLEN);
	packet += DEF_HDRLEN;
	gettimeofday(&msg->time, NULL);
	ft_memcpy(packet , &msg->time, sizeof(struct timeval));
	packet += sizeof(struct timeval);
	ft_memcpy(packet , msg->data, msg->datalen);
	/*TODO: Add checksum if icmp*/
}

int						send_ping(t_mgr *mgr, t_echopkt *echo)
{
	u_int8_t			packet[IP_MAXPACKET];
	size_t				pktlen;

	ft_memset(packet, 0, IP_MAXPACKET);
	fill_packet(mgr, packet, echo);
	pktlen = IPV4_HDRLEN + DEF_HDRLEN + sizeof(echo->time) + echo->datalen;
	if (sendto(mgr->sock, packet, pktlen, 0, (struct sockaddr *)&mgr->sin,
			   sizeof(struct sockaddr)) < 0)
	{
		dprintf(STDERR_FILENO, "Error sendto().\n");
		exit(FAILURE);
	}
	return (SUCCESS);
}

int					ping_loop(t_mgr *mgr, t_echopkt *msg)
{
	struct timeval	then;
	struct timeval	now;

	gettimeofday(&then, NULL);
	while (mgr->flags.run == TRUE)
	{
		send_ping(mgr, echo);
		recv_ping(mgr, &now);
	}
	return (SUCCESS);
}