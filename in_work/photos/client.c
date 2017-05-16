#include<stdio.h>
#include<stdlib.h>


int main(int argc, char *argv[]){
  //clrscr();
  FILE *fptr;
  FILE *txt;
  int c;

  txt=fopen("test1.txt","w");
  fptr=fopen("acespng.png","rb");

  if(fptr==NULL){
    printf("NOTHING In FILE\n");
    fclose(fptr);
  }else{
    printf("success\n");

    while((c=fgetc(fptr)) !=EOF){
      for(int i=0;i<=7;i++){
        if(c&(1<<(7-i))){
          fputc('1',txt);
        }else{
          fputc('0',txt);
        }
      }
    }
  }

  fclose(fptr);
  fclose(txt);
  printf("writing over\n");
}
