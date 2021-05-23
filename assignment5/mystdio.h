#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFSIZE 200
#define EOF -1
#define stdin 1
#define stdout 2
#define stderr 3
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define DEFFILEMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
typedef struct myFile
{
    int fd;
    int size;
    int actual_size;
    int mode;
    int flag;
    char lastop;
    bool eof;
    bool eof_seen;
    char *buffer_base;//버퍼 베이스는 바뀌지 않는다.
    char *write_buf;
    char *buffer_end;
    char *write_buf_end;
    char * cursor;
    bool read;
    bool write;
    bool append;
    bool write_start;
} FILE;
void check_eof(FILE* fp,size_t want,size_t have){
    if(fp->eof_seen){//파일의 마지막 버퍼에서
        if(want>have){//가지고있는것보다 많이 원하면
            fp->eof =true;//eof
            return ;
        }
    }
    fp->eof = false;
}
int buffer_to_file_write(FILE *fp,size_t size)
{
    int tmp = write(fp->fd,fp->buffer_base,size);
    if(tmp == -1)
        return tmp;
    memset(fp->buffer_base,0,size);
    lseek(fp->fd,-size,SEEK_CUR);
    lseek(fp->fd,fp->cursor-fp->buffer_base,SEEK_CUR);
    fp->cursor=fp->buffer_base;
    fp->buffer_end=fp->buffer_base;
    return tmp;
}

//1.fopen
FILE *fopen(const char *pathname, const char *mode)
{
    FILE *resFILE = malloc(sizeof(FILE));
    memset(resFILE,0,sizeof(FILE));
    int flag_flag, mode_fd, flag_mode;
    switch (*mode)
    {
    case 'r':
        resFILE->read = true;
        flag_mode = O_RDONLY;
        flag_flag = O_RDONLY;
        mode_fd = 0;
        break;
    case 'w':
        resFILE-> write = true;
        flag_mode = O_WRONLY;
        flag_flag =  O_TRUNC | O_CREAT;
        mode_fd = DEFFILEMODE;
        break;
    case 'a':
        resFILE->write = true;
        resFILE->append = true;
        flag_mode = O_WRONLY;
        flag_flag =  O_APPEND | O_CREAT;
        mode_fd = DEFFILEMODE;
        break;
    default:
        free(resFILE);
        return NULL;
    }
    if (*(mode + 1) == '+')
    {
        resFILE->read = true;
        resFILE->write = true;
        flag_mode = O_RDWR;
    }
    flag_flag = flag_flag | flag_mode;
    if (*mode == 'r')
    {
        resFILE->fd = open(pathname, flag_flag);
    }
    else
    {
        resFILE->fd = open(pathname, flag_flag, mode_fd);
    }
    resFILE->flag = flag_flag;
    resFILE->mode = mode_fd;
    if (resFILE->fd == -1)
    {
        close(resFILE->fd);//파일열기 실패
        free(resFILE);
        return NULL;
    }
    return resFILE;
}

