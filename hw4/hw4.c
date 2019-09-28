#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#define BUF_SIZE 4096

int main() {
	int fd,i,ret;
	ssize_t numIn;
	char buffer[BUF_SIZE];

	while(1) {
		fd = open("grade.txt", O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
		numIn = read(STDIN_FILENO,buffer,BUF_SIZE);
		ret = flock(fd,LOCK_EX|LOCK_NB);
		if( !ret ) {
			write(fd,buffer,(ssize_t)numIn);
			for(i=0;i<5;i++){
				fprintf(stderr,"-");
				sleep(1);
			}
			printf("\n");
			ret = flock(fd,LOCK_UN);
		}
		else
			printf("Please insert 5 second\n");
	}
	close(fd);
	return(EXIT_SUCCESS);
}
