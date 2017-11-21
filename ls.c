#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<time.h>
#include<grp.h>
#include<pwd.h>
#include<fnmatch.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include<netdb.h>
#include<sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include<semaphore.h>


#define BUFFSIZE 1024
#define PORTNO 40041
#define KEY_NUM 40041
#define MEM_SIZE 1024


 

pthread_mutex_t counter_mutex =  PTHREAD_MUTEX_INITIALIZER;//initalizer
sem_t *sem;
 

int cnt=0;//idle node count variable
int ing=0;

char p_path[256];
int socket_fd;//socket_fd variable
int addrlen;//addrlen variable
int child_flag;//child_flag variable
int M_child;//MaxChilds vairable
int Max_server;//MaxSpareServers variable
int Min_server;//MInSpareServers variable
int S_server;//StartServers variable
char print_time[100];


typedef struct node{
	int pid;//child process ID
	int status;//child process status
	int port;//port
	char ip[100];//client ip
	struct node * next;//next node pointer
}NODE;


char * client_ip;//client_ip
in_port_t fork_port;//fork_port
time_t now;//for time print
NODE * pHead;//linkedlist head
pid_t PPID;//parent PID
void * doit3(void* buf)//for writing at logfile
{
	char * f_buf;// char pointer for writing file
	sem=sem_open("40041",O_RDWR);// semaphore open
	sem_wait(sem);//lock
	FILE * file=fopen("access.log","a");
	f_buf=(char *)buf;
	//printf("362:%s\n",f_buf);
	fprintf(file,"%s",f_buf);
	fclose(file);
	sem_post(sem);//unlock
	sem_close(sem);//close
	return NULL;//retrun
	


}
void child_main(int i, int socketfd, int addrlen);//child_main
void insert(int Pid){//insert
	NODE * pNew=(NODE *)malloc(sizeof(NODE));// make new node
	NODE * p=pHead;	//p is set by pHead
	pNew->pid=Pid;//pNew's pid is set by Pide
	pNew->next=NULL;//pNew's next is set by NULL
	pNew->status=0;

	if(pHead==NULL)//pHead is NULL
		pHead=pNew;//pHead is set by pNew
	else//pHead is not NULL
	{
		pNew->next = pHead;//pNew's next is pHead
		pHead = pNew;//pHead is set by pNew
	}
	

}

void delete(void){//linekdslit delete function

	NODE * p;//NODE variable
	p=pHead;//p is set by pHead
	while(p)//loop 
	{	
		pHead=p->next;//pHead is set by p's next
		free(p);//free p
		p=pHead;//p is set by pHead	

	}

	
}
void time_log(void)//time print function
{
	int i;// int vairable
	memset(print_time,0,100);//reset print_time
	time(&now);// time store at now
	
	strcpy(print_time,ctime(&now));//currnet time copy at print_time 
	for(i=0; print_time[i]!='\n'&&print_time[i]!='\0'; i++);
			
	if(print_time[i]=='\n')//enter remove at print_time array
		print_time[i]='\0';
}
void idle_count(void)//idle node count function
{
	NODE *p=pHead;//p is set by pHead 
	cnt=0;//reset
	while(p)
	{
		if(p->status==0)// p's status is idle
			cnt++;// idle node count is increased
		p=p->next;// p is set by p's next node
	}
}

struct mynode {//mynode struct
char fname[256];//file anme
char permission[256];//permission
int linkcounter;//linkcounter
char u_ID[256];//user id
char g_ID[256];//group id
int CAPACITY;//capacity
int mon;//month
int day;//day
int hour;//hour
int min;//min
int total;//total
} ;

void *doit1(void *vptr)// write shared data function

{

	int  shm_id;// shm_id variable
	void *shm_addr;// shm_addr viarable

   	

	if((shm_id = shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|0666))==-1)//if shmget fail
	{
		printf("shmget fail\n");//print error msg
		return NULL;//return NULL
	}

	if((shm_addr = shmat(shm_id, (void*)0, 0))==(void *)-1)//if shmat is fail
	{
		printf("shmat fail\n");      //print error msg
		return NULL;//return NULL
   	}

   
	//pthread_mutex_lock(&counter_mutex);// mutex lock function
	sem=sem_open("40041",O_RDWR);// semaphore open
	sem_wait(sem);//???????????????
	//sleep(1);
	sprintf((char*)shm_addr,"%d/%s/%d",getpid(),client_ip,fork_port);// store child process pid & client_ip & port at shm_addr
	
	//pthread_mutex_unlock(&counter_mutex);//mutex unlock function
	//sleep(1);
	sem_post(sem);
	sem_close(sem);

 	ing=1;

	return NULL;//return null

 

 

}

void *doit2(void *vptr)//read shared data function