//2.fread
void buffer_clear_and_sync(FILE*fp,size_t size){
    memset(fp->buffer_base, 0, size); //버퍼 비우기
    lseek(fp->fd,size,SEEK_CUR);
    lseek(fp->fd,-size,SEEK_CUR);
    lseek(fp->fd,fp->cursor-fp->buffer_base,SEEK_CUR);
    fp->cursor = fp->buffer_base;
    

}
int file_to_buffer_read(FILE* fp, size_t size){//최대 버퍼사이즈 까지 임
    if(size >BUFSIZE)
        exit(1);
    int tmp = read(fp->fd,fp->buffer_base,size);
    if(tmp == -1)//read fail write only file discriptor
        return -1;
    if(tmp!= size){
        fp->eof_seen =true;
    }
    fp->buffer_end = fp->buffer_base + tmp;
    fp->cursor = fp->buffer_base;
    lseek(fp->fd,-tmp,SEEK_CUR);
    return tmp;//몇개가 버퍼에 들어갔는지 알려줌
}
void do_allocate_buffer(FILE *fp)
{
    if (fp->buffer_base)
        return;
    fp->buffer_base=malloc(BUFSIZE);
    
    
}
int fread_sub_program1(FILE *fp, char *buf, int requested)
{
    size_t want, have;
    ssize_t count;
    char *s = buf;
    want = requested;
    if (fp->buffer_base == NULL)
    { //열려있던 버퍼가 없다면
        do_allocate_buffer(fp);
        int tmp = file_to_buffer_read(fp,BUFSIZE);
        if(tmp == -1)//read fail write only file discriptor
           return -1;
    }
    //읽어야 할것이 남아 있다면
    while (want > 0)
    {
        have = fp->buffer_end  - fp->cursor; //읽을 수 있는 공간
        check_eof(fp,want,have);
        if (want <= have)
        { //읽어야 할게 buf 읽을 수 있는 공간보다작으면
            memcpy(s, fp->cursor, want);
            fp->cursor += want;
            s+= want;
            want = 0;
        }
        else
        { //want >=have 일때
            
            if (have >= 0)
            {                                      // 버퍼에 공간이 아직남아있을 경우 (딱뎀이 아닐경우)
                memcpy(s, fp->cursor, have); // 딱뎀에 맞게 읽는다.(버퍼를 가득채운다)
                fp->cursor += have;
                s += have;                               //읽은 만큼 이동시킴
                want = want - have;                      //딱뎀 맞추고 남는 want
                // fp->cursor =fp->buffer_base;          //딱뎀이 맞게 커서를 이동한다.
                if(fp->write) buffer_to_file_write(fp,fp->cursor-fp->buffer_base);
                else buffer_clear_and_sync(fp,BUFSIZE);
                // fp->position_read = index;
            }
            //딱뎀을 맞춘 후 Read버퍼 처음부터 시작 (버퍼를 가득 채운 후)
            if(want<BUFSIZE){//남아있는 want <BUFSIZE
                int tmp =  file_to_buffer_read(fp, BUFSIZE);
                if(tmp == -1) return -1;
                if(tmp<BUFSIZE){// 버퍼를 다 채우지 못한 경우( 파일의 끝 부분) tmp == have
                    if(want >= tmp){
                        memcpy(s,fp->buffer_base,tmp);
                        fp->cursor +=tmp;
                        if(fp->cursor>fp->buffer_end) fp->buffer_end = fp->cursor;
                        s += tmp;
                        check_eof(fp,want,tmp);
                        want -=tmp;
                        break;
                    }else {
                        memcpy(s,fp->buffer_base,want);
                        fp->cursor += want;
                        s += want;
                        want = 0;
                        continue;
                    }
                }
                //버퍼를 다 채운 경우
                memcpy(s,fp->buffer_base,want);
                fp->cursor += want;
                s+= want;
                want = 0;
                continue;
            }
            //남은 want 가 BuffSize 보다 클때 
            count = want; 
            count = count - want%BUFSIZE;
            int tmp = read(fp->fd,s,count);// 버퍼를 거치지 않고 바로 읽기
            if(tmp == -1) return -1;
            if( tmp < count){// 다 못읽었을 경우 (도중에 파일의 끝을 만남)
                fp->buffer_end = fp->buffer_base;//버퍼 닫기
                fp->eof = true;
                return tmp;
            }
            s += count;
            want -= count;
        }
    }
    return requested - want;
}




int fread(void *buf, int size, int nmemb, FILE *fp)
{
    size_t byte_requested = size * nmemb; //요청하는 바이트
    size_t byte_read;

    if (byte_requested == 0)
        return 0;
    if(fp->read == false)
        return 0;
    byte_read = fread_sub_program1(fp, (char *)buf, byte_requested); // 읽는데 성공한 바이트 수
    return byte_requested == byte_read ? nmemb : byte_read / size;
}

//3.fwrite


