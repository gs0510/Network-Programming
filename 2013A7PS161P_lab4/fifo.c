#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
int main()
{
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH|S_IWGRP;
	int rfd[4],wfd[4],status[5],fifo[5];
	mkfifo("fifo1",mode);
	mkfifo("fifo2",mode);
	mkfifo("fifo3",mode);
	mkfifo("fifo4",mode);
	mkfifo("fifo5",mode);
	fifo[0]=open("fifo1",O_RDONLY|O_NONBLOCK);
	fifo[1]=open("fifo2",O_RDONLY|O_NONBLOCK);
	fifo[2]=open("fifo3",O_RDONLY|O_NONBLOCK);
	fifo[3]=open("fifo4",O_RDONLY|O_NONBLOCK);
	fifo[4]=open("fifo5",O_RDONLY|O_NONBLOCK);
	pid_t ret,ret1,ret2,ret3,ret4,ret5;
	ret = fork();
	if(ret ==0)
	{
		wfd[0] = open("fifo1",O_WRONLY);
		if(wfd[0]==-1) printf("error");
		dup2(wfd[0],STDOUT_FILENO);
		execl("/bin/ls","ls","-l",(char*)0);		
	}
	wait(NULL);
	char buff;
	int c = read(fifo[0],&buff,1);
	fifo[1] = open("fifo2",O_WRONLY);
	fifo[4] = open("fifo5",O_WRONLY);
	while(c!=0)
	{
		write(fifo[1],&buff,1);
		write(fifo[4],&buff,1);
		c=read(fifo[0],&buff,1);
	}
	close(fifo[1]);
	close(fifo[4]);
	ret2 = fork();
	if(ret2==0)
	{
		rfd[0] = open("fifo5",O_RDONLY|O_NONBLOCK);
		wfd[2] = open("fifo3",O_WRONLY); 
		dup2(rfd[0],STDIN_FILENO);
		dup2(wfd[2],STDOUT_FILENO);
		execl("/bin/grep","grep","^d",(char*)0);
	}
	wait(NULL);
	ret3 =fork();
	if(ret3==0)
	{
		rfd[1] = open("fifo2",O_RDONLY |O_NONBLOCK);
		wfd[3] = open("fifo4",O_WRONLY); 
		dup2(rfd[1],STDIN_FILENO);
		dup2(wfd[3],STDOUT_FILENO);
		execl("/bin/grep","grep","^-",(char*)0);
	}
	wait(NULL);
	ret4 =fork();
	if(ret4==0)
	{
		rfd[4] = open("fifo3",O_RDONLY|O_NONBLOCK);
		dup2(rfd[4],STDIN_FILENO);
		execlp("wc","wc","-l",(char*)0);
	}
	wait(NULL);
	ret5 = fork();
	if(ret5==0)
	{
		rfd[5] = open("fifo4",O_RDONLY |O_NONBLOCK);
		dup2(rfd[5],STDIN_FILENO);
		if(execlp("wc","wc","-l",(char*)0)==-1)printf("bahahha\n");
	}
	wait(NULL);
	close(fifo[0]);
	close(fifo[1]);
	close(fifo[2]);
	close(fifo[3]);
	close(fifo[4]);
	remove("fifo1");
	remove("fifo2");
	remove("fifo3");
	remove("fifo4");
	remove("fifo5");
	return 0;
}