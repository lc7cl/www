#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>

static struct nf_hook_ops nfho;

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
	
	if(iph && iph->protocol == 17 && iph->daddr == (192|168<<8|137<<16|153<<24))
	{
		uph = (struct udphdr*)((char*)iph + iph->ihl << 2);
		if(uph->len > 3 && uph->dest == 0x672B)
		{
			payload = (char*)udp + 8;
			*payload = 'A';
			*(payload+1) = 'B';
		}
	}

	return NF_ACCEPT;
}

static int my_init()
{
	nfho.hook = hook_fn;
	nfho.priority = NF_IP_PRI_FIRST;
	nfho.pf = PF_INET;
	nfho.hooknum = NF_IP_PTE_ROUTING;

	nf_register_hook(&nfho);

	return 0;
}

static void my_exit()
{
	nf_unregister_hook(&nfho);
	return;
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
