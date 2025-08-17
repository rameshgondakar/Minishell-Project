#include"header.h"

extern char *external_commands[];
extern char *builtins[];
extern char input_string[100];
char buff[100];
int flag = 0;
List *list=NULL;
int sta,status,signal_flag=0,signal_handler_flag=0;
pid_t pid_p;

void insert_at_first(List **list,pid_t pid,char *s)
{
	List *new = (List*) malloc(sizeof(List));
	new->pid = pid;
	new->signal = malloc(sizeof(50));
	strcpy(new->signal,input_string);
	new->next = NULL;
	if(*list==NULL)
	{
		*list = new;
	}
	else
	{
		new->next = *list;
		*list = new;
	}
}

void delete_first(List **list)
{
	if(*list!=NULL)
	{
	List *temp = *list;
	*list = (*list)->next;
	free(temp);
	}
	else
		printf("Empty\n");
}

void print(List *list)
{
	List *temp = list;
	while(temp!=NULL)
	{
		printf("Stopped               %s\n",temp->signal);
		temp = temp->next;
	}
}

void signal_handler(int sig_num)
{
	if(sig_num==SIGINT)
	{
		if(pid_p==0)
	    {
		printf("\nMinishell$");
		fflush(stdout);
		}
	}
	else if(sig_num==SIGTSTP)
	{
		if(pid_p==0 && WIFSTOPPED(status))
		{
			pid_p = list->pid;
		}
		if(pid_p == 0)
	    {
		printf("\nMinishell$");
		fflush(stdout);
	    }
		else
		{
			insert_at_first(&list,pid_p,input_string);
		}
    }
	else if(pid_p==0 && sig_num == SIGCHLD)
	{
		// printf("\n %d %d outside\n",signal_handler_flag,signal_flag);
		if(signal_handler_flag>=signal_flag && signal_handler_flag!=0 && signal_flag!=0)
		{
		waitpid(-1,&sta,WNOHANG);
		signal_handler_flag--;
		signal_flag--;
		// printf("\n %d %d inside\n",signal_handler_flag,signal_flag);
		if(signal_handler_flag==0 && signal_flag==0)
		{
			signal(SIGCHLD,SIG_DFL);
		}
		}
		else{
			signal_handler_flag++;
		}
	}
}

int check_command_type(char *command)
{
	for(int i=0;external_commands[i]!=NULL;i++)
	{
		if(strcmp(command,external_commands[i])==0)
		{
			return EXTERNAL;
		}
	}
	for(int i=0;builtins[i]!=NULL;i++)
	{
		if(strcmp(command,builtins[i])==0)
		{
			return BUILTIN;
		}
	}
	return NO_COMMAND;
}

void extract_external_commands(char *external_commands[200])
{
	int fd = open("external_commands.txt",O_RDONLY);
	char ch;
	int i=0,j=0;

	if(fd==-1)
	{
		perror("open");
		return;
	}

	while (read(fd,&ch,sizeof(char))!=0)
	{
		if(ch=='\n')
		{
			buff[i] = '\0';
			external_commands[j] = (char*) malloc(sizeof(char)*strlen(buff));
			strcpy(external_commands[j],buff);
			i=0;
			j++;
		}
		else
		{
		    buff[i++] = ch;
		}
	}
	external_commands[j] = NULL;
}

char *get_command(char *input_string)
{
	strcpy(buff,input_string);
	char *res = strtok(buff," ");
	return res;
}

void scan_input(char *prompt, char *input_string)
{
	signal(SIGINT,signal_handler);
	signal(SIGTSTP,signal_handler);
	extract_external_commands(external_commands);
	// int status;
	while(1)
	{
		memset(input_string,'\0',100);
		flag = 0;
		printf("%s",prompt);
		scanf(" %[^\n]",input_string);
		char buff[100];
		strcpy(buff,input_string);
		char *user =  strtok(buff,"=");
		if(strcmp("PS1",user)==0)
		{
			user =  strtok(NULL,"=");
			if(!((user[0]>='a' && user[0]<='z')||(user[0]>='A' && user[0]<='Z')))
			{
				printf("\nerror\n");
				return;
			}
			strcpy(prompt,user);
		} 
		else if(strcmp("PS1 ",user)==0)
		{
			printf("\nerror\n");
		}
		else if(strcmp("fg",user)==0)
		{
			if(list!=NULL)
			{
			List *temp = list;
			kill(temp->pid,SIGCONT);
			waitpid(temp->pid,&status,WUNTRACED);
			delete_first(&list);
			}
		}
		else if(strcmp("bg",user)==0)
		{
			// printf("pid %d\n",list->pid);
			if(list!=NULL)
			{
			signal_flag++;
			signal(SIGCHLD,signal_handler);
			kill(list->pid,SIGCONT);
			delete_first(&list);
			}
		}
		else if(strcmp("jobs",user)==0)
		{
			print(list);
		}
		else{
			//get command
			replace_blank(input_string);
			char *cmd = get_command(input_string);
			//check command type
			int type = check_command_type(cmd);
			// printf("%d",type);
			
			if(type==EXTERNAL)
			{
				execute_external_commands(input_string,prompt);
			}
			else if(type==BUILTIN)
			{
			    execute_internal_commands(input_string,prompt);
			}
		}
	}
}

