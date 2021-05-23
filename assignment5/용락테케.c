#include "mystdio.h"
#include <assert.h>

#define PATH "/home/ssu20170286/시프/assignment5/test"
#define TBUFSIZE 1000

void prepareFile()
{
    char *test = "012345678901234567890123456789012345678901234567890123456789";
    FILE *fp = fopen(PATH, "w");
    fwrite(test, 1, 60, fp);
    if(fp == NULL)
    {
        write(stdout, "Prepare error\n", 15);
        exit(0);
    }
    fclose(fp);
}

void getAllString(char* data)
{
    FILE *fp = fopen(PATH, "r");
    fread(data, sizeof(char), TBUFSIZE, fp);
    fclose(fp);
}

void rModeTest()
{
    FILE *read = fopen(PATH, "r");
    char *buffer = malloc(60);
    int readCheck = fread(buffer, 5, 12, read);
    assert(readCheck == 12);
    assert(buffer[0] == '0');
    assert(buffer[2] == '2');
    assert(buffer[4] == '4');
    assert(buffer[6] == '6');
    assert(buffer[8] == '8');
    assert(buffer[10] == '0');
    assert(buffer[21] == '1');
    assert(buffer[23] == '3');
    assert(buffer[25] == '5');
    assert(buffer[40] == '0');
    assert(buffer[42] == '2');
    assert(buffer[44] == '4');
    assert(buffer[46] == '6');
    assert(buffer[48] == '8');
    assert(buffer[51] == '1');
    assert(buffer[53] == '3');
    assert(buffer[55] == '5');
    assert(buffer[59] == '9');
    // assert(lseek(read->fd, 0, SEEK_CUR) == 60);

    fseek(read, -5, SEEK_CUR);
    // assert(lseek(read->fd, 0, SEEK_CUR) == 55);

    int writeCheck = fwrite("message", 1, 7, read);
    assert(writeCheck == 0);

    int getc = fgetc(read);
    assert(getc == '5');
    getc = fgetc(read);
    assert(getc == '6');
    getc = fgetc(read);
    assert(getc == '7');
    getc = fgetc(read);
    assert(getc == '8');
    getc = fgetc(read);
    assert(getc == '9');
    getc = fgetc(read);
    assert(getc == EOF);

    assert(feof(read) == EOF);

    int putc = fputc(60, read);
    assert(putc == EOF);
    
    char *buffer2 = malloc(100 * sizeof(char));
    memset(buffer2, 'a', 100 * sizeof(char));
    fseek(read, -37, SEEK_CUR);
    // assert(lseek(read->fd, 0, SEEK_CUR) == 23);
    readCheck = fread(buffer2, 1, 5, read);
    assert(readCheck == 5);
    assert(buffer2[0] == '3');
    assert(buffer2[1] == '4');
    assert(buffer2[2] == '5');
    assert(buffer2[3] == '6');
    assert(buffer2[4] == '7');  
    assert(buffer2[5] == 'a');
    assert(buffer2[10] == 'a');
    assert(buffer2[50] == 'a');
    // assert(lseek(read->fd, 0, SEEK_CUR) == 60);

    free(buffer);
    free(buffer2);
}

void wModeTest()
{
    FILE *write = fopen(PATH, "w");
    char *buffer1 = malloc(100 * sizeof(char));
    memset(buffer1, 'a', 100 * sizeof(char));
    int readTest = fread(buffer1, 1, 100, write);
    assert(readTest == 0);
    assert(buffer1[0] == 'a');
    assert(buffer1[99] == 'a');
    int writeTest = fwrite("ABCDEFGHIJK", 11, 1, write);
    assert(writeTest == 1);
    
    char *buffer2 = malloc(11 * sizeof(char));
    memset(buffer2, 'b', 11 * sizeof(char));
    getAllString(buffer2);
    assert(buffer2[0] == 'b');
    assert(buffer2[5] == 'b');
    assert(buffer2[10] == 'b');
    int flushTest = fflush(write);
    assert(flushTest == 0);
    getAllString(buffer2);
    assert(buffer2[0] == 'A');
    assert(buffer2[5] == 'F');
    assert(buffer2[10] == 'K');
    // assert(lseek(write->fd, 0, SEEK_CUR) == 11);

    fseek(write, -5, SEEK_CUR);
    fputc('Z', write);
    getAllString(buffer2);
    assert(buffer2[6] == 'G');
    fflush(write);
    getAllString(buffer2);
    assert(buffer2[6] == 'Z');
    int getcTest = fgetc(write);
    assert(getcTest == EOF);
    assert(feof(write) == 0);

    free(buffer1);
    free(buffer2);
}

