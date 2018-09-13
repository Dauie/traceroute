/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_addr_iface.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/27 13:42:16 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/27 13:55:38 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

void				set_addr_iface(t_mgr *mgr, char *iface)
{
	int				on;
	struct in_addr	addr;

	on = 1;
	if (!iface)
	{
		dprintf(STDERR_FILENO, "traceroute: no interface specified.\n");
		exit(EXIT_FAILURE);
	}
	if (!(addr.s_addr = ft_getifaceaddr(iface, NULL, FALSE)))
	{
		dprintf(STDERR_FILENO, "traceroute: can't find interface '%s'.\n",
				iface);
		exit(EXIT_FAILURE);
	}
	if (!(setsockopt(mgr->send_sock, SOL_SOCKET, SO_DONTROUTE,
					(char*)&on, sizeof(on))))
	{
		dprintf(STDERR_FILENO, "Error. setsockopt().\n");
		exit(EXIT_FAILURE);
	}
	mgr->from.sin_addr = addr;
}

void				set_addr(t_mgr *mgr, char *addr)
{
	if (!addr)
	{
		dprintf(STDERR_FILENO, "traceroute: no address specified.\n");
		exit(EXIT_FAILURE);
	}
	if (ft_isaddrset(addr) == FAILURE)
	{
		dprintf(STDERR_FILENO, "traceroute: cannot use '%s',"
						" not configured on host.\n", addr);
		exit(EXIT_FAILURE);
	}
	inet_pton(AF_INET, addr, &mgr->from.sin_addr);
}
