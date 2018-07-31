#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#include "../libft/incl/net.h"
#include "../libft/incl/cnvrsn.h"

# define OPTLEN 6
/**
* -i -m -p -s -q -N -t -l -1
* -f : Set the initial time-to-live value used in outgoing probe packets.
*      The default is 1, i.e., start with the first hop.
* -i : Specify a network interface to obtain the source IP address for outgoing
*      probe packets. This is normally only useful on a multi-homed host.
*      (See the -s flag for another way to do this.)
* -m : Set the max time-to-live (max number of hops) used in outgoing probe
*      packets.  The default is net.inet.ip.ttl hops (the same default used for
*      TCP connections).
* -p : protocol specific. For UDP and TCP, sets the base port number used in
*      probes (default is 33434).  traceroute hopes that nothing is listening
*      on UDP ports base to base+nhops-1 at the destination host
*      (so an ICMP PORT_UNREACHABLE message will be returned to terminate the
*      route tracing).  If something is listening on a port in the default
*      range, this option can be used to pick an unused port range.
* -s: Use the following IP address (which must be given as an IP number,
*     not a hostname) as the source address in outgoing probe packets.  On hosts
*     with more than one IP address, this option can be used to force the source
*     address to be something other than the IP address of the interface the
*     probe packet is sent on.  If the IP address is not one of this machine's inter-
*     face addresses, an error is returned and nothing is sent.
* -q: Set the number of probes per ``ttl'' to nqueries (default is three probes).
**/

typedef struct			s_opts
{

}						t_opts;

typedef struct			s_flags
{
	uint8_t				spec_iface: 1;
	uint8_t				max_ttl: 1;
	uint8_t				spec_proto: 1;
	uint8_t				spec_addr: 1;

}						t_flags;

typedef struct			s_manager
{
	int					sock;
	int					proto;
	int					init_ttl;
	int					max_ttl;
	int					nprobes;
	struct sockaddr_in	sin;
	char				domain[DOMAIN_NAME_LEN];
	char				daddr[INET_ADDRSTRLEN];
	char				saddr[INET_ADDRSTRLEN];
	t_flags				flags;
}						t_mgr;


#endif