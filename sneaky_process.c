#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[]){
  int current_pid = getpid();
  printf("sneaky_process pid = %d\n", current_pid);
  return 0;
}
