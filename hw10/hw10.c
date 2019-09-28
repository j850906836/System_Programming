/*執行方法：
  ./myShell
  >> ls -R /
  ctr-c
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
#include <sys/resource.h> 
///color///
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

/*
   全域變數，放解析過後的使用者指令（字串陣列）
 */
char* argVect[256];

//下列三個變數作為main和signal handler溝通用
sigjmp_buf jumpBuf;
volatile sig_atomic_t hasChild = 0;
pid_t childPid;
int pipenum;
int pipe_index[30];

/*signal handler專門用來處理ctr-c*/
void handler(int sigNumber) {
	if (hasChild) {
		kill(childPid, sigNumber);
		hasChild = 0;
	} 
	else {
		/*確認main function並不是剛好在處理字串，這裡使用一個隱含的同步方法*/
		/*藉由確認是否argVect[0]（即執行檔）是否為NULL保證main function不是在處理字串*/
		/*主程式的控制迴圈必須在一開始的地方將argVect[0]設為NULL*/
		if (argVect[0] == NULL) {
			ungetc('\n', stdin);ungetc('c', stdin);ungetc('^', stdin);
			siglongjmp(jumpBuf, 1);
		} 
	else {
			/*極少發生，剛好在處理字串，忽略這個signal，印出訊息提示一下*/
			fprintf(stderr, "info, 處理字串時使用者按下ctr-c\n");
	}
	}
}

/*
   parseString：將使用者傳進的命令轉換成字串陣列
   str：使用者傳進的命令
   cmd：回傳執行檔
 */
void parseString(char* str, char** cmd) {
	int idx=0;
	char* retPtr;
	//printf("%s\n", str);
	retPtr=strtok(str, " \n");
	while(retPtr != NULL) {
		//printf("token =%s\n", retPtr);
		//if(strlen(retPtr)==0) continue;
		argVect[idx++] = retPtr;
		if (idx==1)
			*cmd = retPtr;
		retPtr=strtok(NULL, " \n");
	}
	argVect[idx]=NULL;
}


void count_pipeline( char* str )
{    

	pipenum = 0;
	char *ptr = str;
	int index = 0;
	int a = 0;

	for( int i = 0;i<30;++i )
	{
		pipe_index[i] = 0;
	}

	//count how many pipe line
	while(*ptr!='\n')
	{
		if( *ptr == '|' )
		{
			*ptr = '\0';
			pipe_index[a] = index+1-pipe_index[a-1];
			++pipenum;
			++a;
		}
		ptr++;
		index++;
	}


}

