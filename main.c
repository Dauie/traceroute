#include "incl/traceroute.h"

char		*g_args[] = {"-f", "-m", "-p", "-s", "-i"};


static void			useage(void)
{
	printf("Usage:\tping [-chv]\n"
				   "[-c count][-h help][-v verbose]\n");
	exit(SUCCESS);
}

void					set_init_ttl(t_mgr *mgr, char *ttl)
{
	if (!(mgr->init_ttl = ft_atoi(ttl)))
	{
		dprintf(STDERR_FILENO, "traceroute: %s bad value for first ttl.\n",
				ttl);
		exit(FAILURE);
	}
}

void 				set_protocol(t_mgr *mgr, char *protocol)
{
	struct protoent *proto;

	if (!(proto = getprotobyname(protocol)))
	{
		dprintf(STDERR_FILENO, "traceroute: Error getprotobyname()\n");
		exit(FAILURE);
	}
	mgr->proto = proto->p_proto;
}

void					set_max_ttl(t_mgr *mgr, char *ttl)
{
	if (!(mgr->max_ttl = ft_atoi(ttl)))
	{
		dprintf(STDERR_FILENO, "traceroute: %s bad value for max ttl.\n",
				ttl);
		exit(FAILURE);
	}
}

void 				set_probe_amt(t_mgr *mgr, char *nprobes)
{
	if (!(mgr->nprobes = ft_atoi(nprobes)))
	{
		dprintf(STDERR_FILENO, "traceroute: %s bad value for nprobe.\n",
				nprobes);
		exit(FAILURE);
	}
}

void 				set_addr_iface(t_mgr *mgr, char *iface)
{
	if (!(ft_getifaceaddr(iface, mgr->saddr)))
	{
		dprintf(STDERR_FILENO, "traceroute: Can't find interface %s\n",
				iface);
		exit(FAILURE);
	}
}

void 				set_addr(t_mgr *mgr, char *addr)
{
	if (!(ft_isaddrset(addr)))
	{
		dprintf(STDERR_FILENO, "traceroute: Cannot use %s\n",
				addr);
		exit(FAILURE);
	}
	ft_strcpy(mgr->saddr, addr);
}

void (*g_funcs[])(t_mgr *, char *) =
		{&set_init_ttl, &set_max_ttl, &set_protocol, &set_addr, &set_addr_iface };

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
		dprintf(STDERR_FILENO, "traceroute: Invalid option -- %s\n", flag);
		useage();
	}
	return (FAILURE);
}

int					parse_args(t_mgr *mgr, int ac, char **av)
{
	int i;

	i = 0;
	while (++i < ac)
	{
		if (ft_strncmp(av[i], "-h", 2))
			useage();
		else if (av[i][0] == '-' && av[i + 1])
			set_args(mgr, av[i], av[i + 1]);
		else if (av[i][0] != '-' || ac == 2)
		{
			if (ft_domtoip(av[i], mgr->daddr) == FAILURE)
			{
				dprintf(STDERR_FILENO, "traceroute: cannot resolve %s: Unknown host\n", av[i]);
				exit(FAILURE);
			}
		}
	}
	return (SUCCESS);
}

int					main(int ac, char **av)
{
	t_mgr	*mgr;

	if (!(mgr = ft_memalloc(sizeof(t_mgr))))
		return (FAILURE);
	if (ac == 1)
		useage();
	parse_args(mgr, ac, av);
}