#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/errno.h>
#include<net/net_namespace.h>
#include<linux/netdevice.h>
#include <linux/netdev_features.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Vysakh P Pillai");
struct net_device *dev;

struct myi_priv {
        struct net_device_stats stats;
        int status;
        struct myi_packet *ppool;
        struct myi_packet *rx_queue;  /* List of incoming packets */
        int rx_int_enabled;
        int tx_packetlen;
        u8 *tx_packetdata;
        struct sk_buff *skb;
        spinlock_t lock;
        struct net_device *dev;
//        struct napi_struct napi;
};

int myi_open(struct net_device *dev)
{
	printk(KERN_ALERT"myi_open called\n");
	memcpy(dev->dev_addr, "\0MYIN0",ETH_ALEN);
	netif_start_queue(dev);
	return 0;
}

int myi_release( struct net_device *net)
{
	printk(KERN_ALERT"myi_release called\n");
	netif_stop_queue(dev);
	return(0);
}

int myi_tx(struct sk_buff *skb, struct net_device *dev) 
{
	return NETDEV_TX_OK;
}
static int myi_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	printk(KERN_ALERT"myi_ioctl called: %d\n",cmd);
        return 0;
}

const struct net_device_ops myi_netdev_ops = {
         .ndo_open = myi_open,		/*first 3 are mandatory to bring the network up*/
         .ndo_stop = myi_release,
         .ndo_start_xmit = myi_tx,	/*kernel panic without this in x68 . probably due to teh ARP at interface up*/
/*       .ndo_set_config = myi_config,
         .ndo_do_ioctl = myi_ioctl,
         .ndo_get_stats = myi_stats,
         .ndo_change_mtu = myi_change_mtu,
         .ndo_tx_timeout = myi_tx_timeout,
*/
 };

static void myi_setup(struct net_device *dev)
{
	 struct snull_priv *priv;
	 ether_setup(dev);	
	 dev->netdev_ops = &myi_netdev_ops;
	 dev->watchdog_timeo = 5; //jiffies
	 dev->flags           |= IFF_NOARP;
	 dev->features        |= NETIF_F_HW_CSUM;
 	 priv=netdev_priv(dev);
	 memset(priv,0,sizeof(struct myi_priv));
	 	

}

//static __net_init int myi_net_init(struct net *net)
static __net_init int myi_net_init(void)
{
	int err;
	printk(KERN_ALERT"myi_net_init called\n");
	err = -ENOMEM;
	dev = alloc_netdev(sizeof (struct myi_priv) , "myi%d", NET_NAME_UNKNOWN, myi_setup);
	if (!dev)
		return err;
	err = register_netdev(dev);
	if (err)
	{
		free_netdev(dev);
		return err;
		
	}
	return 0;
}
//static void __net_exit myi_net_exit(struct net *net)
static void __net_exit myi_net_exit(void)
{
	printk(KERN_ALERT"myi_net_ecit called\n");
	unregister_netdev(dev);
	free_netdev(dev);
}

module_init(myi_net_init);
module_exit(myi_net_exit);
