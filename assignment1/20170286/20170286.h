#include<stdint.h>
#define BYTE 8

#define SIGNED_CHAR 1
#define ASCII_CODES 2
#define UNSIGNED_CHAR 3
#define SIGNED_INT 4
#define UNSIGNED_INT 5
#define FLOAT 6
#define DOUBLE 7
void readFile(char *, int);//char * 파일명 /int flag
void printResult(void *, int, int);//void* 저장된 메모리 주소,int memSize,int flag 
uint64_t stringToBit(char *, int);//char* 비트로 바꿀 0과1로 이루어진 문자열, int 문자열 크기 (최대 64)