void aModeTest()
{
    FILE *append = fopen(PATH, "a");
    char *buffer1 = malloc(100 * sizeof(char));
    memset(buffer1, 'a', 100 * sizeof(char));
    int readTest = fread(buffer1, 1, 100, append);
    assert(readTest == 0);
    assert(buffer1[0] == 'a');
    assert(buffer1[99] == 'a');
    int writeTest = fwrite("ABCDEFGHIJK", 1, 11, append);
    assert(writeTest == 11);
    
    char *buffer2 = malloc(300 * sizeof(char));
    memset(buffer2, 'b', 300 * sizeof(char));
    getAllString(buffer2);
    assert(buffer2[0] == '0');
    assert(buffer2[5] == '5');
    assert(buffer2[10] == '0');

    int flushTest = fflush(append);
    assert(flushTest == 0);
    getAllString(buffer2);
    assert(buffer2[0] == '0');
    assert(buffer2[5] == '5');
    assert(buffer2[10] == '0');
    assert(buffer2[60] == 'A');
    assert(buffer2[70] == 'K');
    // assert(lseek(append->fd, 0, SEEK_CUR) == 71);

    fseek(append, -10, SEEK_CUR);
    fputc('Z', append);
    getAllString(buffer2);
    assert(buffer2[62] == 'C');
    fflush(append);
    getAllString(buffer2);
    assert(buffer2[71] == 'Z');
    assert(buffer2[69] == 'J');
    int getcTest = fgetc(append);
    assert(getcTest == EOF);
    assert(feof(append) == 0);

    free(buffer1);
    free(buffer2);
}

void rplusModeTest()
{
    FILE *readp = fopen(PATH, "r+");
    char *buffer = malloc(60);
    
    fwrite("ABCD", 1, 4, readp);

    // assert(lseek(readp->fd, 0, SEEK_CUR) == 0);
    int readCheck = fread(buffer, 1, 60, readp);
    assert(readCheck == 56);
    assert(buffer[0] == '4');
    assert(buffer[1] == '5');
    assert(buffer[2] == '6');
    // assert(lseek(readp->fd, 0, SEEK_CUR) == 60);

    char *buffer1 = malloc(100 * sizeof(char));
    memset(buffer1, 'b', 100 * sizeof(char));
    getAllString(buffer1);
    
    assert(buffer1[0] == 'A');
    assert(buffer1[1] == 'B');
    assert(buffer1[3] == 'D');
    assert(buffer1[5] == '5');
    assert(buffer1[7] == '7');
    assert(buffer1[10] == '0');
    assert(buffer1[21] == '1');
    assert(buffer1[23] == '3');
    assert(buffer1[25] == '5');
    assert(buffer1[40] == '0');
    assert(buffer1[42] == '2');
    assert(buffer1[43] == '3');
    assert(buffer1[45] == '5');
    assert(buffer1[46] == '6');
    assert(buffer1[59] == '9');
    assert(buffer1[60] == 'b');

    fseek(readp, -30, SEEK_CUR);
    // assert(lseek(readp->fd, 0, SEEK_CUR) == 30);
    char *buffer2= malloc(7);
    fread(buffer2, 1, 7, readp);
    // assert(lseek(readp->fd, 0, SEEK_CUR) == 60);
    fwrite("ABCDEF", 1, 6, readp); 
    getAllString(buffer1);
    assert(buffer1[37] == '7');
    assert(buffer1[38] == '8');
    assert(buffer1[39] == '9');
    assert(buffer1[40] == '0');
    assert(buffer1[41] == '1');
    assert(buffer1[42] == '2');

    fflush(readp);
    getAllString(buffer1);
    // assert(lseek(readp->fd, 0, SEEK_CUR) == 43);
    assert(buffer1[37] == 'A');
    assert(buffer1[38] == 'B');
    assert(buffer1[39] == 'C');
    assert(buffer1[40] == 'D');
    assert(buffer1[41] == 'E');
    assert(buffer1[42] == 'F');
    
    assert(fgetc(readp) == '3');
    assert(fputc('A', readp));
    assert(fgetc(readp) == '5');
    assert(feof(readp) == 0);
    fread(buffer1, 1, 15, readp);
    assert(feof(readp) == EOF);

    free(buffer);
    free(buffer1);
    free(buffer2);
}