{

	int shm_id, i;// shm_id , i variable
	void *shm_addr;//shm_addr vairable
	int child_pid;//child_pid vairable
	int portnum;
	NODE * p;//NODE * P
	char memory[500];//shared data array
	char temp[100];//temp array
	int temp_i;// temp_i variable
	NODE * pPrev;//p's Prev node vairable
	//char print_time[100];
	int num;
	char logbuf[1000];
	pthread_t M_tid;

	if((shm_id=shmget( (key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|0666))==-1)//if shmet error
	{
		printf("shmget fail\n");//print error msg
		return NULL;//return NULL
	}

	if((shm_addr= shmat(shm_id, (void *)0, 0))==(void *)-1)//if shmat error
	{
		printf("shmat fail \n");//print error msg
		return NULL;
	}

	//pthread_mutex_lock(&counter_mutex);
	strcpy(memory,(char *)shm_addr);// shared memory copy
	//pthread_mutex_unlock(&counter_mutex);

	for(i=0; memory[i]!='/' ;i++)//store process pid at temp by parsing memory 
		temp[i]=memory[i];
	temp[i]='\0';
	temp_i=i+1;//store i at temp_i

	child_pid=atoi(temp);//store child_pid
	
	p=pHead;// p is set by pHead
	while(p)
	{
		if(p->pid==child_pid )//if p's pid equal child_pid 
			break;//break
	
		p=p->next;//p is set by p's next node

		
	}
	if(p->status==0)// p's stauts is zero
	p->status=1;//p's status is changed to 1
	else// p's status is 1
	p->status=0;//p's status is changed to zero
	memset(temp,0,100);//memset temp
	for(i=temp_i ; memory[i]!='/'; i++)// parsing client_ip 
		temp[i-temp_i]=memory[i];
	temp[i-temp_i]='\0';

	
	temp_i=i+1;
	for(i=0; temp[i]!='\0'; i++)//parsing port
		p->ip[i]=temp[i];
	p->ip[i]='\0';

	memset(temp,0,100);//memset temp
	for(i=temp_i ; memory[i]!='\0'; i++)// parsing port num
		temp[i-temp_i]=memory[i];
	temp[i-temp_i]='\0';
	portnum=atoi(temp);// store port num
	p->port=child_pid;// p's port is set by child_pid
	/*
	p=pHead;
	cnt=0;
	while(p)
	{
		if(p->status==0)
			cnt++;
		p=p->next;
	}
	time(&now);// time store at now
	strcpy(print_time,ctime(&now));//currnet time copy at print_time 
	for(i=0; print_time[i]!='\n'&&print_time[i]!='\0'; i++);
			
	if(print_time[i]=='\n')//enter remove at print_time array
		print_time[i]='\0';*/

	if(p->status==0)//disconnected
	{/*
		sprintf(logbuf,"================Disconnected client================\n");//print disconnected
		time_log();
		p_path=(char *)vptr;
		printf("p_path:%s\n",p_path);
		sprintf(logbuf,"%s%s %s\n",logbuf,print_time,p_path);//print time		
		sprintf(logbuf,"%sIP:%s\n",logbuf,p->ip);//print client IP
		sprintf(logbuf,"%sPort: %d \n",logbuf,p->port);//print client port
		sprintf(logbuf,"%s===================================================\n",logbuf);	
		pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);*/	
	}
	idle_count();//idle count setting
	time_log();//time setting
	sprintf(logbuf,"[%s] idleServerCount: %d\n",print_time,cnt);//print idle node count 
	temp_i=0;//reset 
	p=pHead;//p is set by pHead
	while(p)//loop 
	{
		p=p->next;// p is set by p's next node
		temp_i++;// temp_i is increased
	}
	if(temp_i<M_child && cnt<Min_server)// if temp_i is smaller than Max child process && idle count is smaller than Min idle server
	{
		num=S_server-cnt;
		for(i=0; i<num; i++)
		{
			child_pid=child_make(0,socket_fd,addrlen);//pids[i] is set by child_make return value	
			insert(child_pid);	
			sprintf(logbuf,"%s[%s] %d process is forked.\n",logbuf,print_time,child_pid);//print "process is forked"
			sprintf(logbuf,"%s[%s] idleServerCount: %d\n",logbuf,print_time,cnt+1);//print idleserver count
			cnt++;//cnt is increased 
		}
		
	}
	if(cnt>Max_server)//if temp_i is larger than Max_server
	{
		cnt-=S_server;// temp_i is set by S_server
		for(i=0; i<temp_i; i++)////////idle node delete 
		{
			p=pHead;
			while(p)
			{
				if(p->status==0)// if p's status is zero
					break;//break

				pPrev=p;//pPrev is set by p
				p=p->next;//p is p's next

			}
			if(p==pHead)//if p is pHead
			{
				pHead=p->next;// pHead is set by p's next
				p->next=NULL;//p's next is NULL
			}
			else//if p is not head
			{
				pPrev->next=p->next;// pPrev's next is set by p's next
				p->next=NULL;//p's next is NULL

			}
			kill(p->pid,SIGTERM);// kill p node 
			free(p);//free p node
			idle_count();//idle count setting
			sprintf(logbuf,"%s[%s] idleServerCount: %d\n",logbuf,print_time,cnt);//print idle count 
			
			
		}
		

	}
	

	if(shmctl(shm_id, IPC_RMID, 0) == -1)//if  shtml error 
	{
      		sprintf(logbuf,"%sshmctl fail\n",logbuf);//print error msg
	}
	else
	{
		sprintf(logbuf,"%s shared memory delete success\n",logbuf);
	}
	pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);	
	kill(child_pid,SIGUSR2);	

 	return NULL;//return NULL

}
void sig_handler(int sig)//signal function
{
	pid_t PID;//PID variable
	NODE * p;//NODE pointer
	NODE * pPrev=NULL;//NODE pointer 
	int i=0;//vairable
	int k=0;//variable
	//char print_time[100];//for time print
	pthread_t tidA;
	pthread_t M_tid;
	char logbuf[1000];
	if(sig==SIGCHLD)//if signal number=SIGCHLD
	{
		if(child_flag==0)//if child_flag is zero
		{
			time_log();
			PID=wait(NULL);//wait
			sprintf(logbuf,"[%s] %d process is terminated.\n",print_time,PID);//print "process is terminated"
			/////////////////////delete terminated node at linkedlist///////////////////	
			p=pHead;//p is set by pHead
			i=0;//i is set by zero
			while(p)//loop while p does exist
			{
				if(p->pid==PID)// p's pid is equal PID
				break;//break
				pPrev=p;//pPrev is set by p
				p=p->next;//p is set by p's next
				i++;// i is set by i+1
			}

			if(p==pHead)// if p is pHead
				pHead=p->next;//pHead is set by p's next
			
			else//if p isn't pHead
				pPrev->next=p->next;//pPrev's next is set by p's next	
			time(&now);//store time at now
		sprintf(logbuf,"%s================Disconnected client================\n",logbuf);//print disconnected
		sprintf(logbuf,"%s%s",logbuf,ctime(&now));//print time		
		sprintf(logbuf,"%sIP:%s\n",logbuf,p->ip);//print client IP
		sprintf(logbuf,"%sPort: %d \n",logbuf,p->port);//print client port
		sprintf(logbuf,"%s===================================================\n",logbuf);		
		pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);	
		free(p);//delete p	
		/*
			for( ;i>0 ; i--)
			pids[i]=pids[i-1];//pids[i] is set by pids[i-1]
		
		
			pids[0]=child_make(0,socket_fd,addrlen);//pids[0]  is set by child_make return value
			insert(pids[0]);//insert linkedlist pids[0]
		
			printf("[%s] %d process is forked.\n",print_time,pids[0]);//print "process is forked"
		*/	
		}
		
	}
	else if(sig==SIGINT)
	{	
			NODE * pp;	
			sprintf(logbuf,"\n");
			child_flag=1;//flag setting for SIGCHLD
			p=pHead;
			while(p){
			p->status=0;
			p=p->next;
			}
			p=pHead;
			idle_count();
			while(p){
				//idle_count();
				
				
				

				kill(p->pid,SIGTERM);// send SIGTERM signal at pids[i]
				wait(NULL);//wait
				
				time_log();

				sprintf(logbuf,"%s[%s] %d process terminated.\n",logbuf,print_time,p->pid);//print "process terminated"
				sprintf(logbuf,"%s[%s] idleServerCount :%d\n",logbuf,print_time,cnt-1);//print idle process print
				p=p->next;
				cnt=cnt-1;
		
				
			}
			delete();//delete linkedlist
	
			time_log();
			sprintf(logbuf,"%s[%s] Server is terminated.\n",logbuf,print_time);//print "Server is terminated"
			pthread_create(&M_tid,NULL,&doit3,logbuf);
			pthread_join(M_tid,NULL);	
			close(socket_fd);//close socket
			sem_unlink("40041");
			exit(0);//exit parent process
			
	}
	else if(sig==SIGUSR1)
	{
	
		pthread_create(&tidA, NULL, &doit2, p_path);
		pthread_join(tidA,NULL);
		
		
	}
	else if(sig==SIGUSR2)
	{
		if(getpid()==PPID )
		{
			pthread_create(&tidA, NULL, &doit2, NULL);
			pthread_join(tidA,NULL);
		}
		else
		{
			ing=0;
		}
	}
	
	
	
}
void swap(struct mynode *swap1,struct mynode *swap2)//struct data swap
{
	struct mynode swap3;// mynode variable
	swap3=*swap1; //swap
	*swap1=*swap2;
	*swap2=swap3;	
}

