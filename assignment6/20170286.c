#include "20170286.h"

// An example of a semphore for the mutual exclusion
semaphore_t mutex;
FILE *fp;
char str[BUFSIZ];
void reader(void)
{
    while (1)
    { 
	// your code
        P(&mutex);
        if(feof(fp)){
            printf("Thread %d : File END...\n",get_thread_id());
            exit(1);

            
        }
            
        printf("Thread %d is reading a line\n",get_thread_id());
        fgets(str+strlen(str),BUFSIZ,fp);
        V(&mutex);
    }
    V(&mutex);
}

void writer(void)
{
    while(1){
        P(&mutex);
        if(strlen(str)<3){
            printf("Thread %d no more Write from file\n",get_thread_id());
            printf("exit the program...\n");
            break;
        }
        printf("Thread %d is writing 3lines\n",get_thread_id());
        
        fwrite(str,1,strlen(str),stdout);
        fflush(stdout);
        memset(str,0,BUFSIZ);
        V(&mutex);
    }
	// your code
}

int main(int argc, char *argv[])
{
	// example of initializing a semaphore
    mutex = sem_create(1);
    // your code
    if(argv[1]==NULL){
        printf("wrong input \nplease input like this: ./20172086 \"input path\"\n ");
        return 0;
    }
    fp= fopen(argv[1],"r");
    // create threads
    create_thread(reader);
    create_thread(reader);
    create_thread(reader);
    create_thread(writer);
    //que 상태 : reader1 - reader2 - reader3 - writer1(4)
    // Let's run
    run();//reader 1 꺼내기 reader(void) 함수 실행
    
    // stupid while loop
    while (1) {
    }
}   




