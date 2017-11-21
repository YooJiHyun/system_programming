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
int total;
} ;


void swap(struct mynode *swap1,struct mynode *swap2)//struct data swap
{
	char temp_fname[256];// name temp
	char temp_permission[256];// permission temp
	int temp_linkcounter;//linkcounter temp
	char temp_uID[256];//uID temp
	char temp_gID[256];//gID temp
	int temp_CAPACITY;//CAPACITY temp
	int temp_mon;//month temp
	int temp_day;//day temp
	int temp_hour;//hour temp
	int temp_min;//minute temp
	int temp_total;//total temp

	strcpy(temp_fname,(*swap1).fname);//store swap1's name at temp
	strcpy(temp_permission,(*swap1).permission);//store swap1's permission at temp
	temp_linkcounter=(*swap1).linkcounter;//store swap1's linkcounter at temp
	temp_CAPACITY=(*swap1).CAPACITY;//store swap1's CAPACITY at temp
	strcpy(temp_uID,(*swap1).u_ID);//store swap1's uID at temp
	strcpy(temp_gID,(*swap1).g_ID);//store swap1's gID at temp
	temp_mon=(*swap1).mon;//store swap1's month at temp
	temp_day=(*swap1).day;//store swap1's day at temp
	temp_hour=(*swap1).hour;//store swap1's hour at temp
	temp_min=(*swap1).min;//store swap1's minute at temp
	temp_total=(*swap1).total;//store swap1's total attemp

	strcpy((*swap1).fname,(*swap2).fname);//store swap2's name at swap1's name
	strcpy((*swap1).permission,(*swap2).permission);//store swap2's permission at swap1's permission
	(*swap1).linkcounter=(*swap2).linkcounter;//store swap2's linkcounter at swap1's linkcounter
	(*swap1).CAPACITY=(*swap2).CAPACITY;//store swap2's CAPACITY at swap1's CAPACITY
	strcpy((*swap1).u_ID,(*swap2).u_ID);//store swap2's uID at swap1's uID
	strcpy((*swap1).g_ID,(*swap2).g_ID);//store swap2's gID at swap2's gID
	(*swap1).mon=(*swap2).mon;// store swap2's month at swap1's month
	(*swap1).day=(*swap2).day;//store swap2's day at swap1's day
	(*swap1).hour=(*swap2).hour;//store swap2's hour at swap1's hour
	(*swap1).min=(*swap2).min;//store swap2's minute at swap1's min
	(*swap1).total=(*swap2).total;//store swap2's total at swap1's total

	strcpy((*swap2).fname,temp_fname);//store temp_fname at swap2's fname
	strcpy((*swap2).permission,temp_permission);//store temp_permission at swap2's permission
	(*swap2).linkcounter=temp_linkcounter;//store temp_linkcounter at swap2's linkcounter
	(*swap2).CAPACITY=temp_CAPACITY;//store temp_CAPACITY at swap2's CAPACITY
	strcpy((*swap2).u_ID,temp_uID);//store temp_uID at swap2's temp_uID
	strcpy((*swap2).g_ID,temp_gID);//store temp_gID ats swap2's temp_gID
	(*swap2).mon=temp_mon;//store temp_mon at swap2's mon
	(*swap2).day=temp_day;//store temp_day at swap2's day
	(*swap2).hour=temp_hour;//store temp_hour at swap2's hour
	(*swap2).min=temp_min;//store temp_min  at swap2's min
	(*swap2).total=temp_total;//store temp_total at swap2's total
}

void sorting ( struct mynode arr[1000], int idx ,int flag);	//sorting function
void Sorting ( struct mynode arr[1000], int idx);// ls -S sorting function
void hoption( int num);//ls -h option function(print)


