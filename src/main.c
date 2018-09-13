/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rlutt <rlutt@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/17 20:01:41 by rlutt             #+#    #+#             */
/*   Updated: 2018/08/29 20:54:15 by rlutt            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incl/traceroute.h"

char	*g_args[] = {"-f", "-m", "-p", "-s", "-i", "-q"};

void	(*g_funcs[])(t_mgr *, char *) = { &set_init_ttl, &set_max_ttl,
	&set_port, &set_addr, &set_addr_iface, &set_probe_amt };

static int			set_args(t_mgr *mgr, char *flag, char *setting)
{
	int i;

	i = -1;
	while (++i < OPTLEN)
	{
		if (ft_strcmp(g_args[i], flag) == 0)
		{
			g_funcs[i](mgr, setting);
			return (SUCCESS);
		}
	}
	if (i == OPTLEN)
	{
		dprintf(STDERR_FILENO, "traceroute: "
				"invalid option -- '%c'.\n", flag[1]);
		useage();
	}
	return (FAILURE);
}

static int			parse_args(t_mgr *mgr, int ac, char **av)
{
	int				i;
	struct in_addr	addr;

	i = 0;
	while (++i < ac)
	{
		if (ft_strncmp(av[i], "-h", 2) == 0)
			useage();
		else if (ft_strcmp(av[i], "-I") == 0)
			mgr->flags.icmp = TRUE;
		else if (av[i][0] == '-' && i++)
			set_args(mgr, av[i - 1], av[i]);
		else
		{
			if (!(addr.s_addr = ft_domtoip(av[i], NULL)))
			{
				dprintf(STDERR_FILENO, "traceroute: cannot resolve"
						" '%s': unknown host.\n", av[i]);
				exit(EXIT_FAILURE);
			}
			ft_strcpy(mgr->domain, av[i]);
			mgr->to.sin_addr.s_addr = addr.s_addr;
		}
	}
	return (SUCCESS);
}

static void			set_program_defaults(t_mgr *mgr)
{
	mgr->flags.run = TRUE;
	mgr->flags.udp = TRUE;
	mgr->flags.icmp = FALSE;
	mgr->pid = getpid();
	mgr->ttl = DEF_INIT_TTL;
	mgr->max_ttl = DEF_MAX_TTL;
	mgr->nprobes = DEF_PROB_AMT;
	mgr->udp_port = DEF_UDP_PORT;
	mgr->from.sin_port = (in_port_t)mgr->pid;
	mgr->to.sin_port = DEF_UDP_PORT;
	mgr->from.sin_family = AF_INET;
	mgr->to.sin_family = AF_INET;
}

static void			create_sock(t_mgr *mgr)
{
	mgr->send_sock = ft_makerawsock(mgr->flags.udp ?
									IPPROTO_UDP : IPPROTO_ICMP);
	mgr->recv_sock = ft_makerawsock(IPPROTO_ICMP);
	ft_setsock_hdrincl(mgr->send_sock);
	if (mgr->flags.udp == TRUE &&
		bind(mgr->send_sock,
			(struct sockaddr *)&mgr->from, sizeof(mgr->from)) < 0)
	{
		dprintf(STDERR_FILENO, "Error bind().\n");
		exit(EXIT_FAILURE);
	}
}

int					main(int ac, char **av)
{
	t_mgr			*mgr;

	if (!(mgr = ft_memalloc(sizeof(t_mgr))))
		return (FAILURE);
	if (ac == 1)
		useage();
	set_program_defaults(mgr);
	parse_args(mgr, ac, av);
	if (!(mgr->to.sin_addr.s_addr))
	{
		dprintf(STDERR_FILENO, "traceroute: destination not specified.\n");
		useage();
	}
	create_sock(mgr);
	traceroute(mgr);
	free(mgr);
}
