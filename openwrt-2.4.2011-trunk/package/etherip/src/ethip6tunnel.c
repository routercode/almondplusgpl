#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_tunnel.h>
#include <linux/ip6_tunnel.h>

#define IPPROTO_ETHERIP 97

enum {
	ACTION_NONE = 0,
	ACTION_ADD = 1,
	ACTION_DEL = 2,
	ACTION_CHG = 3,
	ACTION_LIST = 4 };

void __init_tunnel6_params(struct ip6_tnl_parm *p, char *devname,
			   struct in6_addr daddr, struct in6_addr saddr, 
			   int set_saddr, int ttl)
{
	if (devname)
		strncpy(p->name, devname, IFNAMSIZ);
	else p->name[0] = 0;
	p->name[IFNAMSIZ-1] = 0;

	p->link = 0;
	p->proto = IPPROTO_ETHERIP;
	if (daddr.s6_addr32[0] != 0 || daddr.s6_addr32[1] != 0 || 
	    daddr.s6_addr32[2] != 0 || daddr.s6_addr32[3] != 0)
		p->raddr = daddr;
	if (set_saddr)
		p->laddr = saddr;
	if ((ttl >= 0) && (ttl < 256))
		p->hop_limit = ttl;
}

int __do_tunnel6_action(int cmd, struct ifreq *ifr)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	int err;

	if (fd == -1) {
		perror("socket");
		return 1;
	}

	err = ioctl(fd, cmd, ifr);
	if (err < 0)
		perror("ioctl");

	return (err < 0);
}

int do_tunnel6_add(char *devname, struct in6_addr daddr, struct in6_addr saddr, 
		   int ttl)
{
	struct ifreq ifr;
	struct ip6_tnl_parm p;

	strncpy(ifr.ifr_name, "ip6ethip0", IFNAMSIZ);
	ifr.ifr_ifru.ifru_data = &p;
	p.hop_limit = 0;
	__init_tunnel6_params(&p, devname, daddr, saddr, 1, ttl);

	return __do_tunnel6_action(SIOCADDTUNNEL, &ifr);
}

int do_tunnel6_change (char *devname, struct in6_addr daddr, 
		       struct in6_addr saddr, int set_saddr, int ttl)
{
	struct ifreq ifr;
	struct ip6_tnl_parm p;
	int fd;

	strncpy(ifr.ifr_name, devname, IFNAMSIZ);
	p.name[IFNAMSIZ-1] = 0;
	ifr.ifr_ifru.ifru_data = &p;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		perror("socket");
		return 1;
	}
	if (ioctl(fd, SIOCGETTUNNEL, &ifr) != 0) {
		perror("ioctl");
		close(fd);
		return 1;
	}
	close(fd);
	__init_tunnel6_params(&p, devname, daddr, saddr, set_saddr, ttl);

	return __do_tunnel6_action(SIOCCHGTUNNEL, &ifr);
}

int do_tunnel6_del(char *devname)
{
	struct ifreq ifr;

	strncpy(ifr.ifr_name, devname, IFNAMSIZ);
	return __do_tunnel6_action(SIOCDELTUNNEL, &ifr);
}

int do_tunnel6_list()
{
	int sd;
	FILE *fd = fopen("/proc/net/dev", "r");
	char line[2048];
	char dev[IFNAMSIZ];
	int i,j,k,found = 0;
	struct ifreq ifr;
	struct ip6_tnl_parm p;
	char ttl_str[8], saddr_str[48], daddr_str[48];
	char *junk;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == -1) {
		perror("socket");
		return 1;
	}

	if (fd == NULL) {
		perror("fopen");
		close(sd);
		return 1;
	}

	/* skip first 2 lines */
	junk = fgets(line, 2048, fd);
	junk = fgets(line, 2048, fd);

	while (fgets(line, 2048, fd)) {
		for (i=0;line[i] == ' ';++i);
		for (j=0;(line[i+j] != ':') && (line[i+j] != 0);++j) {
			if (j > IFNAMSIZ-1) break;
			dev[j] = line[i+j];
		}
		dev[j] = 0;

		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
		ifr.ifr_ifru.ifru_data = &p;
		if (ioctl(sd, SIOCGETTUNNEL, &ifr) != 0) continue;
		if (p.proto != IPPROTO_ETHERIP) continue;

		for (k=0;k<IFNAMSIZ-1;++k) if (p.name[k] == 0) break;
		for (   ;k<IFNAMSIZ-1;++k) p.name[k] = ' ';
		p.name[k] = 0;

		if (p.laddr.s6_addr32[0] != 0 || p.laddr.s6_addr32[1] !=0 ||
		    p.laddr.s6_addr32[2] != 0 || p.laddr.s6_addr32[3] !=0)
			inet_ntop(AF_INET6, (void*)&p.laddr, saddr_str, 48);
		else
			strcpy(saddr_str, "any            ");

		if (p.hop_limit == 0)
			strcpy(ttl_str, "default");
		else
			snprintf(ttl_str, 7, "%d", p.hop_limit);
		if (found == 0)
			printf("Device\t\tDestination\tSource\t\tTTL\n");
		printf("%s\t%s\t%s\t%s\n", p.name,
			inet_ntop(AF_INET6, (void*)&p.raddr, daddr_str, 48),
			saddr_str, ttl_str);
		++found;
	}
	if (found == 0)
		printf("No etherip devices configured\n");

	close(sd);
	fclose(fd);

	return 0;
}

