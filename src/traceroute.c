/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/18 13:46:00 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/19 22:22:12 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"
#include "../libft/incl/ping.h"

long double			time_diff_ms(struct timeval *then, struct timeval *now)
{
	long double x;

	x = (double)(then->tv_usec - now->tv_usec) / 1000.0L +
		(double)(then->tv_sec - now->tv_sec) * 1000.0L;
	return (x);
}

long double			time_diff_sec(struct timeval *then, struct timeval *now)
{
	return ((now->tv_sec + (1.0 / 1000000) * now->tv_usec) -
			(then->tv_sec + (1.0 / 1000000) * then->tv_usec));
}

float				get_percentage(size_t a, size_t b)
{
	size_t			diff;

	diff = a - b;
	if (diff == 0)
		return (0.00f);
	else
		return (((float)diff / a) * 100);
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

int						send_echo(t_mgr *mgr, u_int8_t *pkt, size_t pktlen, struct timeval *then)
{
	if (sendto(mgr->sock, pkt, pktlen, 0, (struct sockaddr *)&mgr->sin,
			   sizeof(struct sockaddr)) < 0)
	{
		dprintf(STDERR_FILENO, "Error sendto().\n");
		exit(FAILURE);
	}
	return (SUCCESS);
}

int						recv_echo(t_mgr *mgr, uint8_t *resp_buff, fd_set *readfds, struct timeval *now)
{
	struct timeval	timeout;
	socklen_t		socklen;

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	ft_memset(resp_buff, 0, IP_MAXPACKET);
	socklen = sizeof(struct sockaddr);
	if ((select(mgr->sock + 1, readfds, NULL, NULL, &timeout)) == 0)
	{
		printf("*");
		return (FAILURE);
	}
	if (recvfrom(mgr->sock,resp_buff,IP_MAXPACKET,0, (struct sockaddr *)&mgr->sin, &socklen) < 0)
	{
		dprintf(STDERR_FILENO, "Error recvfrom().\n");
		exit(FAILURE);
	}
	return (SUCCESS);
}

int					handle_response(uint8_t *resp_buff, struct timeval *then, struct timeval *now)
{
	char 					buff[INET_ADDRSTRLEN];
	struct in_addr			resp_addr;
	static struct in_addr	prev_resp_addr;

	resp_addr = ((struct ip*)resp_buff)->ip_src;
	if (prev_resp_addr.s_addr != resp_addr.s_addr)
	{
		inet_ntop(AF_INET, &resp_addr, buff, INET_ADDRSTRLEN);
		printf("\n%s", buff);
	}
	printf(" %.3f", (float)time_diff_ms(then, now));
	return (SUCCESS);
}

int					ping_loop(t_mgr *mgr, uint8_t *pkt, size_t pktlen)
{
	fd_set			readfds;
	struct timeval	then;
	struct timeval	now;

	u_int8_t 		resp_buff[IP_MAXPACKET];

	FD_ZERO(&readfds);
	FD_SET(mgr->sock, &readfds);
	while (mgr->flags.run == TRUE)
	{
		send_echo(mgr, pkt, pktlen, &then);
		if (recv_echo(mgr, resp_buff, &readfds, &now) == SUCCESS)
			handle_response(resp_buff, &then, &now);
	}
	return (SUCCESS);
}

int				traceroute(t_mgr *mgr)
{
	t_echopkt	msg;
	u_int8_t	pkt[IP_MAXPACKET];
	size_t		pktlen;

	ft_setip_hdr(&msg.iphdr, mgr->init_ttl,
				 mgr->flags.icmp ? IPPROTO_ICMP : IPPROTO_UDP, (int)ft_strlen(MSG_DATA));
	mgr->flags.icmp == TRUE ? ft_seticmp_hdr(&msg.phdr.icmp, 1, getpid()) :
	ft_setudp_hdr(&msg.phdr.udp, mgr->sin.sin_port, (int)ft_strlen(MSG_DATA));
	ft_memcpy(msg.data, MSG_DATA, ft_strlen(MSG_DATA));
	ft_setip_dstsrc(&msg.iphdr, NULL, &mgr->src);
	ft_memset(pkt, 0, IP_MAXPACKET);
	fill_packet(mgr, pkt, &msg);
	pktlen = IPV4_HDRLEN + DEF_HDRLEN + sizeof(msg.time) + msg.datalen;
	ping_loop(mgr, pkt, pktlen);
	//TODO: go into ping loop, limit timout with select(), manage output.
	return (SUCCESS);
}