int main(int argc, char ** argv){
	DIR *dirp;			//DIR variable
	struct dirent *dir;		//struct dirent variable
	struct stat fstat;
	int i=0;			//int variable
	int k=0;			//int variable
	int j=0;			//int variable
	int a=0;			//int variable
	int b=0;			//int variable
	int aflag=0, lflag=0;		// option flag
	char *cvalue=NULL;
	int index=0;			// argv index
	int c=0;			//int variable
	char info[10000];		//variable for information of file
	char path[10000];		//file path virable
	int total=0; 			//total virable
		
	struct mynode  arr[1000];	//struct variable for relative path
	
 	char ab_path[1000][256];	//absolute path 
	char r_path[1000][256];		//relative path 
	int ab_flag=0;			//absolute path flag
	int r_flag=0;			//relative path flag
	int ab_count=0;			//absolute path count
	int r_count=0;			//relative path count
	
	int total_loop=0;			//total_loop for while

	
	int match;//fnmatch 
	//struct mynode fm_arr[1000];//struct variable for fmatch directory print
	int err_woption;// error wild optionn
	int w_open;//for counting of '['of [] option 
	int w_close;// for counting of ']' of [] option 
	int hflag;//-h option flag
	int sflag;//-s option flag
	int Sflag;//-S option flag
	int d_count;//directory count
	int temp;//for checking absolute path before wild card
	char temp_str[256];//for storing absolute path before wild card 
	int match_count; //for checking file or directory is founded by using wild card
	int original_ab_count;//to copy orignal ab_count

	

	
	index=0;			//reset
			
	while((c=getopt(argc,argv,"alhsS"))!=-1)// repeat until c=-1  getopt(argc,argv,"al")
	{
	
		switch(c)
		{
			case 'a' :// if -a option
				aflag=1;
				break;
		 	case 'l' ://if -l option
				lflag=1;
				break;
			case 'h':// if -h option 
				hflag=1;
				break;
			case 's'://if -s option
				sflag=1;
				break;
			case 'S'://if -S option
				Sflag=1;
				break;

		
		}//switch 
	}	
	
	index=optind;
	
	
	
	for( a=0,b=0; index<argc; index++)
	{
		
		if((dirp=opendir(argv[index]))!=NULL)//absolute path
		{	
		 	ab_flag=1;
		 	strcpy(ab_path[a],argv[index]);
		 	a++;
			
			
		}
 
		else					// relative path
		{			
			
			r_flag=1;				//flag up
			strcpy(r_path[b],argv[index]);		//path copy
			b++;
			
		}

		closedir(dirp);
	}

	
	d_count=0;// directory count
	ab_count=a;					// absolute path count
	r_count=b;					//relative path count
	
	c=0;
	i=0;

	for(index=1;index<argc; index++)// for find error option
	 {
		if(argv[index][0]=='-') 
		{
				for(a=1; argv[index][a]!='\0'; a++)
				{
					if(argv[index][a]!='a' && argv[index][a]!='l'&&argv[index][a]!='s' && argv[index][a]!='S'&& argv[index][a]!='h')//if -alsSh option X 
					{
						printf("error command\n");//print error message
						return 0;
					}
				}
		
		}
		
	 }

	
	
	
	

	
	
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
	
	
	total=0;
	i=0;
/////////////////////////////////////////////////////////////////////////////////////////////////////
	original_ab_count=ab_count;

		for(total_loop=0; total_loop<ab_count+r_count || (ab_count+r_count)==0 ; total_loop++)// loop for ls option 
		{
			match_count=1;// set 1
			

			// when absolute path and relative path does not exist at argv or when relatvie path exist at argv & first loop 
			if(lflag==1&&(ab_count+r_count==0 || (total_loop==0)&&(r_flag==1))) 
			{
				getcwd(path,10000);//get curent working directory at path
				/*
				printf("Directory path:%s\n",path); //print "Directory path:"
				memset(path,0,10000);//reset path*/
			}
			
			if((d_count==1) &&total_loop>=original_ab_count+r_count)// when directory that was found for wildcard is only one 
			{
				
				if(lflag==0 )
				{
					printf("%s \n",ab_path[total_loop-r_count]);
					printf("==================================================================================================\n");
					break;
				}
				else
				{i=0;c=0;
				
					
				stat(ab_path[total_loop-r_count],&fstat);//store st_info in info
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
						total+=fstat.st_blocks;	//total store
						strcpy(arr[i].fname,ab_path[total_loop-r_count]);

						
							if(sflag==1)// if -s option
							{
								if(hflag==1)//if -h option
								hoption(1024*arr[i].total/2);
								else//not -hoption
								printf("%d ",arr[i].total/2);

							}
	
							if(lflag==1 )//if -l option
							{
								
								printf("%s ",arr[i].permission);
								printf("%d ",arr[i].linkcounter);
								printf("%s ",arr[i].u_ID);
								printf("%s ",arr[i].g_ID);
								if(hflag==1)
								hoption(arr[i].CAPACITY);
								else
								printf("%d ",arr[i].CAPACITY);
								printf("%d %d %d:%d ",arr[i].mon,arr[i].day,arr[i].hour,arr[i].min);
							}//lflag if end

							printf("%s \n",arr[i].fname);
						

						
						break;
				}
				
				//break;
			}
			else if((ab_count+r_count)==0 && total_loop!=0)//if absolute path and relative path does not exist at argv => if second loop ,break
				break;//break
			else if(total_loop<r_count || (ab_count+r_count)==0) //relative function
			{	
				dirp=opendir(".");//open current directory
			}
			
			else //absolute function
			{
				//d_count=0;
				i=0;
				dirp=opendir(ab_path[total_loop-r_count]);//open absoulte path
				printf("Directory path:%s\n",ab_path[total_loop-r_count]);//print "Directory path"
			}

						err_woption=1; match=1; w_open=0; w_close=0;temp=-1;//variable setting
						memset(temp_str,0,256);//reset
						memset(path,0,10000);//reset

					if(total_loop<r_count)// relative loop
					{
					for(b=0; r_path[total_loop][b]!='\0'; b++)// r_path read
					{

					if(r_path[total_loop][b]=='*' || r_path[total_loop][b]=='?')// if wild option
						err_woption=0;//not err_woption setting
					else if(r_path[total_loop][b]=='[')// if wild option [
						{
						w_open=b; err_woption=0;//store '[' location and not err_woption setting
						}
					else if(r_path[total_loop][b]==']')//if wild option]
						{
						w_close=b; err_woption=0;//store ']' location and not err_woption setting
						}
					else if(r_path[total_loop][b]=='/' && err_woption==1)// before wild card, last '/' location store at temp
						temp=b;
					else //extra cases
						continue;// continue
			
					}


					if(w_open+1==w_close || w_open>w_close)// '[]' option error
						err_woption=1;//err_wopiton setting
					
					

					}//if total_loop<r_count-1 end 
					
					if(temp!=-1 && err_woption==0)// before wild card, when '/' is located
					{
						strcpy(path,r_path[total_loop]);//r_path[total_loop] store at path
						path[temp++]='\0';// NULL
						closedir(dirp);//close dirp
						
						if(opendir(path)==NULL)//if path error
						{
							printf("%s does not exist file or directory\n",path);//print error message
							continue;//continue
						}
						
						for(k=temp; k<b; k++)// path not err
							temp_str[k-temp]=r_path[total_loop][k];//store path at temp_str
						temp_str[k-temp]='\0';//NULL
						
					}

						

					else if(total_loop<r_count || (r_count+ab_count)==0)//relative path loop
						getcwd(path,10000);//get current working directory
					else// extra cases
						strcpy(path,ab_path[total_loop-r_count]);//store ab_path[total_loop-r_count] at path
					if(err_woption==0)// when not err_woption
						match_count=0;//set match_count=0

					if(lflag==1 && err_woption!=0 && (ab_count+r_count==0 || (total_loop<r_count && total_loop==0)))
					{
					printf("Directory path:%s\n",path); //print "Directory path:"
					//memset(path,0,10000);//reset path*
	
					}

						
					while((dir=readdir(dirp))!=NULL)//whie start
					{memset(info,0,10000);//reset
					


						strcpy(info,path);//store path at info
						strcat(info,"/"); //info+/
	
						strcat(info,dir->d_name);// info+file name
						c=0;
						

						if(err_woption==0)//when wild option
						{	

							
								if(dir->d_name[0]=='.')//when hidden file/directory 
									continue; //continue
							
							
							if(temp==-1)// before wild card, absolute path does not exist
							{
							match=fnmatch(r_path[total_loop],dir->d_name,0);//match function by using r_path[total_loop]
							
							}
							else// before wild card, absoulte path exist
							match=fnmatch(temp_str,dir->d_name,0);//match function by using temp_str
							
							if(match!=0)//not match
								continue;
							if(opendir(info)!=NULL) // dir is directory
							{
								d_count++;//d_count +1
								
								strcpy(ab_path[ab_count++],info);//info store at ab_path[ab_count];
								match_count++;//match_count+1
								continue;
							}
							match_count++;//match_count+1









					
						}
						else if(r_flag==1 && total_loop<r_count)// r_path
						{

							if(strcmp(dir->d_name,r_path[total_loop])!=0 && strcmp(info,r_path[total_loop])!=0) // not find file
								continue;		//continue				
							
								
		
						}


						else//if total_loop=0 || ab_path
						{

							
							if(aflag==0)//when aflag is zero
							{
								if(dir->d_name[0]=='.')//if hidden file or directory
									continue;//continue
							}
							
						}
						

						
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
						total+=fstat.st_blocks;	//total store
						if((ab_count+r_count)!=0 && match!=0 && total_loop<r_count)//if r_path loop
						strcpy(arr[i].fname,r_path[total_loop]);//strcpy file name at arr[i].fname
						else if(temp!=-1 &&match==0)//if wild loop &'/' exist before wild
						strcpy(arr[i].fname,info);//strcpy info at arr[i].fname
						else//extra cases
						strcpy(arr[i].fname,dir->d_name);//strcpy dir->d_name at arr[i].fname

						
						
						i++;//i+1
					}
					if(match_count==0 && total_loop<r_count )// if wild option & can't find match file or directory
						printf("%s does not exist file or directory\n",r_path[total_loop]);//print error message
					if(total_loop<r_count-1)// if relative path loop
						continue;
					else// swap & print & arr reset
					{
			
					
					//////////////////////////
					a=0;b=0;
					if(((r_count+ab_count)==0||total_loop>=r_count-1)&&aflag==1)//if current directory ls & -a option
					{
						


							for( j=1; aflag!=0 && j<i; j++)				// use for check where '.' is.
							{						// if '.' is not array first
								if(arr[j].fname[0]=='.'&& arr[j].fname[1]=='\0')
								{
									b=1;					//flag up
									break;
								}
							}


							if(b==1)				//if flag up=> '.' is not first of the array
							{
								swap(&arr[0],&arr[j]);//swap
							}
	

							a=0,b=0;								//reset
			
							for(j=2; aflag!=0 && j<i; j++)							// use for check where '..' is
							{
								if(arr[j].fname[0]=='.' && arr[j].fname[1]=='.' && arr[j].fname[2]=='\0')	// if '..' is not array second
								{
									b=1;								//flag up
									break;
								}
							}

	
							if(aflag!=0 && b==1)				//if flag up=> '..' is not second of the array
							{
								swap(&arr[1],&arr[j]);//swap
							}


						}//if current directory ls & -a option


						if(((ab_count+r_count)==0 || total_loop>=r_count) &&aflag==1 )// for sorting function flag
						{
							a=1; b=3;
						}
						else // for sorting function flag
						{
							a=-1; b=1;
						}

						for(j=b; j<i && Sflag==0; j++)//sorting
							sorting(arr,j,a);
						for(j=0; j<i && Sflag!=0; j++)// -S option sotrting
							Sorting(arr,j);

						for(a=0; total_loop==r_count-1 &&a<r_count && r_flag==1 ; a++)// wild card check
						{

							err_woption=0;//set err_woption=0



							for(b=0; r_path[a][b]!='\0'; b++)//read r_path[a]
							{

								if(r_path[a][b]=='*' || r_path[a][b]=='?')//if wild card
									err_woption++;//err_woption +1
								else if(r_path[a][b]=='[')//if '[' eixst
									{err_woption++;//err_woption+1
									w_open=b;//store '[' location
									}
								else if(r_path[a][b]==']')//if ']' exist
									{err_woption++;//err_woption+1
									w_close=b;//store ']' location
									}
								else//extra case
									continue; //continue
			
							}


							if(w_open+1==w_close || w_open>w_close)// if '[]'wild card error
								err_woption=0;//err_woption set

							if(err_woption!=0) //wild option => not file
								continue;//continue
							



			
							for(b=0; b<i && r_count!=0; b++)//checking file
							{
								if(strcmp(r_path[a],arr[b].fname)==0)//file exist
								 	break;
							}

							if(b==i) // r_path[a] does not exist file
								printf("%s does not exist directory or file\n",r_path[a]);//error message print
		
						}

						if(((ab_count+r_count)==0 || total_loop>=r_count) && lflag==1)//print total
						{
							if(hflag==1)//if -h option
							{
							printf("total:");
							hoption(1024*total/2);//hoption print function
							printf("\n");
							}
						
							else// not -h option
							 printf("total:%d \n",total/2);
						}


						for(  a=0; a<i ; a++)	//print file name
						{
							if(sflag==1)// if -s option
							{
								if(hflag==1)//if -h option
								hoption(1024*arr[a].total/2);
								else//not -hoption
								printf("%d ",arr[a].total/2);

							}
	
							if(lflag==1 )//if -l option
							{
								
								printf("%s ",arr[a].permission);
								printf("%d ",arr[a].linkcounter);
								printf("%s ",arr[a].u_ID);
								printf("%s ",arr[a].g_ID);
								if(hflag==1)
								hoption(arr[a].CAPACITY);
								else
								printf("%d ",arr[a].CAPACITY);
								printf("%d %d %d:%d ",arr[a].mon,arr[a].day,arr[a].hour,arr[a].min);
							}//lflag if end

							printf("%s \n",arr[a].fname);
						}
				
					printf("==================================================================================================\n");
	
					closedir(dirp);//close directory

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
	
	
					total=0;//reset total




					 
					}
					}


}
	


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	/////////////////////change dirlist [idx] and dirlist[idx-1]////////////////(sorting)
	
	swap(&arr[idx],&arr[i]);

	idx--;				//idx=idx-1
  	i--;				//i=i-1
	}
	
	else				// not sorting
	 break;//break
	}		
}

