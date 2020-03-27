#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#define BUFFER_SIZE1 4
#define BUFFER_SIZE2 16
#define BUFFER_SIZE3 1024
#define BUFFER_SIZE4 16 * 1024

typedef struct testconf {
    char * fileName;
    int read;
    int write;
    int random;
    int sequential;
    size_t bufferSize;
    size_t fileSize;
} test_config;

void randomRead(char *fileName, size_t bufferSize){
    FILE *fp;
    bufferSize = bufferSize * 1024;
    
    char * buff =(char *) malloc(bufferSize);
    int repeat;
    int count;
    int total = 0;
    
    // open file
    fp = fopen(fileName, "rb");
    // get fileSize
    fseek(fp, 0, SEEK_END);
    size_t fileSize = ftell(fp);
    printf("file size is :%ld\n", fileSize);
    repeat = fileSize / bufferSize + 1;
    for(int i = 0; i < repeat; i++){
        int randomIndex = rand() % repeat;
        fseek(fp, randomIndex * bufferSize, SEEK_SET);
        count = fread(buff, sizeof(char), bufferSize, fp);
        printf("count is : %d", count);
        total += count;
    }
    printf("total read bytes is : %d", total);

    fclose(fp);
    free(buff);
}

void randomWrite(char *fileName, size_t bufferSize, size_t fileSize){
    FILE *fp;
    bufferSize = bufferSize * 1024;
    
    char * buff =(char *) malloc(bufferSize);
    memset(buff, 'a', bufferSize);
    int repeat;
    int count;
    int total = 0;

     // open file
    fp = fopen(fileName, "wb");
    
    repeat = fileSize / bufferSize + 1;
    for(int i = 0; i < repeat; i++){
        int randomIndex = rand() % repeat;
        fseek(fp, randomIndex * bufferSize, SEEK_SET);
        count = fwrite(buff, sizeof(char), bufferSize, fp);
        printf("count is : %d", count);
        total += count;
    }
    printf("total write bytes is : %d", total);

    fclose(fp);
    free(buff);
}
void sequentialRead(char *fileName, size_t bufferSize){
    FILE *fp;
    bufferSize = bufferSize * 1024;
    char * buff =(char *) malloc(bufferSize);
    int count;
    int total = 0;
    
    
    
    fp = fopen(fileName, "rb");
    while(1){
           count = fread(buff, sizeof(char), bufferSize, fp);
           printf("count is : %d", count);
           total += count;
           if(count < bufferSize){
               break;
           }
    }
    printf("total read bytes is : %d", total);

    fclose(fp);
    free(buff);
}
void sequentialWrite(char * fileName, size_t bufferSize, size_t fileSize){
    FILE *fp;
    bufferSize = bufferSize * 1024;
    char * buff =(char *) malloc(bufferSize);
    memset(buff, 'a', bufferSize);
    int count;
    int total = 0;

    fp = fopen(fileName, "wb");
   
    while(1){
           if(fileSize - total >= bufferSize){
                count = fwrite(buff, sizeof(char), bufferSize, fp);
                fflush(fp);
                printf("count is : %d", count);
                total += count;
           } else {
                count = fwrite(buff, sizeof(char), fileSize - total, fp);
                fflush(fp);
                printf("count is : %d", count);
                total += count;
                break;
           }
           
    }
    printf("total write bytes is : %d", total);

    fclose(fp);
    free(buff);
}
void* thread_worker(void *data){
    test_config * ioConfig = (test_config *) data;
    if(ioConfig -> read == 1 && ioConfig -> sequential == 1){
        sequentialRead( ioConfig -> fileName, ioConfig -> bufferSize);
        return NULL;
    }
    if(ioConfig -> write == 1 && ioConfig -> sequential == 1){
        sequentialWrite(ioConfig -> fileName, ioConfig -> bufferSize, ioConfig -> fileSize);
        return NULL;
    }
    if(ioConfig -> read == 1 && ioConfig -> random == 1){
        randomRead( ioConfig -> fileName, ioConfig -> bufferSize);
        return NULL;
    }
    if(ioConfig -> write == 1 && ioConfig -> random == 1){
        randomWrite(ioConfig -> fileName, ioConfig -> bufferSize, ioConfig -> fileSize);
        return NULL;
    }

}
int main() {
    test_config fileInfo;
    fileInfo.write = 1;
    fileInfo.random = 1;
    fileInfo.fileSize = 21335;
    fileInfo.fileName = "/tmp/test.txt";
    fileInfo.bufferSize = BUFFER_SIZE1;
    pthread_t t1;
    pthread_create(&t1, NULL, thread_worker, &fileInfo);
    pthread_join(t1, NULL);
    //sequentialRead("/tmp/test.txt", SIZE1); 
    //sequentialWrite("/tmp/test.txt", BUFFER_SIZE1, 213329);
    return 0;
}