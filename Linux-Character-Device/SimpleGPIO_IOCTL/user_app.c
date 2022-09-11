#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/fcntl.h>
#include<sys/ioctl.h>

#define DEVICE_NAME "/dev/ioctl_gpiodev"
#define MY_IOCTL_NUMBER 102


#define MY_IOCTL_PIN27  _IO(MY_IOCTL_NUMBER, 0)
#define MY_IOCTL_PIN17  _IO(MY_IOCTL_NUMBER, 1)
#define MY_IOCTL_NR     3

int main()
{
        int dev;
        int data=100;

        dev = open(DEVICE_NAME, O_RDWR);
        if(dev<0) exit(EXIT_FAILURE);

        printf("Start IOCTL GPIO Device\n");
        while(1)
        {
                ioctl(dev, MY_IOCTL_PIN17);
                sleep(2);
                ioctl(dev, MY_IOCTL_PIN27);
                sleep(2);
        }
        close(dev);
        return 0;
}
