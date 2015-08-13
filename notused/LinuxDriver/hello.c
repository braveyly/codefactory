#include <linux/module.h>
#include <linux/init.h>
MODULE_LICENSE("GPL"); 
static int __init hello_init (void)
{
   printk("Hello module init\n");
   return 0;
}

static void __exit hello_exit (void)
{
   printk("Hello module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
