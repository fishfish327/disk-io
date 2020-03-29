#include <ctype.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <<malloc.h>

#define BUFFER_SIZE1 4
#define BUFFER_SIZE2 16
#define BUFFER_SIZE3 1024
#define BUFFER_SIZE4 16 * 1024

typedef struct globalconf {
    int read;
    int write;
    int random;
    int sequential;
    size_t bufferSize;
    size_t fileSize;
} global_config;

typedef struct testconf {
    char * fileName;
    global_config * globalConfig;
} test_config;

void randomRead(char *fileName, size_t bufferSize){
    FILE *fp;
    bufferSize = bufferSize * 1024;
    
    char * buff =(char *) malloc(bufferSize * sizeof(char));
    int repeat;
    int count;
    size_t total = 0;
    
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
	    fflush(fp);
        fsync(fileno(fp));
        //printf("count is : %d", count);
        total += count;
    }
    printf("total read bytes is : %ld\n", total);

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
    size_t total = 0;

     // open file
    fp = fopen(fileName, "wb");
    
    repeat = fileSize / bufferSize + 1;
    for(int i = 0; i < repeat; i++){
        int randomIndex = rand() % repeat;
        fseek(fp, randomIndex * bufferSize, SEEK_SET);
        count = fwrite(buff, sizeof(char), bufferSize, fp);
        fflush(fp);
        fsync(fileno(fp));
        //printf("count is : %d", count);
        total += count;
    }
    printf("total write bytes is : %ld\n", total);

    fclose(fp);
    free(buff);
}

