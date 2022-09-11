#include<stdio.h>
#include<unistd.h>
#include<sys/fcntl.h>
#include<signal.h>
#include<string.h>

#define MY_SIGNAL 34

void sig_handler(int signo);

int main()
{
	int dev;
	int pid;
	struct sigaction act;
	
	printf(“My GPIO Character Device Test\n”);

	dev = open(“dev/simple_gpiodev”, O_RDWR);

	printf(“dev : %d\n”,dev);
	
	if(dev<0)
	{
		printf(“device file open error\n”);
		return -1;
	}
	
	pid = getpid();

   printf(“pid : %d\n”, pid);
	
	write(dev, (char *)&pid,sizeof(int));

	memset(&act, ‘\0’, sizeof(act));
	act.sa_handler = &sig_handler;
	if(sigaction(MY_SIGNAL, &act,NULL)<0)
	{
		perror(“sigaction”);
		return 1;
	}

	while(1)
	{
	}
}

void sig_handler(int signo)
{
	printf(“I Received MY_SIGNAL(%d)\n”, MY_SIGNAL);
}