void replace_blank(char *s)
{
    int count = 0;
    while(*s)
    {
        if(*s==' ')
        {
            count++;
        }
        if(*(s-1)==' ' && *s!=' ')
        {
            char *temp=s;
            while(*temp!='\0')
            {
                *(temp-count+1) = *temp;
                temp++;
            }
            *(temp-count+1)='\0';
            s = s-count+1;
            count = 0;
        }
        s++;
    }
}

void strrev(char *str)
{
	int len = strlen(str);
	int i =0;
	while(i<len/2)
	{
		char temp = str[i];
		str[i] = str[len-i-1];
		str[len-i-1] = temp;
	}
}

void execute_internal_commands(char *input_string,char *prompt)
{

	if(strncmp("exit",input_string,4)==0)
	{
		//completed
		exit(0);
	}
	else if(strncmp("pwd",input_string,3)==0)
	{
		//completed
		char buf[50];
		getcwd(buf,sizeof(buf));
		printf("%s\n",buf);
	}
	else if(strncmp("cd",input_string,2)==0)
	{
		
		if(strchr(input_string,'.')!=NULL)
		{
			int count = 0;
			strcpy(buff,input_string);
			strrev(buff);
			strtok(buff," /");
		}
		else
		{
		if(chdir(input_string+3)==0){
		prompt[strlen(prompt)-1] = '/';
		strcat(prompt,input_string+3);
		strcat(prompt,"$");
		flag++;
		}
		}
	}
	else if(strncmp(input_string,"echo",4)==0)
	{
		if(strstr(input_string,"$$")!=NULL)
		{
			printf("%d\n",getpid());
		}
		else if(strstr(input_string,"$SHELL")!=NULL)
		{
			getenv("SHELL");
		}
		else if(strstr(input_string,"$?")!=NULL)
		{
			//no idea
			printf("%d\n",WEXITSTATUS(sta));
		}
	}
}

void execute_external_commands(char *input_string,char *prompt)
{ 
	flag = 1;
	pid_p = fork();
	int status;
	if(pid_p==0)
	{
	signal(SIGINT,SIG_DFL);
	signal(SIGTSTP,SIG_DFL);
	char *command[20];
	int count = 0;
	char buff[100];
	int i=0,j=0,backup = dup(STDIN_FILENO);
    
	for(int i=0;input_string[i]!='\0';i++)
	{
		if(input_string[i]!=' ')
		{
			buff[j++] = input_string[i];
		}
		else
		{
			buff[j] = '\0';
			command[count] = malloc(strlen(buff)+1);
			strcpy(command[count++],buff);
			j = 0;
		}
	}
    
	buff[j] = '\0';
	command[count] = malloc(strlen(buff)+1);
	strcpy(command[count++],buff);
	command[count] = NULL;
    
	int pipe_ind[count];
    int ind = 0;
    int fd[2];
    
    pipe_ind[0] = 0;
    
    for(int i=0;i<count;i++)
    {
        if(strcmp(command[i],"|")==0)
        {
            pipe_ind[++ind] = i+1;
            command[i] = NULL;
        }
    }
	if(ind == 0)
	{
		//no pipe
		execvp(command[0],(char **)command);
	}
	else
	{
	//n pipe
    for(int i=0;i<=ind;i++)
    {
        if(i!=ind)
        {
        if(pipe(fd)==-1)
        {
            perror("pipe");
        }
        }
        
        pid_t pid = fork();
        
        if(pid == 0)
        {
            if(i!=ind)
            {
                close(fd[0]);
                dup2(fd[1],STDOUT_FILENO);
                close(fd[1]);
            }
            if(execvp(command[pipe_ind[i]],&command[pipe_ind[i]])==-1)
            {
                perror("execvp");
            }
        }
        else if(pid > 0)
        {
            wait(NULL);
            if(i!=ind)
            {
                close(fd[1]);
                dup2(fd[0],STDIN_FILENO);
                close(fd[0]);
            }
        }
    }	    
	dup2(backup,STDIN_FILENO);
	}
    }
	else if(pid_p>0)
	{
		waitpid(pid_p,&status,WUNTRACED);
		pid_p = 0;
	}   
}
