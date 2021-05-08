#include "20170286.h"

int main(){
    int mode;
    initMemory();
    memset(mem,0,sizeof(mem));
    //  *(float*)(mem+0)=3.123;
     dump_mem(mem,sizeof(mem));
     while(1){
        mode = selectMode();
        if(mode==1){
            allocate();
        }else if(mode==2){
            deallocate();
        }else{
            repeatPlz();
            continue;
        }
     }
    return 0;
}