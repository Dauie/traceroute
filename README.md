# traceroute
trace the route of packets through a network 

## Features 


 -i -m -p -s -q -l -f -I
 
 -f : Set the initial time-to-live value used in outgoing probe packets.
 
      The default is 1, i.e., start with the first hop.


 -i : Specify a network interface to obtain the source IP address for outgoing
 
      probe packets. This is normally only useful on a multi-homed host.
      
      (See the -s flag for another way to do this.)


 -m : Set the max time-to-live (max number of hops) used in outgoing probe
 
      packets.The default is 30 hops (the same default used for
      
      ICMP connections).


 -p : Protocol specific. For UDP, sets the base port number used in
 
      probes (default is 33434).  traceroute hopes that nothing is listening
      
      on UDP ports base to base+nhops-1 at the destination host
      (so an ICMP PORT_UNREACHABLE message will be returned to terminate the
      
      route tracing).  If something is listening on a port in the default
      
      range, this option can be used to pick an unused port range.


 -s : Use the following IP address (which must be given as an IP number,
 
      not a hostname) as the source address in outgoing probe packets.
      
      On hosts with more than one IP address, this option can be used
      
      to force the source address to be something other than the IP
      
      address of the interface the probe packet is sent on.
      
      If the IP address is not one of this machine's inter-face addresses,
      
      an error is returned and nothing is sent.
      
      
 -q : Set the number of probes per ``ttl'' (default is three probes).
 
 
 -I : Use ICMP ECHO instead of UDP datagrams.



## install and run
`git clone https://github.com/dauie/traceroute.git && cd traceroute`

`git clone https://github.com/dauie/libft.git`

`make`

`sudo ./traceroute [-impsqlfI] <hostname/ip>


![alt text](https://github.com/Dauie/ping/blob/master/traceroutess.png)
