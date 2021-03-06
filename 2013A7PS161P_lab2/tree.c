#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int k=0;
main(int argc,char *argv[])
{
	int n = atoi(argv[1]);
	int i,j,status,pid[n],last=n;
	printf("Level\tpid\tppid\tPosition\n");
	while(last>0)	
	{
		int flag=0;
		for(i=0;i<last;i++)
		{
			pid[i]=fork();
			k++;			
			if(pid[i]==0)
			{		
				k=i+1;		
				break;			
			}	
		}
		
		for(j=0;j<last;j++)
		{
			if(pid[j] <= 0)
			{
				flag=1;
			}		
		}
		pid_t pid1;
		if(flag==0)		
		{	
			while( pid1 = waitpid(-1,NULL,0)){
				if(errno ==ECHILD) break;	
			}
			printf("\nAll Children Exited\n");					
			if(last!=n){
				printf("%d\t%d\t%d\t%d\n",n-last,getpid(),getppid(),k-last);
			}								
			break;		
		}
		last--;
	}
	if(last==0)
	{
		printf("%d\t%d\t%d\t%d\n",n,getpid(),getppid(),1);
	}
	if(last < n)
	{
		exit(0);
	}
	printf("%d\t%d\t%d\t%d\n",0,getpid(),getppid(),1);
}