void sorting ( struct mynode arr[1000], int idx ,int flag);	//sorting function

void WriteData(int client_fd,char * req_path, char * host)
{
	DIR *dirp;			//DIR variable
	struct dirent *dir;		//struct dirent variable
	struct stat fstat;
	int i=0;			//int variable
	int k=0;			//int variable
	int j=0;			//int variable
	int a=0;			//int variable
	int b=0;			//int variable
	int c=0;			//int variable
	char info[10000];		//variable for information of file
	char path[10000];		//file path virable
	int total=0; 			//total virable	
	struct mynode  arr[1000];	//struct variable for relative path
 	char ab_path[1000][256];	//absolute path 
	int ab_flag=0;			//absolute path flag
	int ab_count=0;			//absolute path count
	int opt;
	char print_buf[500];
	

	if(req_path!=NULL)//absolute path
	{	
		ab_flag=1;
		strcpy(ab_path[0],req_path);
		for(c=0; ab_path[0][c]!='\0'; c++);
			
		if(ab_path[0][c-1]=='/') // deal with exception about parent director ".." hyper link error
		ab_path[0][c-1]='\0';
		 a++;			
	}

	
	
	ab_count=a;					// absolute path count
	
	c=0;
	i=0;
	total=0;
	
	for(a=0;a<1000; a++)		//initialize
	{
	
		memset(arr[a].fname,0,256); 	//reset fname
		memset(arr[a].permission,0,256);//reset permission
		memset(arr[a].u_ID,0,256);	//reset u_ID
		memset(arr[a].g_ID,0,256);	//reset g_ID
		arr[a].linkcounter=0;		//reset linkcounter
		arr[a].CAPACITY=0;		//reset CAPACITY
		arr[a].mon=0;			//reset month
		arr[a].day=0;			//reset day
		arr[a].hour=0;			//reset hour
		arr[a].min=0;			//reset min
		arr[a].total=0;			//reset total
		
	}
	
		
	if(ab_count==0) // not hyper link or path
	dirp=opendir(".");   
	else // if hyper link or path exists
	dirp=opendir(ab_path[0]);
		
	while((dir=readdir(dirp))!=NULL)//whie start
	{
		memset(info,0,10000);//reset
		if(ab_count==0)
		getcwd(info,10000);
				
		else
		strcpy(info,ab_path[0]);
			
		strcat(info,"/"); //info+/
		strcat(info,dir->d_name);// info+file name
		c=0;
						
				
		stat(info,&fstat);//store st_info in info
		switch((fstat.st_mode)&(S_IFMT))//decide   directory or file 
		{
			case S_IFREG://if file
					arr[i].permission[c++]='-';//store '-'
					break;
		
			case S_IFDIR://if directory
					arr[i].permission[c++]='d';//store 'd'
					break;
			case'?'://default
					break;
		}
		
		for(k=0; k<3; k++)
		{
			if(fstat.st_mode&(S_IREAD>>k*3))	//if read enable
			arr[i].permission[c++]='r';		//r store
			else					//if read not enable
			arr[i].permission[c++]='-';		//- store
			if(fstat.st_mode&(S_IWRITE>>k*3))	//if write enable
			arr[i].permission[c++]='w';		//w store
			else					//if write not enable
			arr[i].permission[c++]='-';		//- store
			if(fstat.st_mode&(S_IEXEC>>k*3))	//if execution enable
			arr[i].permission[c++]='x';		//x store
			else					//if execution not enable
			arr[i].permission[c++]='-';		//- store
		}


		arr[i].permission[c]='\0';			//store NULL
		arr[i].linkcounter=fstat.st_nlink;		//store linkcounter
		arr[i].CAPACITY=fstat.st_size;			//store st_size
		strcpy(arr[i].u_ID,getpwuid(fstat.st_uid)->pw_name);//store u_ID
		strcpy(arr[i].g_ID,getgrgid(fstat.st_gid)->gr_name);//store g_ID	
		arr[i].mon=localtime(&fstat.st_mtime)->tm_mon+1;//store month
		arr[i].day=localtime(&fstat.st_mtime)->tm_mday;//store day
		arr[i].hour=localtime(&fstat.st_mtime)->tm_hour;//store hour
		arr[i].min=localtime(&fstat.st_mtime)->tm_min;//store min
		arr[i].total=fstat.st_blocks;//store file size
		total+=arr[i].total;
		strcpy(arr[i].fname,dir->d_name);//strcpy dir->d_name at arr[i].fname
				

		i++;//i+1
	}//while end


			
		
	a=0;b=0;
				
	for( j=1;  j<i; j++)				// use for check where '.' is.
	{						// if '.' is not array first
		if(arr[j].fname[0]=='.'&& arr[j].fname[1]=='\0')
		{
			b=1;					//flag up
			break;
		}
	}
	if(b==1)				//if flag up=> '.' is not first of the array
	swap(&arr[0],&arr[j]);//swap

	a=0,b=0;								//reset
		
	for(j=2 ;  j<i; j++)							// use for check where '..' is
	{
		if(arr[j].fname[0]=='.' && arr[j].fname[1]=='.' && arr[j].fname[2]=='\0')	// if '..' is not array second
		{
			b=1;								//flag up
			break;
		}
	}

	if( b==1)				//if flag up=> '..' is not second of the array
	swap(&arr[1],&arr[j]);//swap
										
	a=1; b=3;
				
	for(j=b; j<i; j++)//sorting
	sorting(arr,j,a);
					
	closedir(dirp);//close directory
					 
	memset(info,0,10000);//reset info
	getcwd(info,10000);//get current working directory at info
	if(ab_count!=0)// if hyper link or path exist
	{
		strcat(info,"/");
		strcat(info,ab_path[0]);
	}
	

	write(client_fd,"<html><head><title>",strlen("<html><head><title>"));//make html
	write(client_fd,info,strlen(info));
	write(client_fd,"</title></head><body><h3>",strlen("</title></head><body><h3>"));
						
	
		
	a=0,b=0;
		
				
	sprintf(print_buf,"Directory path:%s <br>",info);//store directory path at print_buf
	write(client_fd,print_buf,strlen(print_buf));// print print_buf at client descriptor					
	write(client_fd,"</h3>\n",strlen("</h3>\n"));
	sprintf(print_buf,"total:%d\n",total/2);//store total at print_buf
	write(client_fd,print_buf,strlen(print_buf));//print print_buf at client descriptor
				
	//make table for ls -al option
	write(client_fd,"<table border=\"3\"<tr>",strlen("<table border=\"3\"<tr>"));
	write(client_fd,"<th>Name</th>",strlen("<th>Name</th>"));//name block
	write(client_fd,"<th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th>",strlen("<th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th>"));// -l option block
	write(client_fd,"</tr>",strlen("</tr>"));
	for( b=0;b<i ; b++)// arr struct information print  at table
	{
					
		write(client_fd,"<tr>",strlen("<tr>"));
		memset(path,0,10000);//reset path
		if(ab_count!=0)//relative path or hyper link
		{
					
			strcpy(path,ab_path[a]);//strcpy ab_path[0] at path
			strcat(path,"/");//path + '/'
			strcat(path,arr[b].fname);// path + arr[b].fname 
		
		}
		else //not path	
		strcpy(path,arr[b].fname);
					
		sprintf(print_buf,"<td><a href=\"http://%s/%s\">%s</a></td>",host,path,arr[b].fname);//hyper link about file name
		write(client_fd,print_buf,strlen(print_buf));	//print file name
		sprintf(print_buf,"<td>%s </td><td>%d </td><td>%s </td><td>%s </td><td>%d </td><td>%d %d %d:%d</td>",arr[b].permission,arr[b].linkcounter,arr[b].u_ID,arr[b].g_ID,arr[b].CAPACITY,arr[b].mon,arr[b].day,arr[b].hour,arr[b].min); 
		write(client_fd,print_buf,strlen(print_buf));//print -l option block by using sprintf
	}
						

	write(client_fd,"</tr>",strlen("</tr>"));	
	write(client_fd,"</table><br><br><br>",strlen("</table><br><br><br>"));//table end
	write(client_fd,"</body></html>",strlen("</body></html>"));//end html


}