int main (int argc, char** argv) {
	char cmdLine[4096];
	char hostName[256];
	char cwd[256];
	char* exeName = "a";
	int pid, pos, wstatus,pid2,pid3;
	int pipefd[2];
	struct rusage resUsage;     //資源使用率
	struct timespec statTime, endTime;
	struct sigaction act,act1,act2;
	act.sa_handler= handler;
	act1.sa_handler= SIG_IGN;
	/*底下程式碼註冊signal的處理方式*/
	sigaction(SIGINT,&act,NULL);
	sigaction(SIGQUIT,&act1,NULL);
	sigaction(SIGTSTP,&act1,NULL);

	/*無窮迴圈直到使用者輸入exit*/
	while(1) {
		char* showPath;
		char* loginName;
		int homeLen = 0;
		//設定化hasChild, argVect[0]，避免發生race condtion
		hasChild = 0;
		argVect[0]=NULL;
		//抓取主機名稱、用戶名稱
		loginName = getlogin();
		gethostname(hostName, 256);
		/*
		   底下程式碼製造要顯示的路徑字串，
		   會參考"home"將"home"路徑取代為~
		 */
		getcwd(cwd, 256);
		pos=strspn(getenv("HOME"), cwd);
		homeLen = strlen(getenv("HOME"));
		//printf("cwd=%s, home=%s, pos=%d, prompt=%s", cwd, getenv("HOME"), pos, &cwd[pos]);
		if(pos>=homeLen) {
			cwd[pos-1]='~';
			showPath=&cwd[pos-1];
		}
		else
			showPath=cwd;
		/*
		   底下程式碼負責印出提示符號
		 */
		printf(LIGHT_GREEN"%s@%s:", loginName, hostName);
		printf(BLU_BOLD"%s>> " NONE, showPath);
		sigsetjmp(jumpBuf, 1);
		
		//接收使用者命令，除了cd, exit以外，其他指令呼叫對應的執行檔
		fgets(cmdLine, 4096, stdin);

		if (strlen(cmdLine)>1)  //判斷長度是否大於1，判斷「使用者無聊按下enter鍵」
		{   
			count_pipeline(cmdLine);
			if(pipenum==0)
			parseString(cmdLine, &exeName);

		}

		else
			continue;
		if (strcmp(exeName, "^c") == 0) {   //使用者按下control-c，^c是由signal handler放入
			//printf("ctr-c \n");
			printf("\n");
			continue;
		}
		if (strcmp(exeName, "exit") == 0)   //內建指令exit
			break;
		if (strcmp(exeName, "cd") == 0) {   //內建指令cd
			if (strcmp(argVect[1], "~")==0)
				chdir(getenv("HOME"));
			else
				chdir(argVect[1]);
			continue;
		}


		if( pipenum == 0)
		{
			pid = fork();   //除了exit, cd，其餘為外部指令
			if (pid == 0) {
				if (execvp(exeName, argVect)==-1) {
					perror("myShell");
					exit(errno*-1);
				}
			}	 

			else {
				childPid = pid;/*通知singal handler，如果使用者按下ctr-c時，要處理這個child*/
				hasChild = 1;/*通知singal handler，正在處理child*/
				wait3(&wstatus, 0, &resUsage);
			}
		}

		else
		{
			
			int pipe_indexx = 0;
			char *ptr = cmdLine;
			int pipeline[100][2];
			int i = 0;
			pipe(pipeline[pipe_indexx]);

			parseString(ptr, &exeName);
			pid = fork();
			if(pid==0)
			{
				close(1);
				dup(pipeline[pipe_indexx][1]);
				close(pipeline[pipe_indexx][0]);
				close(pipeline[pipe_indexx][1]);
				execvp(exeName, argVect);
			}

			else
			{
				pipenum--;
				ptr+=pipe_index[i];
				i++;
				parseString(ptr, &exeName);

				while(pipenum>=1)
				{
					pipe_indexx++;
					pipe(pipeline[pipe_indexx]);

					pid2 = fork();
					if(pid2 == 0)
					{
						close(0);
						dup(pipeline[pipe_indexx-1][0]);
						close(1);
						dup(pipeline[pipe_indexx][1]);
						for(int j = 0;j<=pipe_indexx;++j)
						{
							close(pipeline[j][0]);
							close(pipeline[j][1]);
						}
						execvp(exeName, argVect);
					}
					else
					{
						pipenum--;
						ptr+=pipe_index[i];
						i++;
						parseString(ptr, &exeName);
						continue;
					}
				}

				pid3 = fork();
				if(pid3==0)
				{
					close(0);
					dup(pipeline[pipe_indexx][0]);
					if(pipe_indexx==0)
					{
							close(pipeline[0][0]);
							close(pipeline[0][1]);
					
					}
					
					for(int j = 0;j<=pipe_indexx;++j)
					{
							close(pipeline[j][0]);
							close(pipeline[j][1]);
					}
					execvp(exeName, argVect);
				}

			}
			
			for( int j = 0;j<pipe_indexx+1;++j)
			{
					close(pipeline[j][0]);
					close(pipeline[j][1]);
			}
			
				wait(&wstatus);
				wait(&wstatus);
				wait(&wstatus);
			
			if(pipe_indexx == 0)
			{
				wait(&wstatus);
			}
				
			for( int j = 0;j<pipe_indexx+1;++j)
			{
					wait(&wstatus);
			}
		}
	}
}

