#include<stdio.h>
#include<stdlib.h>

void copyfile(){
  FILE *source, *destination;
  char ch;
  
  source = fopen("/etc/passwd", "r");
  destination = fopen("/tmp/passwd", "w");

  if(source == NULL || destination == NULL){
    fclose(source);
    fclose(destination);
    printf("Error while opening files\n");
    exit(EXIT_FAILURE);
  }
  
  while((ch = fgetc(source)) != EOF){
    fputc(ch, destination);
  }
  
  fclose(source);
  fclose(destination);
}


int main(int argc, char* argv[]){
  int current_pid = getpid();
  printf("sneaky_process pid = %d\n", current_pid);
  copyfile();
  return 0;
}