void readS(char * fileName, size_t bufferSize){
    bufferSize = bufferSize * 1024;
    char * buff =(char *) memalign(1024, bufferSize);
    int count;
    size_t total = 0;

    int fd;

    // open file
    fd = open(fileName, O_RDONLY | O_DIRECT);

    while(1){
        count = read(fd, buff, bufferSize);
        total += count;
        if(count < bufferSize){
            break;
        }
    }
    printf("total read bytes is : %ld\n", total);

    close(fd);
    free(buff);
}
void sequentialRead(char *fileName, size_t bufferSize){
    /*FILE *fp;
    bufferSize = bufferSize * 1024;
    char * buff =(char *) malloc(bufferSize);
    int count;
    size_t total = 0;
    
    
    
    fp = fopen(fileName, "rb");
    // set no buffer
    //setvbuf(fp, NULL, _IONBF, 0);
    while(1){
           count = fread(buff, sizeof(char), bufferSize, fp);
	       fflush(fp);
	       fsync(fileno(fp));
//         printf("count is : %d\n", count);
           total += count;
           if(count < bufferSize){
               break;
           }
    }
    printf("total read bytes is : %ld\n", total);

    fclose(fp);
    free(buff);*/
    readS(fileName, bufferSize);
}
void sequentialWrite(char * fileName, size_t bufferSize, size_t fileSize){
    FILE *fp;
    bufferSize = bufferSize * 1024;
    char * buff =(char *) malloc(bufferSize);
    memset(buff, 'a', bufferSize);
    int count;
    size_t total = 0;

    fp = fopen(fileName, "wb");
   
    while(1){
           if(fileSize - total >= bufferSize){
                count = fwrite(buff, sizeof(char), bufferSize, fp);
                fflush(fp);
                fsync(fileno(fp));
                //printf("count is : %d", count);
                total += count;
           } else {
                count = fwrite(buff, sizeof(char), fileSize - total, fp);
                fflush(fp);
                fsync(fileno(fp));
                //printf("count is : %d", count);
                total += count;
                break;
           }
           
    }
    printf("total write bytes is : %ld\n", total);

    fclose(fp);
    free(buff);
}
void* thread_worker(void *data){
    test_config * testConfig = (test_config *) data;
    char *fileName = testConfig -> fileName;
    global_config * ioConfig = testConfig -> globalConfig;
    printf("lauch a thread for io on : %s\n", fileName);
    if(ioConfig -> read == 1 && ioConfig -> sequential == 1){
        sequentialRead(fileName, ioConfig -> bufferSize);
        return NULL;
    }
    if(ioConfig -> write == 1 && ioConfig -> sequential == 1){
        sequentialWrite(fileName, ioConfig -> bufferSize, ioConfig -> fileSize);
        return NULL;
    }
    if(ioConfig -> read == 1 && ioConfig -> random == 1){
        randomRead(fileName, ioConfig -> bufferSize);
        return NULL;
    }
    if(ioConfig -> write == 1 && ioConfig -> random == 1){
        randomWrite(fileName, ioConfig -> bufferSize, ioConfig -> fileSize);
        return NULL;
    }

}
void getfileNameList(char ** fileNameList, char * configFileName, int n){
    FILE *fp;
    char *find;
    fp = fopen(configFileName, "r");
    for(int i = 0; i < n; i++){
        fileNameList[i] = (char *)malloc(255 * sizeof(char));
        fgets(fileNameList[i], 255, fp);

        find = strchr(fileNameList[i], '\n');          //查找换行符
        if(find){
           //如果find不为空指针
           *find = '\0';   
        }                            
             
        printf("get file name : %s\n", fileNameList[i]);
    }
    fclose(fp);
}
double getThroughput(double diff_t, size_t fileSize){
    // fileSize is in MB
    return (double)(fileSize / diff_t);
}
int main(int argc, char *argv[]) {
    int c;
    global_config globalConfig;
    test_config fileInfo;
    int numOfThreads;
    char *configFileName;
    char ** fileNameList;
    pthread_t * threadGroup;
    size_t totalFileSize;
    // start time && end time
    time_t start_t, end_t;
    double diff_t;
    double throughput;
    // parse arg
    while( -1 != (c = getopt(argc, argv,
          "b:"  /* block size */
          "s:"  /* size (bytes) to read/write */
          "n:"  /* num read threads */
          "f:"  /* config file name */
          "r"  /* read operation*/
          "w"  /* write operation*/
          "S"  /* sequential read & write*/
          "R"  /* random read & write*/
    ))){
        switch (c) {
            case 'b':
                  globalConfig.bufferSize =  (off_t)strtoul(optarg, NULL, 10);
                  break;
            case 's':
                  // use MB to represent file size
                  totalFileSize =  (off_t)strtoul(optarg, NULL, 10);
                  break;
            case 'n':
                  numOfThreads = atoi(optarg);
                  break;
            case 'f':
                  configFileName = optarg;
                  break; 
            case 'r':
                  globalConfig.read = 1;
                  break;
            case 'w':
                  globalConfig.write = 1;
                  break;
            case 'S':
                  globalConfig.sequential = 1;
                  break;
            case 'R':
                  globalConfig.random = 1;
                  break;
        }
    }
    if(numOfThreads > 0){
        fileNameList =(char **) malloc(numOfThreads * sizeof(char *));
        threadGroup = (pthread_t *) malloc(numOfThreads * sizeof(pthread_t));
        // for write operation we need a file size
        if(globalConfig.write == 1){
            // total file size : MB -> transfer to byte 
            globalConfig.fileSize = (size_t)(totalFileSize * 1024 * 1024 / numOfThreads);
        }
        getfileNameList(fileNameList, configFileName, numOfThreads);
        time(&start_t);
        // lauch thread to execute
        test_config *configGroup = malloc(numOfThreads * sizeof(test_config));
        for(int i = 0; i < numOfThreads; i++){
            configGroup[i].globalConfig = &globalConfig;
            configGroup[i].fileName = fileNameList[i];
            pthread_create(&threadGroup[i], NULL, thread_worker, &configGroup[i]);
        }

        // join per thread
        for(int i = 0; i < numOfThreads; i++){
            pthread_join(threadGroup[i], NULL);
        }
        time(&end_t);
        diff_t = difftime(end_t, start_t);
        throughput = getThroughput(diff_t, totalFileSize);
        printf("Execution time = %f\n", diff_t);
        printf("IO throughput is: %f\n", throughput);
        printf("Exiting of the program...\n");
        for(int i = 0; i < numOfThreads; i++){
            free(fileNameList[i]);
        }
        free(fileNameList);
        free(threadGroup);
        free(configGroup);
    }
    
    return 0;
}