void usage(const char* prog)
{
	printf("EtherIP Tunnel Setup Tool 0.1\n");
	printf("Usage: %s [-a|-r|-c|-l] [-d dest] [-n devname] [-s <source>] [-t ttl]\n", prog);
	printf("-a           add a new device\n");
	printf("-r           remove a device\n");
	printf("-c           change the destination address of a device\n");
	printf("-l           list devices (other arguments are ignored)\n");
	printf("-d <addr>    specify tunnel destination (use with -a)\n");
	printf("-s <addr>    specify tunnel source address\n");
	printf("-n <devname> specify the name of the tunnel device\n");
	printf("-t <ttl>     specify ttl for tunnel packets (0 means system default)\n");
}

int action_error()
{
	printf("Only one action can be specified\n");
	return 1;
}

int main(int argc, char **argv)
{
	int opt;
	char *devname = NULL;
	int action = ACTION_NONE;
	struct hostent *host;
	struct in6_addr daddr;
	struct in6_addr saddr;
	int set_saddr = 0;
	int ttl = -1;

	memset(&daddr, 0, sizeof(struct in6_addr));
	memset(&saddr, 0, sizeof(struct in6_addr));

	do {
		opt = getopt(argc, argv, "hard:n:lct:s:");
		switch (opt) {
			case 'h':
				usage(argv[0]);
				return 0;
			case 'a':
				if (action != ACTION_NONE)
					return action_error();

				action = ACTION_ADD;
				break;
			case 'r':
				if (action != ACTION_NONE)
					return action_error();

				action = ACTION_DEL;
				break;
			case 'c':
				if (action != ACTION_NONE)
					return action_error();

				action = ACTION_CHG;
				break;
			case 'l':
				if (action != ACTION_NONE)
					return action_error();

				action = ACTION_LIST;
				break;
			case 'd':
				inet_pton(AF_INET6, optarg, &daddr);
#if 0 //TODO: not support gethostbyname currently
				host = gethostbyname(optarg);
				if (host == NULL) {
					printf("Destination host not "
							"found: %s\n", optarg);
					return 1;
				}
				daddr = *(struct in6_addr*)host->h_addr;
#endif
				break;
			case 's':
				inet_pton(AF_INET6, optarg, &saddr);
#if 0 //TODO: not support gethostbyname currently
				host = gethostbyname(optarg);
				if (host == NULL) {
					printf("Source host not "
							"found: %s\n", optarg);
					return 1;
				}
				set_saddr = 1;
				saddr = *(struct in6_addr*)host->h_addr;
#endif
				break;
			case 'n':
				devname = optarg;
				break;
			case 't':
				ttl = atoi(optarg);
				if ((ttl < 0) || (ttl > 255)) {
					printf("TTL value must be between 0 and 255\n");
					usage(argv[0]);
					return 1;
				}
			case -1:
				break;
			default:
				usage(argv[0]);
				return 1;
		}
	} while (opt != -1);

	if (action == ACTION_ADD) {
		if (daddr.s6_addr32[0] == 0 && daddr.s6_addr32[1] == 0 &&
		    daddr.s6_addr32[2] == 0 && daddr.s6_addr32[3] == 0) {
			printf("A valid tunnel destination must be specified\n");
			return 1;
		} else return do_tunnel6_add(devname, daddr, saddr, ttl);
	}

	if (action == ACTION_DEL) {
		if (devname == 0) {
			printf("Please specify the devicename to delete\n");
			return 1;
		} else return do_tunnel6_del(devname);
	}

	if (action == ACTION_CHG) {
		if (devname == 0) {
			printf("A valid devicename must be specified\n");
			return 1;
		} else return do_tunnel6_change(devname, daddr, saddr, set_saddr, ttl);
	}

	if (action == ACTION_LIST)
		return do_tunnel6_list();

	printf("Please specify an action\n");
	return 1;
}
