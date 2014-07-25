#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/netfilter.h>
//#include <linux/netfilter_ipv4.h>
//#include <uapi/linux/udp.h>
//#include <uapi/linux/ip.h>

//static struct nf_hook_ops nfho;

static char* netmask = "255.255.255.0";
static char* subnet = "10.1.1.0";

#if 0
static unsigned int inet_addr(char* ip)
{
	unsigned int ret = 0, a = b = c = d = 0;
	char* p;

	p=strsep(&ip, ".")
	a = strtoul(p, NULL, 16) & 0xff; 
	p=strsep(&ip, ".")
	b = strtoul(p, NULL, 16) & 0xff;
	p=strsep(&ip, ".")
	c = strtoul(p, NULL, 16) & 0xff;
	p=strsep(&ip, ".")
	d = strtoul(p, NULL, 16) & 0xff;

	ret = a<<24||b<<16||c<<8|d;

	return htonl(ret);
}
#endif


#if 0
unsigned int hook_fn(unsigned int hooknum,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out
		int (*okfn)(struct sk_buff *))
{
	struct iphdr *iph;
	struct udphdr *uph;
	char *payload;
	if(skb == NULL)
		return NF_ACCEPT;

	iph = ip_hdr(skb);
	
	if(iph && iph->protocol == 17 && iph->daddr == inet_addr("192.168.137.153"))
	{
		uph = (struct udphdr*)((char*)iph + iph->hdl << 2);
		if(uph->len > 3 && uph->dport == htons(11111))
		{
			payload = (char*)udp + 8;
			*payload = 'A';
			*(payload+1) = 'B';
		}
	}
	return NF_ACCEPT;
}
#endif

static int __init my_init(void)
{
#if 0
	nfho.hook = hook_fn;
	nfho.priority = NF_IP_PRI_FIRST;
	nfho.pf = PF_INET;
	nfho.hooknum = NF_IP_PRE_ROUTING;

	nf_register_hook(&nfho);
#endif

	return 0;
}

static void __exit my_exit(void)
{
#if 0
	nf_unregister_hook(&nfho);
#endif
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
