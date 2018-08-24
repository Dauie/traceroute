/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/18 13:46:00 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/23 17:21:52 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

#define HEXDUMP_COLS 6

long double				time_diff_ms(struct timeval *then, struct timeval *now)
{
	long double x;

	x = (double)(then->tv_sec - now->tv_sec) * 1000.0L +
		(double)(then->tv_usec - now->tv_usec) / 1000.0L;
	return (x);
}

long double				time_diff_sec(struct timeval *then, struct timeval *now)
{
	return ((now->tv_sec - (1.0 / 1000000) * now->tv_usec) -
			(then->tv_sec + (1.0 / 1000000) * then->tv_usec));
}

void hexdump(void *mem, unsigned int len)
{
	unsigned int i, j;

	for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if(i % HEXDUMP_COLS == 0)
		{
			ft_printf("0x%06x: ", i);
		}

		/* print hex data */
		if(i < len)
		{
			ft_printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			ft_printf("   ");
		}

		/* print ASCII dump */
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if(j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if(ft_isprint(((char*)mem)[j])) /* printable char */
				{
					ft_putchar(0xFF & ((char*)mem)[j]);
				}
				else /* other char */
				{
					ft_putchar('.');
				}
			}
			ft_printf("\n");
		}
	}
	ft_printf("\n");
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
	if (sendto(mgr->send_sock, pkt, pktlen, 0, (struct sockaddr *)&mgr->to, sizeof(struct sockaddr)) < 0)
	{
		dprintf(STDERR_FILENO, "Error sendto().\n");
		exit(FAILURE);
	}
	return (SUCCESS);
}

int						recv_echo(t_mgr *mgr, t_echopkt *msg, int8_t *resp_buff, fd_set *readfds)
{
	ssize_t				ret;
	struct timeval		timeout;
	socklen_t			socklen;

	timeout.tv_sec = DEF_WAIT_TIME;
	timeout.tv_usec = 0;
	FD_ZERO(readfds);
	FD_SET(mgr->recv_sock, readfds);
	socklen = sizeof(struct sockaddr);
	ret = select(mgr->recv_sock + 1, readfds, NULL, NULL, &timeout);
	if (ret <= 0)
	{
		printf("*");
		return (FAILURE);
	}
	else if (ret > 0 && FD_ISSET(mgr->recv_sock, readfds))
	{
		if (recvfrom(mgr->recv_sock, resp_buff, IP_MAXPACKET, 0,  (struct sockaddr *)&mgr->from, &socklen) < 0)
		{
			dprintf(STDERR_FILENO, "Error recvfrom().\n");
			exit(FAILURE);
		}
		gettimeofday(&msg->recvd, NULL);
	}
	return (SUCCESS);
}

int							icmppkt_check(int8_t *resp_buff, int pid, int seq)
{
	struct icmp				*icmp;

	icmp = (struct icmp *)resp_buff + IPV4_HDRLEN;
	if (icmp->icmp_hun.ih_idseq.icd_id == htons(pid) &&
		icmp->icmp_hun.ih_idseq.icd_seq == htons(seq))
		return (SUCCESS);
	return (FAILURE);
}

int							udppkt_check(int8_t *resp_buff, int udp_port, int pid, int seq)
{
	struct udphdr			*udp;

	udp = (struct udphdr *)(resp_buff + IPV4_HDRLEN + ICMP_HDRLEN + IPV4_HDRLEN);
	if (udp->uh_dport == htons(udp_port) && udp->uh_sport == htons(pid + seq))
		return (SUCCESS);
	return (FAILURE);
}

int							check_packet(t_mgr *mgr, int8_t *resp_buff)
{
	//hexdump(resp_buff,  72);
	if (mgr->flags.icmp == TRUE)
		return (icmppkt_check(resp_buff, mgr->pid, mgr->ttl));
	else
		return (udppkt_check(resp_buff, mgr->udp_port, mgr->pid, mgr->ttl));
}

