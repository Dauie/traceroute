/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/17 20:01:41 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/24 15:16:33 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "incl/traceroute.h"

char		*g_args[] = {"-f", "-m", "-p", "-s", "-i", "-q"};

static void			useage(void)
{
	printf("Usage:\ttraceroute [-fmpsih]\n"
				"\t[-f first-ttl][-m max-ttl][-p protocol]\n"
				"\t[-s ip address][-s interface address]\n"
				"\t[-q probe amount][-h help]... destination\n");
	exit(SUCCESS);
}

void					set_init_ttl(t_mgr *mgr, char *ttl)
{
	if (!ttl)
	{
		dprintf(STDERR_FILENO, "traceroute: no initial ttl specified\n");
		useage();
	}
	if (!(mgr->ttl = ft_atoi(ttl)))
	{
		dprintf(STDERR_FILENO, "traceroute: '%s' bad value for initial ttl.\n",
				ttl);
		exit(FAILURE);
	}
}

void 				set_port(t_mgr *mgr, char *port)
{
	if (!port)
	{
		dprintf(STDERR_FILENO, "traceroute: no port specified\n");
		useage();
	}
	if (!(mgr->udp_port  = ft_atoi(port)))
	{
		dprintf(STDERR_FILENO, "traceroute: Error bad port specified\n");
		exit(FAILURE);
	}
	mgr->to.sin_port = (in_port_t)mgr->udp_port;
}

void					set_max_ttl(t_mgr *mgr, char *ttl)
{
	if (!ttl)
	{
		dprintf(STDERR_FILENO, "traceroute: no max ttl specified\n");
		useage();
	}
	if (!(mgr->max_ttl = ft_atoi(ttl)))
	{
		dprintf(STDERR_FILENO, "traceroute: '%s' bad value for max ttl.\n",
				ttl);
		exit(FAILURE);
	}
}

void 				set_probe_amt(t_mgr *mgr, char *nprobes)
{
	if (!nprobes)
	{
		dprintf(STDERR_FILENO, "traceroute: no probe amount specified\n");
		useage();
	}
	if (!(mgr->nprobes = (uint)ft_atoi(nprobes)))
	{
		dprintf(STDERR_FILENO, "traceroute: '%s' bad value for nprobe.\n",
				nprobes);
		exit(FAILURE);
	}
	if (mgr->nprobes > MAX_PROB_AMT)
	{
		dprintf(STDERR_FILENO, "no more than '%d' probes per hop", MAX_PROB_AMT);
		exit(FAILURE);
	}
}

void 				set_addr_iface(t_mgr *mgr, char *iface)
{
	struct in_addr	addr;

	if (!iface)
	{
		dprintf(STDERR_FILENO, "traceroute: no interface specified\n");
		useage();
	}
	if (!(addr.s_addr = ft_getifaceaddr(iface, NULL, FALSE)))
	{
		dprintf(STDERR_FILENO, "traceroute: Can't find interface '%s'\n",
				iface);
		exit(FAILURE);
	}
	mgr->from.sin_addr = addr;
}

void 				set_addr(t_mgr *mgr, char *addr)
{
	if (ft_isaddrset(addr) == FAILURE)
	{
		dprintf(STDERR_FILENO, "traceroute: Cannot use '%s', not configured on host\n",
				addr);
		exit(FAILURE);
	}
	inet_pton(AF_INET, addr, &mgr->from.sin_addr);
}

void (*g_funcs[])(t_mgr *, char *) =
		{ &set_init_ttl, &set_max_ttl, &set_port, &set_addr, &set_addr_iface, &set_probe_amt };

int 				set_args(t_mgr *mgr, char *flag, char *setting)
{
	int i;

	i = -1;
	while (++i < OPTLEN)
	{
		if (ft_strncmp(g_args[i], flag, 2) == 0)
		{
			g_funcs[i](mgr, setting);
			return (SUCCESS);
		}
	}
	if (i == OPTLEN)
	{
		dprintf(STDERR_FILENO, "traceroute: Invalid option -- '%c'\n", flag[1]);
		useage();
	}
	return (FAILURE);
}

int					parse_args(t_mgr *mgr, int ac, char **av)
{
	int i;
	struct in_addr addr;

	i = 0;
	while (++i < ac)
	{
		if (ft_strncmp(av[i], "-h", 2) == 0)
			useage();
		else if (av[i][0] == '-')
		{
			set_args(mgr, av[i], av[i + 1]);
			i++;
		}
		else if (av[i][0] != '-' || ac == 2)
		{
			if (!(addr.s_addr = ft_domtoip(av[i], NULL, FALSE)))
			{
				dprintf(STDERR_FILENO, "traceroute: cannot resolve"
						" '%s': Unknown host\n", av[i]);
				exit(FAILURE);
			}
			ft_strcpy(mgr->domain, av[i]);
			mgr->to.sin_addr.s_addr = addr.s_addr;
		}
	}
	return (SUCCESS);
}

void				set_program_defaults(t_mgr *mgr)
{
	mgr->flags.run = TRUE;
	mgr->flags.udp = TRUE;
	mgr->flags.icmp = FALSE;
	mgr->pid = getpid();
	mgr->ttl = DEF_INIT_TTL;		/* 1 */
	mgr->max_ttl = DEF_MAX_TTL;		/* 64 */
	mgr->nprobes = DEF_PROB_AMT;	/* 3 */
	mgr->udp_port = DEF_UDP_PORT;	/* 33434 */
	mgr->from.sin_port = (in_port_t)mgr->pid;
	mgr->to.sin_port =  DEF_UDP_PORT;
	mgr->from.sin_family = AF_INET;
	mgr->to.sin_family = AF_INET;
}

void				create_sock(t_mgr *mgr)
{
	struct protoent *pro;

	pro = getprotobyname(mgr->flags.icmp ? "icmp" : "udp");
	mgr->send_sock = ft_makerawsock(IPPROTO_RAW);
	mgr->recv_sock = ft_makerawsock(pro->p_proto);
	ft_sock_hdrincl(mgr->send_sock);
	if (mgr->flags.udp == TRUE &&
		bind(mgr->send_sock,
			(struct sockaddr *)&mgr->from, sizeof(mgr->from)) < 0)
	{
		dprintf(STDERR_FILENO, "Error bind().\n");
		exit(FAILURE);
	}
}

int					main(int ac, char **av)
{
	t_mgr	*mgr;

	if (!(mgr = ft_memalloc(sizeof(t_mgr))))
		return (FAILURE);
	if (ac == 1)
		useage();
	set_program_defaults(mgr);
	parse_args(mgr, ac, av);
	create_sock(mgr);
	traceroute(mgr);
	free(mgr);
}