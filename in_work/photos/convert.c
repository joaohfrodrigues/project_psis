#include<stdio.h>
#include<stdlib.h>


//The following function converts the ones and zeroes in the text file into a character.
//For example the text file may have the 8 consecutive characters '1','0','0','0','1','0','0','0'.
//This converts it into the character equivalent of the binary \\value 10001000

char bytefromtext(char* text){
  char result=0;
  for(int i=0;i<8;i++){
    if(text[i]=='1'){
      result |= (1<< (7-i) );
    }
  }
  return result;
}

void main(){
  //clrscr();
  FILE *pfile;
  FILE *image;
  unsigned char buf[8];
  unsigned char c;
  int j=0;

  image=fopen("aces2.png","wb"); //open an empty .bmp file to
  pfile=fopen("test1.txt","r");

  if(pfile==NULL)
    printf("error");
  else{
    int ch=fgetc(pfile);
    c=(unsigned char) ch;
    while(ch!=EOF){
      buf[j++]=c;
      if(j==8){
        fputc(bytefromtext(buf),image);
        j=0;
      }
      ch=fgetc(pfile);
      c= (unsigned char) ch;
    }
    fclose(pfile);
    fclose(image);
  }
}