void wplusModeTest()
{
    FILE *writep = fopen(PATH, "w+");
    char *buffer = malloc(60);
    memset(buffer, 'z', 60);
    fwrite("01234567890123456789", 1, 20, writep);
    getAllString(buffer);
    assert(buffer[0] == 'z');
    assert(buffer[10] == 'z');

    fflush(writep);
    getAllString(buffer);
    assert(buffer[0] == '0');
    assert(buffer[3] == '3');
    assert(buffer[5] == '5');
    assert(buffer[10] == '0');
    assert(buffer[19] == '9');
    assert(buffer[20] == 'z');
    assert(buffer[21] == 'z');

    fseek(writep, 0, SEEK_SET);
    fwrite("ABCDE", 1, 5, writep);
    getAllString(buffer);
    assert(buffer[0] == '0');
    assert(buffer[3] == '3');
    assert(buffer[5] == '5');
    assert(buffer[10] == '0');
    assert(buffer[19] == '9');
    assert(buffer[20] == 'z');
    assert(buffer[21] == 'z');

    int readCheck = fread(buffer, 1, 60, writep);
    assert(readCheck == 15);
    assert(buffer[0] == '5');
    assert(buffer[1] == '6');
    assert(buffer[2] == '7');
    assert(buffer[3] == '8');
    assert(buffer[4] == '9');
    assert(buffer[5] == '0');
    assert(buffer[6] == '1');

    fseek(writep, -5, SEEK_CUR);
    fputc('A', writep);
    fputc('B', writep);
    char *buffer1 = malloc(100);
    memset(buffer1, 'z', 100);

    getAllString(buffer1);
    assert(buffer1[15] == '5');
    assert(buffer1[16] == '6');

    fflush(writep);
    getAllString(buffer1);
    assert(buffer1[15] == 'A');
    assert(buffer1[16] == 'B');
    // assert(lseek(writep->fd, 0, SEEK_CUR) == 17);
    assert(fgetc(writep) == '7');
    // assert(lseek(writep->fd, 0, SEEK_CUR) == 20);
    fseek(writep, -2, SEEK_CUR);
    // assert(lseek(writep->fd, 0, SEEK_CUR) == 16);
    fread(buffer, 1, 100, writep);
    assert(feof(writep) == EOF);

    free(buffer);
    free(buffer1);
}

void aplusModeTest()
{
    FILE *appendp = fopen(PATH, "a+");
    char *buffer = malloc(100);
    memset(buffer, 'y', 100);
    int readCheck = fread(buffer, 2, 10, appendp);
    assert(readCheck == 10);
    assert(buffer[0] == '0');
    assert(buffer[1] == '1');
    assert(buffer[2] == '2');
    assert(buffer[3] == '3');
    assert(buffer[4] == '4');
    assert(buffer[5] == '5');
    assert(buffer[6] == '6');
    assert(buffer[7] == '7');
    assert(buffer[8] == '8');
    assert(buffer[9] == '9');
    assert(buffer[20] == 'y');
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 60);

    fseek(appendp, 0, SEEK_SET);
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 0);
    int writeCheck = fwrite("ABCDEFGHIJ", 2, 5, appendp);
    assert(writeCheck == 5);
    getAllString(buffer);
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 0);
    assert(buffer[60] == 'y');
    assert(buffer[61] == 'y');
    assert(buffer[62] == 'y');
    fflush(appendp);
    getAllString(buffer);
    assert(buffer[60] == 'A');
    assert(buffer[61] == 'B');
    assert(buffer[62] == 'C');
    assert(buffer[63] == 'D');
    assert(buffer[64] == 'E');
    assert(buffer[70] == 'y');
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 70);
    
    assert(fgetc(appendp) == EOF);
    assert(feof(appendp) == EOF);
    fseek(appendp, -50, SEEK_CUR);
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 20);
    fputc('0', appendp);
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 20);
    fflush(appendp);
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 71);
    fseek(appendp, 0, SEEK_SET);
    fseek(appendp, 0, SEEK_END);
    // assert(lseek(appendp->fd, 0, SEEK_CUR) == 71);
    assert(feof(appendp) == 0);
    assert(fgetc(appendp) == EOF);
    assert(feof(appendp) == EOF);

    free(buffer);
}

int main()
{
    
    write(stdout, "NOTICE: BUFSIZE of \"mystido.h\" should be more than 100\n", 56);
    write(stdout, "Please check the PATH before launch this program\n", 50);
    write(stdout, "You should define stdin = 0, stdout = 1, stderr = 2, EOF = -1 in \"mystdio.h\"\n", 78);
    write(stdout, "---------------------------------------TEST START----------------------------------------\n", 91);
    prepareFile();
    rModeTest();
    write(stdout, "[Success]: \"r\" mode test, contain fread & fwrite & fgetc & fputc & fseek & fflush & feof\n", 90);

    wModeTest();
    write(stdout, "[Success]: \"w\" mode test, contain fread & fwrite & fgetc & fputc & fseek & fflush & feof\n", 90);

    prepareFile();
    aModeTest();
    write(stdout, "[Success]: \"a\" mode test, contain fread & fwrite & fgetc & fputc & fseek & fflush & feof\n", 90);

    prepareFile();
    rplusModeTest();
    write(stdout, "[Success]: \"r+\" mode test, contain fread & fwrite & fgetc & fputc & fseek & fflush & feof\n", 91);

    wplusModeTest();
    write(stdout, "[Success]: \"w+\" mode test, contain fread & fwrite & fgetc & fputc & fseek & fflush & feof\n", 91);

    prepareFile();
    aplusModeTest();
    write(stdout, "[Success]: \"a+\" mode test, contain fread & fwrite & fgetc & fputc & fseek & fflush & feof\n", 91);

    write(stdout, "---------------------------------TEST IS DONE, CONGRATS----------------------------------\n", 91);
}