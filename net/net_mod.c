#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/of.h>                 
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fake Ethernet driver");

static int eth_probe(struct platform_device *pdev);
static int eth_remove(struct platform_device *pdev);
static int eth_open(struct net_device *dev);
static int eth_release(struct net_device *dev);
static int eth_xmit(struct sk_buff *skb, struct net_device *ndev);
static int eth_init(struct net_device *dev);


struct eth_struct {
    int bar;
    int foo;
    struct net_device *my_ndev;
};

static struct platform_driver mypdrv = {
    .probe = eth_probe,
    .remove = eth_remove,
    .driver = {
        .name = "eth",
        .of_match_table = of_match_ptr(eth_dt_ids),
        .owner = THIS_MODULE,
    },
};

module_platform_driver(mypdrv);

static const struct net_device_ops my_netdev_ops = {
    .ndo_init = eth_init,
    .ndo_open = eth_open,
    .ndo_stop = eth_release,
    .ndo_start_xmit = eth_xmit,
    .ndo_validate_addr = eth_validate_addr,
    .ndo_validate_addr = eth_validate_addr,
};

static const struct of_device_id eth_dt_ids[] = {
    { .compatible = "packt,eth", },
    { /* sentinel */ }
};

static int eth_probe(struct platform_device *pdev)
{
    int ret;
    struct eth_struct *priv;
    struct net_device *my_ndev;

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    my_ndev = alloc_etherdev(sizeof(struct eth_struct));
    my_ndev->if_port = IF_PORT_10BASET;
    my_ndev->netdev_ops = &my_netdev_ops;


    ret = register_netdev(my_ndev);
    if(ret) {
        pr_info("my net dev: Error %d initalizing card ...", ret);
        return ret;
    }
    priv->my_ndev = my_ndev;
    platform_set_drvdata(pdev, priv);
    return 0;
}

static int eth_remove(struct platform_device *pdev)
{
    struct eth_struct *priv;
    priv = platform_get_drvdata(pdev);
    pr_info("Cleaning Up the Module\n");
    unregister_netdev(priv->my_ndev);
    free_netdev(priv->my_ndev);

    return 0;
}

static int eth_open(struct net_device *dev) 
{
    printk("eth_open called\n");

    netif_start_queue(dev);
    return 0;
}

static int eth_release(struct net_device *dev) 
{
    pr_info("eth_release called\n");
    netif_stop_queue(dev);
    return 0;
}

static int eth_xmit(struct sk_buff *skb, struct net_device *ndev) 
{
    pr_info("eth_xmit called...\n");
    ndev->stats.tx_bytes += skb->len;
    ndev->stats.tx_packets++;
    skb_tx_timestamp(skb);
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static int eth_init(struct net_device *dev)
{
    pr_info("eth device initialized\n");
    return 0;
};
