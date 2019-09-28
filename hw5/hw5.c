#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 4096

int main(int argc, char* argv[]) {

	int inputFd, outputFd, tmpfile , change=1 , i=0;
	ssize_t numIn, numOut;
	char buffer[BUF_SIZE], file2add[100] = {0} , file2name[100] = {0},template[] = "tmp_XXXXXX";
	char stop,*ptr,*ptr1,*ptr2;		
	inputFd = open (argv [1], O_RDONLY);
	ptr=ptr1=ptr2=argv[2];
	while(*ptr2!='\0') {
		if(*ptr2=='/')	ptr1 = ptr2;
		ptr2++;
	}
	while(ptr!=ptr1){
		file2add[i++] = *ptr;
		ptr++;
	}
	i=0;
	ptr1++;
	while(ptr1!=ptr2){
		file2name[i++] = *ptr1;
		ptr1++;
	}
	printf("%s\n",file2name);
	change = chdir(file2add);

	tmpfile = mkstemp(template);
	getcwd(buffer,4096);
	printf("%s\n",buffer);
	if (inputFd == -1) {
		perror ("cannot open the file for read"); exit(1); }
	if(tmpfile == -1){
		perror("canot open the file for write"); exit(1); }

	while((numIn = read (inputFd, buffer, BUF_SIZE)) > 0){
		numOut = write (tmpfile, buffer, (ssize_t) numIn);
		if(numIn != numOut){ perror("numIn != numOut"); exit(1); }
	}
	while(1){
		if(stop=='\n') break;
		printf("program stops now, press Enter to continue...");
		scanf("%c",&stop);
	}
	rename(template,file2name);		
	close (inputFd); 
	return (EXIT_SUCCESS);
}


