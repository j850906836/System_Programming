#include <stdio.h>
#include <unistd.h>
int main () {
  pid_t pid;
  pid = fork ();
  pid = fork ();
  pid = fork ();
  pid = fork ();
  printf("pid = %d\n",getpid() );
  
  return 0;
}
