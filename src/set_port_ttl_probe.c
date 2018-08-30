/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_port_ttl_probe.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/27 13:38:33 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/29 18:51:07 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

void				set_init_ttl(t_mgr *mgr, char *ttl)
{
	if (!ttl)
	{
		dprintf(STDERR_FILENO, "traceroute: no initial ttl specified\n");
		exit(FAILURE);
	}
	if ((mgr->ttl = ft_atoi(ttl)) <= 0)
	{
		dprintf(STDERR_FILENO, "traceroute: '%s' bad value for initial ttl.\n",
				ttl);
		exit(FAILURE);
	}
}

void				set_max_ttl(t_mgr *mgr, char *ttl)
{
	if (!ttl)
	{
		dprintf(STDERR_FILENO, "traceroute: no max ttl specified\n");
		exit(FAILURE);
	}
	if ((mgr->max_ttl = ft_atoi(ttl)) <= 0)
	{
		dprintf(STDERR_FILENO, "traceroute: '%s' bad value for max ttl.\n",
				ttl);
		exit(FAILURE);
	}
}

void				set_port(t_mgr *mgr, char *port)
{
	if (!port)
	{
		dprintf(STDERR_FILENO, "traceroute: no port specified\n");
		exit(FAILURE);
	}
	if ((mgr->udp_port = ft_atoi(port)) <= 0)
	{
		dprintf(STDERR_FILENO, "traceroute: Error bad port specified\n");
		exit(FAILURE);
	}
	mgr->to.sin_port = (in_port_t)mgr->udp_port;
}

void				set_probe_amt(t_mgr *mgr, char *nprobes)
{
	if (!nprobes)
	{
		dprintf(STDERR_FILENO, "traceroute: no probe amount specified\n");
		exit(FAILURE);
	}
	if ((mgr->nprobes = (uint)ft_atoi(nprobes)) <= 0)
	{
		dprintf(STDERR_FILENO, "traceroute: '%s' bad value for nprobe.\n",
				nprobes);
		exit(FAILURE);
	}
	if (mgr->nprobes > MAX_PROB_AMT)
	{
		dprintf(STDERR_FILENO, "traceroute: no more than"
				" '%d' probes per hop.\n", MAX_PROB_AMT);
		exit(FAILURE);
	}
}
