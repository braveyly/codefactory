#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <errno.h>
#include <asm/uaccess.h> 
#include <asm/processor.h>
#include <linux/prefetch.h>
MODULE_LICENSE("GPL");

#define MAJOR_NUM 254
static int global_var = 0; 

static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off)
{
 if (__copy_to_user(buf, &global_var, sizeof(int)))
 {
  return - EFAULT;
 } 
 return sizeof(int);
}

static ssize_t globalvar_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
 if (__copy_from_user(&global_var, buf, sizeof(int)))
 {
  return - EFAULT;
 } 
 return sizeof(int);
}

//static ssize_t globalvar_read(struct file *filp, char *buf, size_t len, loff_t *off);
//static ssize_t globalvar_read(struct file *, char *, size_t, loff_t*);
//static ssize_t globalvar_write(struct file *, const char *, size_t, loff_t*);

struct file_operations globalvar_fops =
{
 read: globalvar_read, 
 write: globalvar_write
};

static int __init globalvar_init(void)
{
 int ret;

 ret = register_chrdev(MAJOR_NUM, "globalvar", &globalvar_fops);
 if (ret)
 {
  printk("globalvar register failure");
 }
 else
 {
  printk("globalvar register success");
 }
 return ret;
}

static void __exit globalvar_exit(void)
{
 int ret;

 ret = unregister_chrdev(MAJOR_NUM, "globalvar");
 if (ret)
 {
  printk("globalvar unregister failure");
 }
 else
 {
  printk("globalvar unregister success");
 }
}



module_init(globalvar_init);
module_exit(globalvar_exit);

