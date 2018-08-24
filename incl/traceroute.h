#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#include "../libft/incl/net.h"
#include "../libft/incl/net_ping.h"
#include "../libft/incl/cnvrsn.h"

# define OPTLEN (7)
# define DEF_WAIT_TIME (2)
# define DEF_MAX_TTL (64)
# define DEF_INIT_TTL (1)
# define DEF_PROB_AMT (3)
# define DEF_UDP_PORT (33434)
# define MAX_PROB_AMT (10)
# define MSG_DATA "         4242~!@#$%^&*()_+12345678890-="

/**
** -i -m -p -s -q -l -f -I
** -f : Set the initial time-to-live value used in outgoing probe packets.
**      The default is 1, i.e., start with the first hop.
** -i : Specify a network interface to obtain the source IP address for outgoing
**      probe packets. This is normally only useful on a multi-homed host.
**      (See the -s flag for another way to do this.)
** -m : Set the max time-to-live (max number of hops) used in outgoing probe
**      packets.  The default is net.inet.ip.ttl hops (the same default used for
**      TCP connections).
** -p : Protocol specific. For UDP, sets the base port number used in
**      probes (default is 33434).  traceroute hopes that nothing is listening
**      on UDP ports base to base+nhops-1 at the destination host
**      (so an ICMP PORT_UNREACHABLE message will be returned to terminate the
**      route tracing).  If something is listening on a port in the default
**      range, this option can be used to pick an unused port range.
** -s : Use the following IP address (which must be given as an IP number,
**      not a hostname) as the source address in outgoing probe packets.  On hosts
**      with more than one IP address, this option can be used to force the source
**      address to be something other than the IP address of the interface the
**      probe packet is sent on.  If the IP address is not one of this machine's
**      inter-face addresses, an error is returned and nothing is sent.
** -q : Set the number of probes per ``ttl'' to nqueries (default is three probes).
** -I : Use ICMP ECHO instead of UDP datagrams.
**/

typedef struct			s_flags
{
	uint8_t 			run: 1;
	uint8_t				icmp: 1;
	uint8_t				udp: 1;
}						t_flags;

typedef struct			s_manager
{
	t_flags				flags;
	int					ttl;
	int					max_ttl;
	int					udp_port;
	pid_t				pid;
	int					nprobes;
	char				domain[DOMAIN_NAME_LEN];
	int					send_sock;
	int					recv_sock;
	struct sockaddr_in	from;
	struct sockaddr_in	to;

}						t_mgr;

int				traceroute(t_mgr *mgr);

#endif