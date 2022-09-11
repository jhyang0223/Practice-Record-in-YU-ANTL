#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

#include<linux/interrupt.h>
#include<linux/gpio.h>

#include<asm/uaccess.h>
#include<linux/fs.h>

#include<linux/signal.h>
#include<linux/sched.h>

#include<linux/slab.h>
#include<linux/device.h>
#include<linux/cdev.h>

#define DRIVER_AUTHOR   "YJH"
#define DRIVER_DESC     "Test IOCTL Function with GPIO Output Pin"

#define DEVICE_NAME     "ioctl_gpiodev"
#define READ_BUFF_LEN   6

#define GPIO_OUTPUT_PIN27 27
#define GPIO_OUTPUT_PIN27_DESC "BCM 27"

#define GPIO_OUTPUT_PIN17 17
#define GPIO_OUTPUT_PIN17_DESC "BCM 17"

#define MY_IOCTL_NUMBER 102
#define MY_IOCTL_PIN27  _IO(MY_IOCTL_NUMBER, 0)
#define MY_IOCTL_PIN17  _IO(MY_IOCTL_NUMBER, 1)
#define MY_IOCTL_NR     3

//Module params declaration block
int cur_gpio_pin = GPIO_OUTPUT_PIN27;

//device registration variables
struct class *myclass;
struct cdev *mycdev;
struct device *mydevice;
dev_t mydev;

int ioctl_gpiodev_open(struct inode *, struct file *);
int ioctl_gpiodev_release(struct inode *, struct file *);
long ioctl_gpiodev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct file_operations fops = {
        .open = ioctl_gpiodev_open,
        .release = ioctl_gpiodev_release,
        .unlocked_ioctl = ioctl_gpiodev_ioctl
};

int ioctl_gpiodev_open(struct inode *inode, struct file *file)
{
        printk(KERN_ALERT "[ioctl_gpiodev] device open\n");
        if(gpio_request(GPIO_OUTPUT_PIN27, GPIO_OUTPUT_PIN27_DESC))
        {
                printk(KERN_ALERT "[ioctl_gpiodev] GPIO request failure %s\n", GPIO_OUTPUT_PIN27_DESC);
                return -1;
        }

        if(gpio_direction_output(GPIO_OUTPUT_PIN27,1))
        {
                printk(KERN_ALERT"[ioctl_gpiodev] GPIO direction configuration failure : %s\n", GPIO_OUTPUT_PIN27_DESC);
                return -2;
        }

        cur_gpio_pin = GPIO_OUTPUT_PIN27;

        return 0;
}

int ioctl_gpiodev_release(struct inode *inode, struct file *file)
{
        gpio_free(cur_gpio_pin);
        printk(KERN_ALERT "[ioctl_gpiodev] device release\n");
        return 0;
}
void mod_exit(void)
{
//      my_int_release();
        cdev_del(mycdev);
        device_destroy(myclass, mydev);
        class_destroy(myclass);
        unregister_chrdev_region(mydev,1);
}

long ioctl_gpiodev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int err=0, size;
        int data=0;
        if(_IOC_TYPE(cmd) != MY_IOCTL_NUMBER) return -EINVAL;
        if(_IOC_NR(cmd) >= MY_IOCTL_NR) return -EINVAL;
        if(_IOC_DIR(cmd) & _IOC_READ)
        {
                err = access_ok(VERIFY_READ, (void *)arg, sizeof(unsigned long));
                if(err<0) return -EINVAL;
        }
       else if(_IOC_DIR(cmd) & _IOC_WRITE)
        {
                err = access_ok(VERIFY_WRITE, (void *)arg, sizeof(unsigned long));
                if(err<0) return -EINVAL;
        }
       size = _IOC_SIZE(cmd);
        switch(cmd)
        {
                case MY_IOCTL_PIN27:
                        printk("[ioctl_gpiodev] OUTPUT PIN IS 27 FROM NOW\n");
                        gpio_free(cur_gpio_pin);
                       if(gpio_request(GPIO_OUTPUT_PIN27, GPIO_OUTPUT_PIN27_DESC))
                        {
                                printk(KERN_ALERT "[ioctl_gpiodev] GPIO request failure %s\n", GPIO_OUTPUT_PIN27_DESC);
                                return -2;
                        }

                        if(gpio_direction_output(GPIO_OUTPUT_PIN27,1))
                        {
                                printk(KERN_ALERT"[ioctl_gpiodev] GPIO direction configuration failure : %s\n", GPIO_OUTPUT_PIN27_DESC);
                                return -2;
                        }
                        cur_gpio_pin = GPIO_OUTPUT_PIN27;
                        break;

                case MY_IOCTL_PIN17:
                        printk("[ioctl_gpiodev] OUTPUT PIN IS 17 FROM NOW\n");
                        gpio_free(cur_gpio_pin);
                        if(gpio_request(GPIO_OUTPUT_PIN17, GPIO_OUTPUT_PIN27_DESC))
                        {
                                printk(KERN_ALERT "[ioctl_gpiodev] GPIO request failure %s\n", GPIO_OUTPUT_PIN27_DESC);
                                return -2;
                        }

                        if(gpio_direction_output(GPIO_OUTPUT_PIN17,1))
                        {
                                printk(KERN_ALERT"[ioctl_gpiodev] GPIO direction configuration failure : %s\n", GPIO_OUTPUT_PIN27_DESC);
                                return -2;
                        }
                        cur_gpio_pin = GPIO_OUTPUT_PIN17;
                        break;

                default:
                        printk("[ioctl_gpiodev] Unknown command...\n");
                        break;
        }
        return 0;


}

int mod_init(void)
{
        if(alloc_chrdev_region(&mydev,0,1,"ioctl_gpiodev")<0)
                return -EBUSY;

        myclass = class_create(THIS_MODULE, "ioctl_gpiodev");
        if(IS_ERR(myclass))
        {
                unregister_chrdev_region(mydev,1);
                return PTR_ERR(myclass);
        }

        mydevice = device_create(myclass, NULL, mydev, NULL, "ioctl_gpiodev");
        if(IS_ERR(mydevice))
        {
                class_destroy(myclass);
                unregister_chrdev_region(mydev,1);
                return PTR_ERR(mydevice);
        }

        mycdev = cdev_alloc();
        mycdev->ops = &fops;
        mycdev->owner = THIS_MODULE;

        if(cdev_add(mycdev, mydev,1)<0)
        {
                device_destroy(myclass,mydev);
                class_destroy(myclass);
                unregister_chrdev_region(mydev,1);
                return -EBUSY;
        }

}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

