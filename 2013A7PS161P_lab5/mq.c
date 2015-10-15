#include <time.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#define MAX 256
struct my_msgbuf
{
	long mtype;
	int mtext;
};

int n,msqid,msg_count;
pid_t arr[MAX];
pid_t parent_pid;

void sigalrm_handler(int signo)
{
	//child sending message to parent every 5 seconds
	struct my_msgbuf buf;	
	buf.mtype=parent_pid;
	buf.mtext=rand();
	if (msgsnd (msqid, &buf, sizeof (buf), 0) == -1) perror ("msgsnd");
	printf("sent by %d the random integer %d\n",getpid(),buf.mtext);
	alarm(5);	
}

//signal handler to kill all child processes and exit
void sigint_handler(int signo)
{
	int i;
	for(i=0;i<n;i++)
	{
		kill(arr[i],SIGINT);
	}
	printf("Total messages sent- %d\n", msg_count/n);
	msgctl(msqid,IPC_RMID,NULL);
	exit(1);
}

int main(int argc,char* argv[])
{
	parent_pid = getpid(); //the pid of the process
	n = atoi(argv[1]); 
	pid_t ret;
	
	//generating a message queue
	key_t key;
	if((key = ftok ("mq.c", 'B')) == -1){perror ("ftok");}
	msqid=msgget(key,IPC_CREAT|S_IRUSR|S_IWUSR);
	
	//initialising for rand function
	time_t t;
	srand((unsigned) time(&t));

	//generating child processes
	int i;
	for(i=0;i<n;i++)
	{
		ret=fork();
		//if parent then add child pid to array
		if(ret>0)
		{
			arr[i]=ret;
		}
		else if(ret==0)
		{
			//child sending the signal to parent using alarm()
			signal(SIGALRM,sigalrm_handler);
			if((key = ftok ("mq.c", 'B')) == -1){perror ("ftok");}
			msqid=msgget(key,S_IRUSR|S_IWUSR);
			alarm(5);
			//child receving messages of its own type from queue
			while(1)
			{
				struct my_msgbuf buf;
				if(msgrcv(msqid,&buf,sizeof(buf),getpid(),0)>=0)
				printf("received by %d the random integer %d\n",getpid(),buf.mtext);	
			}
		}
	}
	msg_count =0;
	signal(SIGINT,sigint_handler);
	//parent receving message of its tye and sending the same message to all its children
	while(1)
	{
		struct my_msgbuf buf;
		if(msgrcv(msqid,&buf,sizeof(buf),getpid(),0)==-1)perror("msgrcv");
		msg_count+=n;
		for(i=0;i<n;i++)
		{
			buf.mtype=arr[i];
			if(msgsnd(msqid,&buf,sizeof(buf),0)==-1)perror("msgsnd");
		}
	}
	while(1)pause();
}