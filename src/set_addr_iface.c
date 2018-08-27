/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_addr_iface.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/27 13:42:16 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/27 13:42:16 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

void 				set_addr_iface(t_mgr *mgr, char *iface)
{
	struct in_addr	addr;

	if (!iface)
	{
		dprintf(STDERR_FILENO, "traceroute: no interface specified.\n");
		useage();
	}
	if (!(addr.s_addr = ft_getifaceaddr(iface, NULL, FALSE)))
	{
		dprintf(STDERR_FILENO, "traceroute: can't find interface '%s'.\n",
				iface);
		exit(FAILURE);
	}
	mgr->from.sin_addr = addr;
}

void 				set_addr(t_mgr *mgr, char *addr)
{
	if (ft_isaddrset(addr) == FAILURE)
	{
		dprintf(STDERR_FILENO, "traceroute: cannot use '%s', not configured on host.\n",
				addr);
		exit(FAILURE);
	}
	inet_pton(AF_INET, addr, &mgr->from.sin_addr);
}
