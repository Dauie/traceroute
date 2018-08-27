/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_response.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/25 16:43:52 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/25 16:52:20 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

static void					print_specials(int8_t *buff)
{
	struct icmp				*icmp;
	char unreach_specials[] = "NHP!FSUWIAZQTXVC";

	icmp = (struct icmp *)(buff + IPV4_HDRLEN);
	if (icmp->icmp_type == ICMP_TIMXCEED)
		return;
	else if (icmp->icmp_type == ICMP_UNREACH)
	{
		if (icmp->icmp_code <= 15)
			printf(" !%c", unreach_specials[icmp->icmp_code]);
		else
			printf(" !%d", icmp->icmp_code);
	}
	else if (icmp->icmp_type == ICMP_SOURCEQUENCH)
		printf(" !QN");
}

int							handle_response(t_mgr *mgr, int8_t *resp_buff,
											   t_echopkt *msg, int probe)
{
	struct in_addr			resp_addr;
	static struct in_addr	prev_resp_addr;
	char 					revdnsstr[DOMAIN_NAME_LEN];

	if (check_packet(mgr, resp_buff) == SUCCESS)
	{
		resp_addr = ((struct ip *)resp_buff)->ip_src;
		if (prev_resp_addr.s_addr != resp_addr.s_addr)
		{
			ft_iptodom(resp_addr.s_addr, revdnsstr);
			printf("  %s (%s)", revdnsstr, inet_ntoa(resp_addr));
		}
		printf("  %.3f ms", (float)ft_timediff_ms(&msg->recvd, &msg->sent));
		print_specials(resp_buff);
		prev_resp_addr = resp_addr;
	}
	if (probe >= mgr->nprobes)
		prev_resp_addr.s_addr = 0;
	return (FAILURE);
}