/* 
 * File:   receiver.c
 * Author: daniel
 *
 * Created on September 20, 2011, 2:47 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <math.h>


int Convert2ASCII( char[]);
/*
 * 
 */
int main(int argc, char** argv) {
    char fileName[256], fileNameout[256]; /**< name of file to be open */
     char userFile[256], dir[256];
     FILE* fins;
     FILE* fos;
     long fSize;
     char* buff;
     int inputChar, frameCount, numberChar, syncCount, charCount, totChar;
     char* charBuffer;
     char* storage;
     char binChar[8];
     size_t result;

     //while(1){
    /** Get the file location, file name and character to replace space with */
/*
        printf("Enter 'exit' for dir to quit. \n");
        printf("Enter directory of the file: \n Ex: /home/user/\n");
        fgets(dir,sizeof(dir), stdin);
        if(strncmp(dir,"exit",4)==0){
            break;
        }
        printf("Enter filename: \n Ex: myfile.txt\n");
        fgets(userFile, sizeof(userFile),stdin);
        if(strncmp(dir,"exit",4)==0){
            break;
        }
        if(dir[strlen(dir)-1]== '\n'){
            dir[strlen(dir)-1]= '\0';
        }
        if(userFile[strlen(userFile)-1]== '\n'){
            userFile[strlen(userFile)-1]= '\0';
        }
*/

        /** Prep the strings for the server thread */
/*
        strcpy(fileName,dir);
        strcat(fileName,userFile);
        strcpy(fileNameout,dir);
        strncat(fileNameout,userFile,strlen(userFile)-4);
        strcat(fileNameout,".pdf");
*/
        
        fins = fopen("/home/daniel/NetBeansProjects/Transmitter/output.bin", "rb");
        if (fins==NULL) {fputs ("File error",stderr); exit (1);}
        
        fseek (fins , 0 , SEEK_END);
        fSize = ftell (fins);
        rewind (fins);      
        
        buff = (char*) malloc (sizeof(char)*fSize/8-4);
        if (buff == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        frameCount=0;
        syncCount=0;
        numberChar=0;
        charCount=0;
        totChar=0;
        strncpy(fileNameout,"output",6);
        
        while(fgets(binChar,9,fins)!=NULL){
            charBuffer = (char*) malloc (sizeof(char));
            if (charBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}
            inputChar=Convert2ASCII(binChar);
            if(inputChar!=22&&numberChar!=0&&frameCount>0){
                sprintf(charBuffer,"%c",inputChar);
                strncat(buff,charBuffer, 1);
                charCount++;
            }
            if(inputChar!=22&&numberChar!=0&&frameCount==0){
                sprintf(charBuffer,"%c",inputChar);
                strncat(fileNameout,charBuffer, 1);
                charCount++;
            }
            if(syncCount==1&&numberChar==0){
                numberChar=inputChar/8;
            }
            if(inputChar==22){
                syncCount++;
            }
            if(syncCount==2){
                if(charCount!=numberChar){
                    fputs ("Number of bits does not match",stderr); exit (3);
                }
                totChar+=numberChar;
                numberChar=0;
                syncCount=0;
                frameCount=0;
                charCount=0;
                frameCount++;
            }
        }

        

/*
        storage = strtok(buff,22);
        while(storage!=NULL){
            storage=strtok(NULL,22);
        }
*/

        
        //printf(buff);
        fos = fopen(fileNameout, "wb");
        if (fos==NULL) {fputs ("File error",stderr); exit (1);}
        fwrite(buff,1,totChar-4,fos);
        fclose(fins);
        fclose(fos);
        free(buff);
    // }



    return (EXIT_SUCCESS);
}

int Convert2ASCII( char str[8]){
    int parity, i, pcount;
    double exponent;
    parity = 0;
    pcount = 0;
    //ascii = 0;
    
    for(i=0; i<8; i++){
        if(i==0){
            if(str[i] == '1'){
                parity = 1;
                str[i]='0';
            }
            
        }
        else{
            if(str[i] == '1'){
                pcount++;
            }
        }
    }
    if(pcount%2 != parity)  
        {fputs ("Parity Error",stderr); exit (4);}
    return((int) strtol(str, NULL,2));
    
}
 