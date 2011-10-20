/* 
 * File:   receiver.c
 * Author: Daniel Huelsman 
 * Class: Data Communications 
 * Project 1
 *
 * Created on September 20, 2011, 2:47 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>



int Convert2ASCII( char[]);
void HammingDecode(char[],char[]);
void CRCDecode(char[],char[]);
/*
 * 
 */
int main(int argc, char** argv) {
     char fileNameout[256]; // Name of the output file
     FILE* fins;            // Input file Stream
     FILE* fos;             // Output file Stream
     long fSize;            // File Size in "bits"
     char* buff;            // Buffer for the converted file
     
     /* The input character as an int, frame counter, character counter, 
      * sync character counter, total character counter.
      */
     int inputChar, frameCount, numberChar, syncCount, charCount, totChar;
     char* charBuffer;      //Buffer for the character
     char binChar[13];       //Character as a Binary String
     char asciiChar[8];
     char encoding[5];
     
        printf("Enter 'exit' for dir to quit. \n");
        printf("Enter 1 for Hamming Encoding \nEnter 2 For CRC\n");
        fgets(encoding, sizeof(encoding), stdin);
        if(strncmp(encoding,"exit",4)==0){
            return(EXIT_SUCCESS);
        }

        /* Opens the File for Reading
         */
        fins = fopen("output.bin", "rb");
        if (fins==NULL) {fputs ("File error",stderr); exit (1);}
        
        /* Finds the File Size */
        fseek (fins , 0 , SEEK_END);
        fSize = ftell (fins);
        rewind (fins);      
        
        /*Dyanmically allocate a buffer buff for the file*/
        buff = (char*) malloc (sizeof(char)*fSize/13-4);
        if (buff == NULL) {fputs ("Memory error",stderr); exit (2);}
        
        /*Initialize the counters*/
        frameCount=0;
        syncCount=0;
        numberChar=0;
        charCount=0;
        totChar=0;
        
        /*Name the file output*/
        strncpy(fileNameout,"output",6);
        
        /*Read in 8 "bits" at a time*/
        while(fgets(binChar,14,fins)!=NULL){
            charBuffer = (char*) malloc (sizeof(char));
            if (charBuffer == NULL) {fputs ("Memory error",stderr); exit (2);}
            
            /*
             * If inputChar is not a sync character and it is not the first
             * frame it is part of the file. Add it to the output file buffer.
             */
            if(encoding[0]=='1'){
                HammingDecode(binChar,asciiChar);
            }else if(encoding[0]=='2'){
                CRCDecode(binChar,asciiChar);
            }
            inputChar=Convert2ASCII(asciiChar);
            if(inputChar!=22&&numberChar!=0&&frameCount>0){
                sprintf(charBuffer,"%c",inputChar);
                strncat(buff,charBuffer, 1);
                charCount++;
            }
            /*
             * If inputChar is not a sync character and it is the first
             * frame it is part of the extension. Add it to the output file name. 
             */
            if(inputChar!=22&&numberChar!=0&&frameCount==0){
                sprintf(charBuffer,"%c",inputChar);
                strncat(fileNameout,charBuffer, 1);
                charCount++;
            }
            /*
             * If there has been one sync character and there is no character
             * number this must be the number of characters of the frame 
             */
            if(syncCount==1&&numberChar==0){
                numberChar=inputChar/13;
            }
            /*sync character increment syncCount*/
            if(inputChar==22){
                syncCount++;
            }
            /*
             * If there has been two sync characters the frame has ended reset
             * the comments.
             */
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
         * Open the output file and write the data to it. Then close the files
         * and free the buffer. 
         */
        fos = fopen(fileNameout, "wb");
        if (fos==NULL) {fputs ("File error",stderr); exit (1);}
        fwrite(buff,1,totChar-4,fos);
        printf("File Received. \nFile is in the same directory as the receiver program. \n"
               "File is named %s. Press enter to exit.", fileNameout);
        
        
        fclose(fins);
        fclose(fos);
        free(buff);
        fgets(buff, 9, stdin);


    return (EXIT_SUCCESS);
}

/*
 * A function to convert a binary string to an ascii character code. 
 */
int Convert2ASCII( char str[8]){
    int parity, i, pcount;
    double exponent;
    parity = 0;
    pcount = 0;
    
    for(i=0; i<8; i++){
        if(i==0){
            if(str[i] == '1'){
                pcount++;
                str[i]='0';
            }
            
        }
        else{
            if(str[i] == '1'){
                pcount++;
            }
        }
    }
    if(pcount%2 != 1)  
        {fputs ("Parity Error",stderr); exit (4);}
    return((int) strtol(str, NULL,2));
    
   
}
 
void HammingDecode (char hstr[13],char str[8]){
    int pOne = 0, pTwo=0, pFour=0, pEight=0;
    //char hstr[13];
    int i, pCount=0, errorBit=0, firstBit=0, oddErrors=0;
    int hstr1 =0, hstr2=0, hstr4=0, hstr8 =0, numErrorb=0;
    
    if(hstr[0]=='1'){
        firstBit=1;
    }if(hstr[1]=='1'){
        hstr1=1;
    }if(hstr[2]=='1'){
        hstr2=1;
    }if(hstr[4]=='1'){
        hstr4=1;
    }if(hstr[8]=='1'){
        hstr8=1;
    }
    
    
    for(i=1; i<13; i++){
        if(hstr[i]=='1'){
            pCount++;
        }
    }
    if(pCount%2==firstBit){
        oddErrors=1;
    }
    
    if (hstr[3]=='1'){
        pOne++;
        pTwo++;
    }if (hstr[5]=='1'){
        pOne++;
        pFour++;
    }if (hstr[6]=='1'){
        pTwo++;
        pFour++;
    }if (hstr[7]=='1'){
        pOne++;
        pTwo++;
        pFour++;
    }if (hstr[9]=='1'){
        pOne++;
        pEight++;
    }if (hstr[10]=='1'){
        pTwo++;
        pEight++;
    }if (hstr[11]=='1'){
        pOne++;
        pTwo++;
        pEight++;
    }if (hstr[12]=='1'){
        pFour++;
        pEight++;
    }
    if(pOne%2==hstr1){
        errorBit+=1;
        numErrorb++;
    }
    if(pTwo%2==hstr2){
        errorBit+=2;
        numErrorb++;
    }
    if(pFour%2==hstr4){
        errorBit+=4;
        numErrorb++;
    }
     if(pEight%2==hstr8){
        errorBit+=8;
        numErrorb++;
    } 
    
    if(numErrorb>0 && oddErrors!=1){
        fputs("Even Number of errors could not Correct!!!\n",stderr);
        exit(5);
    }else if(errorBit>12){
        fputs("More than 2 errors can not correct!!!!\n",stderr);
        exit(6);
    }else if(numErrorb>0 && oddErrors==1){
        fputs("One bit error detected and corrected.\n", stderr);
        if(hstr[errorBit]=='1'){
            hstr[errorBit]='0';
        }else{
            hstr[errorBit]='1';
        }
            
    }
    
    str[0]=hstr[3];
    str[1]=hstr[5];
    str[2]=hstr[6];
    str[3]=hstr[7];
    str[4]=hstr[9];
    str[5]=hstr[10];
    str[6]=hstr[11];
    str[7]=hstr[12];

    
}
void CRCDecode(char crstr[13],char str[8]){
int i,j,n=8,g=6,a=13,arr[13],gen[6];

for(i=0;i<a;i++){
    if(crstr[i]=='1'){
        arr[i]=1;
    }else{
        arr[i]=0;
    }
}
gen[0]=1;
gen[1]=0;
gen[2]=0;
gen[3]=0;
gen[4]=1;
gen[5]=1;
//Calculate CRC
for(i=0;i< n;++i){
    if(arr[i]==0){
        for(j=i;j< g+i;++j)
        arr[j] = arr[j]^0;
    }else{
        arr[i] = arr[i]^gen[0];
        arr[i+1]=arr[i+1]^gen[1];
        arr[i+2]=arr[i+2]^gen[2];
        arr[i+3]=arr[i+3]^gen[3];
        arr[i+4]=arr[i+4]^gen[4];
        arr[i+5]=arr[i+5]^gen[5];
    }
}
for(i=0;i<a;i++){
    if(arr[i]==1){
        fputs("CRC Detected an error!!!\n",stderr);
        exit(7);
    }
}
for(i=0;i<n;i++){
    str[i]=crstr[i];
}


}