pid_t child_make(int i,int socketfd, int addrlen)//child make function
{
	pid_t pid;//pid variable
	if((pid=fork())>0)//fork
	return pid;//return pid

	child_main(i,socketfd,addrlen);//child_main function

}
void child_main(int i, int socketfd, int addrlen)//child main function
{

	int len_out;//int vairable
	int client_fd;//client descriptor variable
	char buf[BUFFSIZE];//buf array
	socklen_t clilen;//socklen_t variable
	struct sockaddr_in  client_addr;//sockaddr_in variable
	FILE * file;//file pointer
	char file_buf[256];//file buf array
	char * end;//pointer for file end checking
	char client_buf[BUFFSIZE];//client buf array
	char temp[100];//temp array
	int j,k,a;//int variables
	char host[256];//host array
	int b;//int variable
	//char print_time[100];// array for print time

	signal(SIGTERM,SIG_DFL);//set SIGTERM default
	signal(SIGINT, SIG_IGN);//set SIGINT signal
	
	
	pthread_t tidA;
	pthread_t M_tid;
	char logbuf[1000];
	sem=sem_open("40041",O_CREAT|O_RDWR,0777,1);
	sem_close(sem);
	while(1){// loop for connecting and disconnecting
		
		clilen=addrlen;//clilen is set by addrlen
		client_fd=accept(socketfd,(struct sockaddr*)&client_addr,&addrlen);//accept 
		
		client_ip=inet_ntoa( client_addr.sin_addr);//store client IP
		fork_port=client_addr.sin_port;//store client port
		///////////////////////print New client////////////////////
		/*
		//pthread_create(&tidA,NULL,&doit1,NULL);// make thread
		//pthread_join(tidA,NULL);//join thread
		pthread_create(&tidA,NULL,&doit1,NULL);// make thread
		pthread_join(tidA,NULL);//join thread
		while(1)
		{
			if(ing==1)
				break;
		}
		//ing=0;
	
		kill(PPID,SIGUSR1);
		*/
		file=fopen("accessible.usr","r");// "accessible.usr" file open
		
		if(file==NULL)//if file open fail
		{
			printf("file open error!\n");//error message print
			exit(0);//exit child process
		}
		
		while((end=fgets(file_buf,256,file))!=NULL)//read file 
		{
		
			
			for(b=0; file_buf[b]!='\0' ; b++)// loop for line change removing at file buf
			{
				
				if(file_buf[b]=='\n')//if file_buf[i]='\n'
				file_buf[b]='\0';//file_buf[i] = NULL
			}
			
			
			if(fnmatch(file_buf,client_ip,0)==0){//if file_buf and client IP match
			
				break;//break
			}
			
			else{// not match
				
				continue;//continue
			}
			
		}
		
		if(end==NULL)//if end is NULL, in short, client IP is denied IP
		{
		//sprintf(client_buf,"HTTP/1.1 403.6 FORBIDDEN\r\nContent-Length : 50000\r\nContent-Type: text/html\r\n\r\n");//store response message of request	of client at client_buf	
		sprintf(client_buf,"HTTP/1.1 200 OK\r\nContent-Length : 50000\r\nContent-Type: text/html\r\n\r\n");//store response message of request	of client at client_buf	
		write(client_fd,client_buf,strlen(client_buf));//print client_buf at client descriptor
		write(client_fd,"\n",strlen("\n"));
		write(client_fd,"<html><head><title>",strlen("<html><head><title>"));//make html
		write(client_fd,"</title></head><body><h3>",strlen("</title></head><body>"));
		write(client_fd,"<h3>Access denied!<br>",strlen("Access denied!<br>"));//print error message
		sprintf(client_buf,"Your IP:%s\n</h3>",inet_ntoa(client_addr.sin_addr));//store error IP at client_buf 
		write(client_fd,client_buf,strlen(client_buf));//print error IP 
		write(client_fd,"You have no permission to access this web server.<br>HTTP 403.6-Forbidden:IP address reject<br>",strlen("You have no permission to access this web server.<br> HTTP 403.6-Forbidden:IP address reject<br>"));//print error message
		write(client_fd,"</body></html>",strlen("</body></html>"));
		close(client_fd);//close client_fd
		fclose(file);//close file
		continue;
		}
		
		fclose(file);//close file
		time(&now);
		
		
			
		if((len_out=read(client_fd,buf,BUFFSIZE))>0){//for geting message and print message
			if(strncmp(buf,"GET /favicon.ico",16)==0)// if error request 
			{
				close(client_fd);//close client_fd
				continue;//continue
			}
			
			for(b=0; buf[b]!='/'; b++);//buf count
			memset(host,0,256);//reset host
			for(j=0; ; j++)
			{
				if(buf[j]=='H' && buf[j+1]=='o' && buf[j+2]=='s' && buf[j+3]=='t' && buf[j+4]==':'&& buf[j+5]==' ')//for host parsing
				{
				for(a=0; buf[a+j+6]!='\n';a++)
				{
					host[a]=buf[a+j+6];//store host parsing at host
				}
				host[a]='\0';
				
				break;//break
				}
							

			}
			
			if(buf[b+1]==' ')// not link
			{	
				sprintf(client_buf,"HTTP/1.1 200 OK\r\nContent-Length : 10583\r\nContent-Type: text/html\r\n\r\n");//store response message of request of client at client_buf
				write(client_fd,client_buf,strlen(client_buf));//print client_buf at client descriptor 
				memset(client_buf,0,BUFFSIZE);//reset client_buf
				WriteData(client_fd,NULL,host);//Write function 
				getcwd(buf,BUFFSIZE);
				strcat(buf,"--- 200 OK\n");

			
			}
			else
			{
			
				for(k=b+1; buf[k]!=' ' ;k++)
					temp[k-b-1]=buf[k];//client_buf[k-i-1]=buf[k];
				temp[k-b-1]='\0';//client_buf[k-i-1]='\0';
				memset(client_buf,0,BUFFSIZE);//reset client_buf
				getcwd(client_buf,BUFFSIZE);//get current working directory at client_buf
				strcat(client_buf,"/");//client_buf + '/'
				strcat(client_buf,temp);//client_buf + temp
				strcpy(buf,client_buf);
				
				if(opendir(client_buf)!=NULL)// if client_buf is directory 
				{
					sprintf(client_buf,"HTTP/1.1 200 OK\r\nContent-Length : 50000\r\nContent-Type: text/html\r\n\r\n");// store response message of request of client at client_buf
		
					write(client_fd,client_buf,strlen(client_buf));//print client_buf at client descriptor
					WriteData(client_fd,temp,host);//Write function
					strcat(buf,"---2OO OK\n");
					
				}
				else if((file=fopen(client_buf,"r"))!=NULL)// if client_buf is file
				{
					
					sprintf(client_buf,"HTTP/1.1 200 OK\r\nContent-Length : 50000\r\nContent-Type: text/plain\r\n\r\n");//store response message of request	of client at client_buf	
					write(client_fd,client_buf,strlen(client_buf));//print client_buf at client descriptor
					//write(client_fd,"\n",strlen("\n"));
					while(fgets(file_buf,256,file)!=NULL)//read file 
					{
						write(client_fd,file_buf,strlen(file_buf)); //write entity at client descriptor
					}
					fclose(file);//close file
					strcat(buf,"---2OO OK\n");
				}
				else// error path
				{
				
					sprintf(client_buf,"HTTP/1.1 200 OK\r\nContent-Length: 10000\r\nContent-Type: text/html\r\n\r\n");
					write(client_fd,client_buf,strlen(client_buf));//write client_buf at client descriptor
					write(client_fd,"<html><head><title>",strlen("<html><head><title>"));//make html
					write(client_fd,"ls: error path",strlen("ls: error path"));
					write(client_fd,"</title></head><body>",strlen("</title></head><body>"));
					sprintf(client_buf,"ls: %s error path\n",temp);//store error message at client_buf
					write(client_fd,client_buf,strlen(client_buf));//write client_Buf at client descriptor
					write(client_fd,"</body></html>",strlen("</body></html>"));
					strcat(buf,"---403 Not Forbidden\n");
				
				}

			}
			strcpy(p_path,buf);
			pthread_create(&tidA,NULL,&doit1,NULL);// make thread
			pthread_join(tidA,NULL);//join thread
			while(1)
			{
				if(ing==1)
					break;
			}
			//ing=0;
	
			
			sprintf(logbuf,"=====================New client====================\n");
			time_log();
			sprintf(logbuf,"%s%s %s",logbuf,print_time,buf);//print time
	
			sprintf(logbuf,"%sIP : %s\n",logbuf,client_ip);//print client IP
			sprintf(logbuf,"%sPort : %d\n",logbuf,fork_port); //print client Port
			sprintf(logbuf,"%s===================================================\n",logbuf);
			kill(PPID,SIGUSR1);
			pthread_create(&M_tid,NULL,&doit3,logbuf);
			pthread_join(M_tid,NULL);	
			usleep(10000);
			sprintf(logbuf,"================Disconnected client================\n");//print disconnected
			time_log();
			
			sprintf(logbuf,"%s%s %s",logbuf,print_time,p_path);//print time		
			sprintf(logbuf,"%sIP:%s\n",logbuf,client_ip);//print client IP
			sprintf(logbuf,"%sPort: %d \n",logbuf,fork_port);//print client port
			sprintf(logbuf,"%s===================================================\n",logbuf);	
			pthread_create(&M_tid,NULL,&doit3,logbuf);
			pthread_join(M_tid,NULL);
			


		}//read while end
		
		
		close(client_fd);//close client descriptor
		while(1)
		{
			if(ing==0)
				break;
		}
		pthread_create(&tidA,NULL,&doit1,NULL);//make thread
		pthread_join(tidA,NULL);//make join
		
		while(1)
		{
			if(ing==1)// if ing is 1
				break;//break
		}
		

		kill(PPID,SIGUSR2);// kill sigusr2 to parent
		
	
	}// while(1) end
}

