/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_packet.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/25 16:44:48 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/27 16:10:43 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

static int			icmppkt_check(int8_t *respbuff, int pid, int seq)
{
	struct icmp		*icmp;

	icmp = (struct icmp *)(respbuff + IPV4_HDRLEN + ICMP_HDRLEN + IPV4_HDRLEN);
	return (icmp->icmp_id == htons(pid) &&
		icmp->icmp_seq == htons(seq));
}

static int			udppkt_check(int8_t *respbuff, int udp_port,
								int pid, int seq)
{
	struct udphdr	*udp;

	udp = (struct udphdr *)(respbuff + IPV4_HDRLEN + ICMP_HDRLEN + IPV4_HDRLEN);
	return (udp->uh_dport == htons(udp_port) &&
			udp->uh_sport == htons(pid + seq));
}

int					check_packet(t_mgr *mgr, int8_t *resp_buff)
{
	if (mgr->flags.icmp == TRUE)
		return (icmppkt_check(resp_buff, mgr->pid, mgr->ttl));
	else
		return (udppkt_check(resp_buff, mgr->udp_port, mgr->pid, mgr->ttl));
}