void do_allocate_write_buffer(FILE *fp)
{
    if (fp->write_buf)
        return;
    fp->write_buf=malloc(BUFSIZE);
}
int fwrite_sub_program1(FILE* fp, const char* data,size_t n){
    char*s = (char*)data;//우라가 적을 데이
    size_t have;
    size_t to_do = n;
    size_t count = 0;
    if(n <=0)
        return 0;
    if(fp->buffer_base==NULL){
        do_allocate_buffer(fp);
        if(fp->flag &O_APPEND){
            fp->buffer_end =fp->buffer_base;
            fp->cursor = fp-> buffer_base;
            fp->write_start = true;
        }else if(fp->flag & O_TRUNC){
            fp->buffer_end = fp->buffer_base;
            fp->cursor=fp->buffer_base;
        }else{
            file_to_buffer_read(fp,BUFSIZE);
        }

    }
    while(to_do >0)
    {
        have =BUFSIZE-(fp->cursor - fp->buffer_base);
        if(to_do<have){
            memcpy(fp->cursor,s,to_do);
            s += to_do;
            fp->cursor += to_do;
            if(fp->cursor>fp->buffer_end) fp->buffer_end = fp->cursor;
            to_do = 0;
        }
        else{
            if(have>=0){
                memcpy(fp->cursor,s,have);//가득 채운다.
                fp->cursor += have;
                s += have;
                to_do= to_do - have;
                if(fp->cursor>fp->buffer_end) fp->buffer_end = fp->cursor;
                buffer_to_file_write(fp,BUFSIZE);// 버퍼가 가득 찼으니 write실행

            }
            if(to_do<BUFSIZE){//가득채웠는데 남아있는 쓸 것이 버퍼크기보다 작다면
                if(fp->read == true){//리드가 허용 됨
                    int tmp = file_to_buffer_read(fp,BUFSIZE);
                }
                memcpy(fp->cursor,s,to_do);
                fp->cursor += to_do;
                if(fp->buffer_end < fp->cursor) fp->buffer_end = fp->cursor;
                s+= to_do;
                
                to_do = 0;
                continue;
            }
            //남은 todo가 buffersize보다 큼
            count = to_do;
            count = count - to_do%BUFSIZE;
            int tmp = write(fp->fd,s,count);// buf size의 배수만큼 쓰기
            s+= count;
            to_do -= count;//todo는 무조건 bufsize보다 작다
        }
    }
    return n-to_do;
}
int fwrite(const void *ptr, ssize_t size, ssize_t nmemb, FILE *fp){
    int request = size*nmemb;
    int written = 0;
    if(request == 0)
        return 0;
    if(fp->write == false)
        return 0;
    written = fwrite_sub_program1(fp,(const char*)ptr, request);
    if(written == request|| written ==EOF)
        return nmemb;
    else
        return written/size;
}

//4.fgetc
int fgetc(FILE *fp){
    unsigned char res;
    int sucess = fread(&res,1,1,fp);
    if(sucess) return (int)res;
    else return EOF;
}

//5.fputc
int fputc(int c, FILE * fp){
    unsigned char puts = c;
    int sucess = fwrite(&puts,1,1,fp);
    if(sucess) return puts;
    else return EOF;
}
//6.fflush
int fflush(FILE* fp){
    size_t blockSize = fp->buffer_end- fp->buffer_base;
    if(fp->buffer_base ==NULL) return EOF;
    if(fp->flag &O_APPEND){
        if(fp->write_start == true){
                buffer_to_file_write(fp,blockSize);
                return 0;
        }else{
            buffer_clear_and_sync(fp,blockSize);
            return EOF;
        }
    }
    else if(fp->write == true){ 
        buffer_to_file_write(fp,blockSize);
        return 0;
    }else {
        buffer_clear_and_sync(fp,blockSize);
        return EOF;
    }
}
//7.fseek
int fseek(FILE* fp, int offset,int whence){
    // int have =fp->buffer_end-fp->cursor;
    fflush(fp);
    free(fp->buffer_base);
    fp->buffer_base = NULL;
    fp->buffer_end = NULL;
    fp->cursor = NULL;
    fp->eof_seen =false;
    fp->eof =false;
    int res = lseek(fp->fd,offset,whence);
    if(res == -1) return EOF;
    else 0;
}
//8.feof
int feof(FILE *fp)
{
    if (fp->eof)

        return EOF;
    else
        return 0;
}
//9.fclose
int fclose(FILE *fp)
{
    int blockSize = fp->buffer_end - fp->buffer_base;
    if(fp==NULL) return EOF;
    if(fp->write == true)
        fflush(fp);
    free(fp->buffer_base);
    close(fp->fd);
    free(fp);
    fp =NULL;
    return 0;
}