void sorting ( struct mynode arr[1000], int idx , int flag)//sorting function
{
	int i=0;				//int variable
	int j=0;				//int variable
	int k=0;				//int variable
	int start_point=0;			//check for '.' 
	char temp1[256];			//store file name
	char temp2[256];			//store file name
	
	for(i=idx-1; i>flag ; )			// when -a option :arr[0] & arr[1] is alread set, so compare when i>1, when not -a option:compare i>-1
	{
	
	 k=0,j=0;
	 if(arr[i].fname[0]=='.' ||arr[i].fname[0]=='/')				//if hidden file
	  start_point=1;			//set start_point 1
	 else 						//not hidden file
	  start_point=0;			//set start_point 0
	 
	for(k=start_point; arr[i].fname[k]!='\0'; k++)	// store file name  at temp1
	{
	  if(64<arr[i].fname[k]&& arr[i].fname[k]<91)	//if capital 
	   temp1[j]=arr[i].fname[k]+32;		// change to small letter for comparing 
	  else						//not  capital
	   temp1[j]=arr[i].fname[k];		//store temp1[j] at dirlist[i][k]
	 j++;//j++
	}
	 temp1[j]='\0';
	
	 if(arr[idx].fname[0]=='.'||arr[idx].fname[0]=='/')		//if hidden file
	   start_point=1;		//set start_point 1
	 else					//not hidden file
	   start_point=0;		//set start_point 0
	
	j=0;
	for(k=start_point; arr[idx].fname[k]!='\0'; k++)	//store file name at temp2
	{
	 if(64<arr[idx].fname[k] && arr[idx].fname[k]<91)	//if capital 
	  temp2[j]=arr[idx].fname[k]+32;			//change to small letter for comparing
	 else
	  temp2[j]=arr[idx].fname[k];			//store temp2[j]  at dirlist[idx][k]
	 j++;//j++
	}
	temp2[j]='\0';
	
	
	if(strcmp(temp1,temp2)>0)		// temp1> temp2 => sorting 
	{
	swap(&arr[idx],&arr[i]);
	idx--;				//idx=idx-1
  	i--;				//i=i-1
	}
	
	else				// not sorting
	 break;//break
	}		
}



