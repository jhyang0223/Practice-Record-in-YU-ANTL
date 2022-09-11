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
#define DRIVER_DESC     "Test sending signal triggered by gpio."

#define DEVICE_NAME     "simple_gpiodev"
#define READ_BUFF_LEN   6

#define GPIO_INPUT_PIN  27
#define GPIO_INPUT_PIN_DESC     "BCM 27"
#define GPIO_INPUT_PIN_DEVICE_DESC      "Signal trigger gpio pin."

#define GPIO_OUTPUT_PIN 17
#define GPIO_OUTPUT_PIN_DESC "BCM 17"

#define MY_SIGNAL       34

/Module params declaration block
short int gpio_pin = GPIO_INPUT_PIN;

//Interrupts variables block
short int irq_gpio_pin = GPIO_INPUT_PIN;
short int irq_enabled = 0;

//Function variables block
pid_t send_to_pid = 0; //store pid of process
short int signal_to_pid = MY_SIGNAL;

//device registration variables
struct class *myclass;
struct cdev *mycdev;
struct device *mydevice;
dev_t mydev;

int simple_gpiodev_open(struct inode *, struct file *);
int simple_gpiodev_release(struct inode *, struct file *);

ssize_t simple_gpiodev_write(struct file *, const char *, size_t, loff_t *);

struct file_operations fops = {
        .write = simple_gpiodev_write,
        .open = simple_gpiodev_open,
        .release = simple_gpiodev_release
};

static irqreturn_t my_irq_handler(int irq, void* dev_id);
void my_int_config(void);
void my_int_release(void);

int find_my_task(int task_pid);
void signal_config(void);

int irq_gpio_input = 0;

//Signal variables
struct task_struct *t;
struct siginfo info;

int simple_gpiodev_open(struct inode *inode, struct file *file)
{
        printk(KERN_ALERT "[simple_gpiodev] device open\n");
        return 0;
}

int simple_gpiodev_release(struct inode *inode, struct file *file)
{
        my_int_release();
        printk(KERN_ALERT "[simple_gpiodev] device release\n");
        return 0;
}

int mod_init(void)
{
        if(alloc_chrdev_region(&mydev,0,1,"simple_gpiodev")<0)
                return -EBUSY;

        myclass = class_create(THIS_MODULE, "simple_gpiodev");
        if(IS_ERR(myclass))
        {
                unregister_chrdev_region(mydev,1);
                return PTR_ERR(myclass);
        }

        mydevice = device_create(myclass, NULL, mydev, NULL, "simple_gpiodev");
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

static irqreturn_t my_irq_handler(int irq, void *dev_id)
{
        unsigned long flags;
        int curVal;
        int ret;

        local_irq_save(flags);

        curVal = gpio_get_value(GPIO_OUTPUT_PIN);
        curVal = 1-curVal;
        gpio_set_value(GPIO_OUTPUT_PIN, curVal);

        ret = send_sig_info(MY_SIGNAL, &info, t);
        if(ret<0)
        {
                printk("error sending signal!!\n");
                return ret;
        }


        printk(KERN_ALERT "[simple_gpiodev] Interrupt [%d] for device %s was triggered \n", irq, (char *)dev_id);

        local_irq_restore(flags);

        return IRQ_HANDLED;
}

int find_my_task(int task_pid)
{
        t = pid_task(find_vpid(task_pid),PIDTYPE_PID);
        if(t==NULL)
        {
                printk("no such pid \n");
                return -1;
        }
        return 0;
}

void signal_config(void)
{
        memset(&info, 0, sizeof(struct siginfo));
        info.si_signo = MY_SIGNAL;
        info.si_code = SI_QUEUE;
}

void my_int_release(void)
{
        free_irq(irq_gpio_input, GPIO_INPUT_PIN_DEVICE_DESC);
        gpio_free(GPIO_INPUT_PIN);
        gpio_free(GPIO_OUTPUT_PIN);
}

void mod_exit(void)
{
        cdev_del(mycdev);
        device_destroy(myclass, mydev);
        class_destroy(myclass);
        unregister_chrdev_region(mydev,1);
}

void my_int_config(void)
{
        if(gpio_request(GPIO_INPUT_PIN, GPIO_INPUT_PIN_DESC))
        {
                printk(KERN_ALERT "[simple_gpiodev] GPIO request failure %s\n", GPIO_INPUT_PIN_DESC);
                return ;
        }

        if(gpio_request(GPIO_OUTPUT_PIN, GPIO_OUTPUT_PIN_DESC))
        {
                printk(KERN_ALERT "[simple_gpiodev] GPIO request failure %s\n", GPIO_OUTPUT_PIN_DESC);
                return ;
        }

        if(gpio_direction_input(GPIO_INPUT_PIN))
        {
                printk(KERN_ALERT "[simple_gpiodev] GPIO direction configuration failure :%s\n", GPIO_INPUT_PIN_DESC);
                return ;
        }
        if(gpio_direction_output(GPIO_OUTPUT_PIN,0))
        {
                printk(KERN_ALERT"[simple_gpiodev] GPIO direction configuration failure : %s\n", GPIO_OUTPUT_PIN_DESC);
                return ;
        }

        irq_gpio_input = gpio_to_irq(GPIO_INPUT_PIN);
        printk(KERN_ALERT"[simple_gpiodev] Mapped int %d\n", irq_gpio_input);

        if(irq_gpio_input<0)
        {
                printk(KERN_ALERT"[simple_gpiodev] GPIO to IRQ mapping failure  %s\n", GPIO_INPUT_PIN_DESC);
                return ;
        }


        if(request_irq(irq_gpio_input,(irq_handler_t)my_irq_handler, IRQF_TRIGGER_FALLING, GPIO_INPUT_PIN_DESC, GPIO_INPUT_PIN_DEVICE_DESC))
        {
                printk(KERN_ALERT "[simple_gpiodev] IRQ request failure\n ");
                return ;
        }

}

ssize_t simple_gpiodev_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
        int pid;

        copy_from_user(&pid, buf, sizeof(count));

        printk(KERN_ALERT "[simple_gpiodev] device write function called! pid = %d",pid);

        find_my_task(pid);
        signal_config();
        my_int_config();

        return count;
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
