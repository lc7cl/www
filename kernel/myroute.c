#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm-generic/bug.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>
#include <net/ip.h>

#include "ip_proxy.h"

static char *cli_ip = "192.168.110.49";
static char *svr_ip = "192.168.110.54";
static char *ip_48 = "192.168.110.48";
struct net_device *dev;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
static unsigned int recv_hookfn(const struct nf_hook_ops *ops,
		struct sk_buff *skb, 
		const struct net_device *in, 
		const struct net_device *out,
		int (*okfn)(struct sk_buff*))
#else
static unsigned int recv_hookfn(unsigned int hooknum, 
		struct sk_buff *skb, 
		const struct net_device *in, 
		const struct net_device *out,
		int (*okfn)(struct sk_buff*))
#endif
{
	int ret;
	struct iphdr *iph;
	struct proxy_client *client = NULL;
	
	iph = ip_hdr(skb);
	if (iph->saddr != in_aton(svr_ip) 
		||(iph->daddr != in_aton(ip_48))) 
		goto out;

	switch(iph->protocol) {
		case IPPROTO_ICMP:
			{
				unsigned short *port;
				port = (unsigned short *)(iph + 1);
				ret = find_client_by_port(*port, &client);
				if (!ret)
					goto out;
				BUG_ON(client == NULL);
				iph->daddr = client->saddr;

			}
			break;
		case IPPROTO_UDP:
		case IPPROTO_TCP:
			{
				unsigned short *dport;
				dport = (unsigned short *)(iph + 1);
				ret = find_client_by_port(htons(*dport), &client);
				if (!ret)
					goto out;
				BUG_ON(client == NULL);

				*dport = client->sport;
				iph->daddr = client->saddr;
				
			}

			break;
		default:
			goto out;
	}

	iph->daddr = in_aton(cli_ip);
	skb->ip_summed = CHECKSUM_NONE;	

out:
	return NF_ACCEPT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)
static unsigned int send_hookfn(const struct nf_hook_ops *ops,
		struct sk_buff *skb, 
		const struct net_device *in, 
		const struct net_device *out,
		int (*okfn)(struct sk_buff*))
#else
static unsigned int send_hookfn(unsigned int hooknum, 
		struct sk_buff *skb, 
		const struct net_device *in, 
		const struct net_device *out,
		int (*okfn)(struct sk_buff*))
#endif
{
	struct iphdr *iph;
	unsigned short *sport;
	int ret;

	iph = ip_hdr(skb);
	if (iph->daddr != in_aton(svr_ip)) 
		goto out;

	switch(iph->protocol)
	{
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		{
			sport = (unsigned short *)(iph + 1);
			sport++;
			ret = add_client(iph->saddr, *sport);
			if (ret < 0) {
				*sport = htons(ret + base_port);
				iph->saddr = in_aton(ip_48);
			}
		}
		break;
	case IPPROTO_ICMP:
		sport = (unsigned short *)(iph + 1);
		ret = add_client(iph->saddr, htons(*sport));
		printk("%s %d saddr:%x daddr:%x typecode:%2hx \n",__FILE__, __LINE__, iph->saddr, iph->daddr, htons(*sport));
		if (ret < 0)
			iph->saddr = in_aton(ip_48);
		break;
	default:
		goto out;
	}
out:
	return NF_ACCEPT;
}

static struct nf_hook_ops __read_mostly recv_hook_ops = {
	.hook = recv_hookfn, 	
	.pf = NFPROTO_IPV4,
	.hooknum = NF_INET_PRE_ROUTING, 
	.priority = NF_IP_PRI_LAST
};

static struct nf_hook_ops __read_mostly send_hook_ops = {
	.hook = send_hookfn, 	
	.pf = NFPROTO_IPV4,
	.hooknum = NF_INET_POST_ROUTING, 
	.priority = NF_IP_PRI_LAST
};

static int __init route_init(void)
{
	int ret;
	
	ret = proxy_init();
	if (ret < 0)
		return -1;

	ret = nf_register_hook(&recv_hook_ops);
	if (ret < 0) 
		return -1;
	ret = nf_register_hook(&send_hook_ops);
	if (ret < 0)
		return -1;

	return 0;
}

static void __exit route_exit(void)
{
	nf_unregister_hook(&recv_hook_ops);
	nf_unregister_hook(&send_hook_ops);
	proxy_fini();
}

module_init(route_init);
module_exit(route_exit);
MODULE_LICENSE("GPL");