///////////////////////////main////////////////////////////////////////////////////

int main(int argc, char ** argv){
	
	int i=0;			//int variable
	int k=0;			//int variable
	int j=0;			//int variable
	int a=0;			//int variable
	int b=0;			//int variable
	int c=0;			//int variable
	int total=0; 			//total virable	
	
	
	struct sockaddr_in server_addr, client_addr;// server & client variable
	int client_fd;// socket & client socket descriptor
	//int len, len_out;		//int variable
	int opt;			//for exception task
	char  host[256];//for server IP
	struct hostent * h;//variable for server IP
	//char print_time[100];

	FILE * file;//httpd.conf file pointer
	
	char * end;
	char file_buf[256];//file buf
	char temp[100]; // store number for atoi function
	int pids;
	char logbuf[1000];
	pthread_t M_tid;


	

	child_flag=0;//child flag is set by zero
	signal(SIGCHLD,sig_handler);	//SIGCHLD signal setting
	signal(SIGINT,sig_handler);	//SIGINT signal setting
	signal(SIGUSR1,sig_handler);	//SIGUSR1 signal setting
	signal(SIGUSR2,sig_handler);	//SIGUSR2 signal setting

	file=fopen("httpd.conf","r");// file open
	if(file==NULL)//if file open error
	{
		sprintf(logbuf,"file open error!\n");//print error msg
		pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);	
		return 0;//return 0
	}
	
	while((end=fgets(file_buf,256,file))!=NULL)//read file 
	{
	
		for(i=0; file_buf[i]!=':' ; i++)
			temp[i]=file_buf[i];
		temp[i]='\0';
		
		if(strcmp(temp,"MaxChilds")==0)// Maxchilds 
		{
			memset(temp,0,100);//reset temp array
			for(i++ ; file_buf[i]!=' ' && file_buf[i]!='\t'; i++);//parsing
			a=i;
			for( ; file_buf[i]!='\0' && file_buf[i]!='\n'; i++)//parsing
				temp[i-a]=file_buf[i];
			temp[i-a]='\0';
			M_child=atoi(temp);
		}
		else if(strcmp(temp,"MaxSpareServers")==0)// MaxSpareServers
		{
			memset(temp,0,100);//reset temp array
			for(i++ ; file_buf[i]!=' ' && file_buf[i]!='\t'; i++);//parsing
			a=i;
			for( ; file_buf[i]!='\0'&& file_buf[i]!='\n'; i++)//parsing
				temp[i-a]=file_buf[i];
			temp[i-a]='\0';
			Max_server=atoi(temp);
		}
		else if(strcmp(temp,"MinSpareServers")==0)//MinSpareServers
		{
			memset(temp,0,100);//reset temp array
			for(i++ ; file_buf[i]!=' ' && file_buf[i]!='\t'; i++);//parsing
			a=i;
			for( ; file_buf[i]!='\0'&& file_buf[i]!='\n'; i++)//parsing
				temp[i-a]=file_buf[i];
			temp[i-a]='\0';
			Min_server=atoi(temp);
		}
		else if(strcmp(temp,"StartServers")==0)//StartServsers
		{
			memset(temp,0,100);//reset temp array
			for(i++ ; file_buf[i]!=' ' && file_buf[i]!='\t'; i++);//parsing
			a=i;
			for( ; file_buf[i]!='\0'&& file_buf[i]!='\n'; i++)//parsing
				temp[i-a]=file_buf[i];
			temp[i-a]='\0';
			S_server=atoi(temp);
		}

			
	}
	
	time_log();
	sprintf(logbuf,"[%s] Server is started.\n",print_time);//print "Server is started"
	PPID=getpid();//PPID is set parent PID
	pHead=NULL;//pHead is set by NULL

	if((socket_fd=socket(PF_INET,SOCK_STREAM,0))<0) {// socket open error
		sprintf(logbuf,"%sServer:Can't open,stream socket.",logbuf);//print error message
		pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);	
		return 0;//return 
	}
	gethostname(host,sizeof(host));//get host name
	h=gethostbyname(host);//get host ip
	
	time_log();
	sprintf(logbuf,"%s[%s] Socket is created IP:%s port:%d \n",logbuf,print_time, inet_ntoa(*(struct in_addr*)h->h_addr_list[0]),PORTNO);//print Socket IP & port
	
	
	bzero((char *)&server_addr,sizeof(server_addr));//socket reset
	server_addr.sin_family=AF_INET;//set address format
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//s_addr setting by host to network  
	server_addr.sin_port=htons(PORTNO);//port seeting by host to network
	opt=1;
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//for blocking bind error 
	if(bind(socket_fd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){//socket bind
		sprintf(logbuf,"%sServer:Can't bind local address.\n",logbuf);// if socket bind error, print error message
		pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);	
		return 0;
	}
	listen(socket_fd,5);//listen 
	
	addrlen=sizeof(client_addr);//addrlen setting
	
	

	for(i=0; i<S_server; i++){//loop for fork
		
		time_log();
		pids=child_make(i,socket_fd,addrlen);//pids[i] is set by child_make return value
		
		sprintf(logbuf,"%s[%s] %d process is forked.\n",logbuf,print_time,pids);//print "process is forked"
		sprintf(logbuf,"%s[%s] idleServerCount: %d\n",logbuf,print_time,i+1);
		insert(pids);
	}

	pthread_create(&M_tid,NULL,&doit3,logbuf);
		pthread_join(M_tid,NULL);	
	

	for( ; ; )//pause
	pause();


	return 0;//return 


	
}	

