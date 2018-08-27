/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/18 13:46:00 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/25 17:58:54 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

static void			fill_packet(t_mgr *mgr, t_echopkt *msg, int8_t *packet)
{
	ft_memcpy(packet, &msg->iphdr, IPV4_HDRLEN);
	packet += IPV4_HDRLEN;
	if (mgr->flags.icmp == TRUE)
		ft_memcpy(packet, &msg->phdr.icmp, ICMP_HDRLEN);
	else
		ft_memcpy(packet, &msg->phdr.udp, UDP_HDRLEN);
	packet += DEF_HDRLEN;
	gettimeofday(&msg->sent, NULL);
	ft_memcpy(packet, &msg->sent, sizeof(struct timeval));
	packet += sizeof(struct timeval);
	ft_memcpy(packet, msg->data, msg->datalen);
}

static void			update_echopkt(t_mgr *mgr, t_echopkt *msg)
{
	ft_setip_hdr(&msg->iphdr, mgr->ttl, mgr->flags.icmp == TRUE ?
				IPPROTO_ICMP : IPPROTO_UDP, msg->datalen);
	mgr->flags.icmp == TRUE ?
	ft_seticmp_hdr(&msg->phdr.icmp, ICMP_ECHO, mgr->ttl, mgr->pid) :
	ft_setudp_hdr(&msg->phdr.udp, mgr->pid + mgr->ttl,
				mgr->udp_port, msg->datalen);
}

static int			ping_loop(t_mgr *mgr, t_echopkt *msg,
								int8_t *pkt, size_t pktlen)
{
	fd_set			readfds;
	int8_t			resp_buff[IP_MAXPACKET];
	int				probe;

	while (mgr->flags.run == TRUE && mgr->ttl <= mgr->max_ttl)
	{
		probe = 0;
		printf(mgr->ttl <= 9 ? " %d " : "%d ", mgr->ttl);
		while (probe++ < mgr->nprobes)
		{
			fill_packet(mgr, msg, pkt);
			send_echo(mgr, pkt, pktlen);
			ft_memset(resp_buff, 0, IP_MAXPACKET);
			recv_echo(mgr, msg, resp_buff, &readfds);
			handle_response(mgr, resp_buff, msg, probe);
			fflush(stdout);
		}
		printf("\n");
		mgr->ttl++;
		update_echopkt(mgr, msg);
		if (((struct ip *)resp_buff)->ip_src.s_addr == mgr->to.sin_addr.s_addr)
			mgr->flags.run = FALSE;
	}
	return (SUCCESS);
}

static int			initialize_echopacket(t_mgr *mgr, t_echopkt *msg)
{
	if (!(msg->data = (uint8_t *)ft_strdup(MSG_DATA)))
		return (FAILURE);
	msg->datalen = (u_short)ft_strlen(MSG_DATA) + sizeof(struct timeval);
	update_echopkt(mgr, msg);
	ft_setip_dstsrc(&msg->iphdr, &mgr->from.sin_addr, &mgr->to.sin_addr);
	return (SUCCESS);
}

int					traceroute(t_mgr *mgr)
{
	int8_t			pkt[IP_MAXPACKET];
	size_t			pktlen;
	t_echopkt		msg;

	ft_memset(pkt, 0, IP_MAXPACKET);
	ft_memset(&msg, 0, sizeof(t_echopkt));
	if (initialize_echopacket(mgr, &msg) == FAILURE)
		return (FAILURE);
	pktlen = IPV4_HDRLEN + DEF_HDRLEN + msg.datalen;
	printf("traceroute to %s (%s), %d hops max, %zu byte %s packets\n",
		mgr->domain, inet_ntoa(mgr->to.sin_addr),
		mgr->max_ttl, pktlen, mgr->flags.icmp == TRUE ? "icmp" : "udp");
	ping_loop(mgr, &msg, pkt, pktlen);
	free(msg.data);
	return (SUCCESS);
}