void Sorting ( struct mynode arr[1000], int idx )//sorting function
{
	int i=0;				//int variable
	int j=0;				//int variable
	int k=0;				//int variable
	
	
	for(i=idx-1; i>-1 ; )			// when -a option :arr[0] & arr[1] is alread set, so compare when i>1, when not -a option:compare i>-1
	{
	 k=0,j=0;
	 
	
	if(arr[i].CAPACITY<arr[idx].CAPACITY)		// arr[i].CAPACITY> arr[idx].CAPACITY => sorting 
	{
	/////////////////////change dirlist [idx] and dirlist[idx-1]////////////////(sorting)
	
	swap(&arr[idx],&arr[i]);

	idx--;				//idx=idx-1
  	i--;				//i=i-1
	}
	else if(arr[i].CAPACITY==arr[idx].CAPACITY)
	{
	sorting(arr,idx,idx-1);
	break;
	}
	
	else				// not sorting
	 break;//break
	}		
}



void hoption( int num)//hoption
{
if(num>1024*1024*1024)//if num is bigger than 1024x1024x1024
	printf("%.1fG ",(double)num/(1024*1024*1024));//print G unit
else if(num>1024*1024)//if num is bigger than 1024x1024
	printf("%.1fM ",(double)num/(1024*1024));//print M uint
else if(num>1024)//if numb is bigger then 1024
	printf("%.1fK ",(double)num/1024);//print K unit
else//if extra case
	printf("%d ",num);//print


}