int							handle_response(t_mgr *mgr, int8_t *resp_buff, t_echopkt *msg, int probe)
{
	struct in_addr			resp_addr;
	static struct in_addr	prev_resp_addr;

	if (check_packet(mgr, resp_buff) == FAILURE)
		return (FAILURE);
	resp_addr = ((struct ip*)resp_buff)->ip_src;
	if (prev_resp_addr.s_addr != resp_addr.s_addr)
		printf("(%s)", inet_ntoa(resp_addr));
	printf("  %.3f ms", (float)time_diff_ms(&msg->recvd, &msg->sent));
	prev_resp_addr = resp_addr;
	if (probe  == mgr->nprobes)
		prev_resp_addr.s_addr = 0;
	return (SUCCESS);
}

void						update_echopkt(t_mgr *mgr, t_echopkt *msg)
{
	ft_setip_hdr(&msg->iphdr, mgr->ttl,
				 mgr->flags.icmp == TRUE ? IPPROTO_ICMP : IPPROTO_UDP, msg->datalen);
	mgr->flags.icmp == TRUE ?
	ft_seticmp_hdr(&msg->phdr.icmp, ICMP_ECHO, mgr->ttl, mgr->pid) :
	ft_setudp_hdr(&msg->phdr.udp, mgr->pid + mgr->ttl, mgr->udp_port, msg->datalen);
}

int					ping_loop(t_mgr *mgr, t_echopkt *msg, int8_t *pkt, size_t pktlen)
{
	fd_set			readfds;
	int8_t 			resp_buff[IP_MAXPACKET];
	int 			probe;

	probe = 0;
	while (mgr->flags.run == TRUE && mgr->ttl < mgr->max_ttl)
	{
		if (((struct ip *)resp_buff)->ip_src.s_addr == mgr->to.sin_addr.s_addr)
			break;
		printf(mgr->ttl <= 9 ? " %d  " : "%d  ", mgr->ttl);
		while (probe < mgr->nprobes)
		{
			fill_packet(mgr, msg, pkt);
			send_echo(mgr, pkt, pktlen);
			ft_memset(resp_buff, 0, IP_MAXPACKET);
			if (recv_echo(mgr, msg, resp_buff, &readfds) == SUCCESS)
				handle_response(mgr, resp_buff, msg, probe + 1);
			if (++probe == mgr->nprobes)
				printf("\n");
			fflush(stdout);
		}
		probe = 0;
		mgr->ttl++;
		update_echopkt(mgr, msg);
		if ((msg->iphdr.ip_ttl = (unsigned char)mgr->ttl) >= mgr->max_ttl)
			mgr->flags.run = FALSE;
	}
	return (SUCCESS);
}

int 			initialize_echopacket(t_mgr *mgr, t_echopkt *msg)
{
	if (!(msg->data = (uint8_t *)ft_strdup(MSG_DATA)))
		return (FAILURE);
	msg->datalen = (u_short)ft_strlen(MSG_DATA) + sizeof(struct timeval);
	update_echopkt(mgr, msg);
	ft_setip_dstsrc(&msg->iphdr,  &mgr->from.sin_addr, &mgr->to.sin_addr);
	return (SUCCESS);
}

int				traceroute(t_mgr *mgr)
{
	int8_t		pkt[IP_MAXPACKET];
	size_t		pktlen;
	t_echopkt	msg;

	ft_memset(pkt, 0, IP_MAXPACKET);
	ft_memset(&msg, 0, sizeof(t_echopkt));
	initialize_echopacket(mgr, &msg);
	pktlen = IPV4_HDRLEN + DEF_HDRLEN + msg.datalen;
	printf("traceroute to %s (%s), %d hops max, %zu byte packets\n",
		   mgr->domain, inet_ntoa(mgr->to.sin_addr), mgr->max_ttl, pktlen);
	ping_loop(mgr, &msg, pkt, pktlen);
	free(msg.data);
	return (SUCCESS);
}
