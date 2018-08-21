/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/18 13:46:00 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/21 13:07:03 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"
#include "../libft/incl/ping.h"

long double				time_diff_ms(struct timeval *then, struct timeval *now)
{
	long double x;

	x = (double)(then->tv_usec - now->tv_usec) / 1000.0L +
		(double)(then->tv_sec - now->tv_sec) * 1000.0L;
	return (x);
}

long double				time_diff_sec(struct timeval *then, struct timeval *now)
{
	return ((now->tv_sec + (1.0 / 1000000) * now->tv_usec) -
			(then->tv_sec + (1.0 / 1000000) * then->tv_usec));
}

float					get_percentage(size_t a, size_t b)
{
	size_t				diff;

	diff = a - b;
	if (diff == 0)
		return (0.00f);
	else
		return (((float)diff / a) * 100);
}

static void				fill_packet(t_mgr *mgr, t_echopkt *msg, int8_t *packet)
{
	ft_memcpy(packet, &msg->iphdr, IPV4_HDRLEN);
	packet += IPV4_HDRLEN;
	if (mgr->flags.icmp == TRUE)
		ft_memcpy(packet , &msg->phdr.icmp, ICMP_HDRLEN);
	else
		ft_memcpy(packet, &msg->phdr.udp, UDP_HDRLEN);
	packet += DEF_HDRLEN;
	gettimeofday(&msg->sent, NULL);
	ft_memcpy(packet , &msg->sent, sizeof(struct timeval));
	packet += sizeof(struct timeval);
	ft_memcpy(packet , msg->data, msg->datalen);
	/*TODO: Add checksum if icmp*/
}

int						send_echo(t_mgr *mgr, int8_t *pkt, size_t pktlen)
{
	if (sendto(mgr->sock, pkt, pktlen, 0, (struct sockaddr *)&mgr->to, sizeof(struct sockaddr)) < 0)
	{
		dprintf(STDERR_FILENO, "Error sendto().\n");
		exit(FAILURE);
	}
	return (SUCCESS);
}

int						recv_echo(t_mgr *mgr, t_echopkt *msg, int8_t *resp_buff, fd_set *readfds)
{
	int					ret;
	struct timeval		timeout;
	socklen_t			socklen;

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	socklen = sizeof(struct sockaddr);
	ret = select(mgr->sock + 1, readfds, NULL, NULL, &timeout);
	if (ret < 0)
	{
		printf("*");
		return (FAILURE);
	}
	else if (ret > 0 && FD_ISSET(mgr->sock, readfds))
	{
		if (recvfrom(mgr->sock, resp_buff, IP_MAXPACKET, 0,  (struct sockaddr *)&mgr->from, &socklen) < 0)
		{
			dprintf(STDERR_FILENO, "Error recvfrom().\n");
			exit(FAILURE);
		}
		gettimeofday(&msg->recvd, NULL);
	}
	return (SUCCESS);
}

int							handle_response(const int8_t *resp_buff, t_echopkt *msg)
{
	char 					ipstr[INET_ADDRSTRLEN];
	struct in_addr			resp_addr;
	static struct in_addr	prev_resp_addr;

	resp_addr = ((struct ip*)resp_buff)->ip_src;
	if (prev_resp_addr.s_addr != resp_addr.s_addr)
	{
		inet_ntop(AF_INET, &resp_addr, ipstr, INET_ADDRSTRLEN);
		printf("\n%s", ipstr);
	}
	printf(" %.3f", (float)time_diff_ms(&msg->sent, &msg->recvd));
	prev_resp_addr = resp_addr;
	return (SUCCESS);
}

int					ping_loop(t_mgr *mgr, t_echopkt *msg, int8_t *pkt, size_t pktlen)
{
	fd_set			readfds;
	int8_t 			resp_buff[IP_MAXPACKET];

	FD_ZERO(&readfds);
	FD_SET(mgr->sock, &readfds);
	printf("Starting ping loop: \n");
	while (mgr->flags.run == TRUE)
	{
		send_echo(mgr, pkt, pktlen);
		ft_memset(resp_buff, 0, IP_MAXPACKET);
		if (recv_echo(mgr, msg, resp_buff, &readfds ) == SUCCESS)
			handle_response(resp_buff, msg);
	}
	return (SUCCESS);
}

int 			initialize_echopacket(t_mgr *mgr, t_echopkt *msg)
{
	if (!(msg->data = (uint8_t *)ft_strdup(MSG_DATA)))
		return (FAILURE);
	msg->datalen = (u_short)ft_strlen(MSG_DATA);
	ft_setip_hdr(&msg->iphdr, mgr->init_ttl,
				 mgr->flags.icmp ? IPPROTO_ICMP : IPPROTO_UDP, msg->datalen);
	mgr->flags.icmp == TRUE ? ft_seticmp_hdr(&msg->phdr.icmp, 1, getpid()) :
	ft_setudp_hdr(&msg->phdr.udp, mgr->to.sin_port, msg->datalen);
	ft_setip_dstsrc(&msg->iphdr,  &mgr->from.sin_addr, &mgr->to.sin_addr);
	return (SUCCESS);
}

int				traceroute(t_mgr *mgr)
{
	int8_t		pkt[IP_MAXPACKET];
	size_t		pktlen;
	t_echopkt	msg;

	printf("traceroute initialization: ");
	ft_memset(pkt, 0, IP_MAXPACKET);
	ft_memset(&msg, 0, sizeof(t_echopkt));
	initialize_echopacket(mgr, &msg);
	fill_packet(mgr, &msg, pkt);
	pktlen = IPV4_HDRLEN + DEF_HDRLEN + sizeof(struct timeval) + msg.datalen;
	printf("done.\n");
	ping_loop(mgr, &msg, pkt, pktlen);
	free(msg.data);
	return (SUCCESS);
}