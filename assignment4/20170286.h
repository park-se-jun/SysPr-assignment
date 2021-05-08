#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
void notEnoughMemory();
void repeatPlz();
int selectMode();
void dump_mem(const void *mem, size_t len);
int isAlreadyName(char* name);
int isSizeOver();
void allocate();
void allocate_SelectTypeAndName();
void allocate_Char();
void allocate_Int();
void allocate_Struct();
void allocate_Double();
void allocate_Struct_value(int memberCount,char*name);
void deallocate();

int parseLine(char *sArr[], int size);
char mem[256];
typedef struct varinfo{
    char name[32];
    unsigned char size;
    struct varinfo* next;
    struct varinfo* prev;
} varinfo;
varinfo* varlist;
void initMemory(){
    memset(mem,0,sizeof(mem));
    varlist=calloc(1,sizeof(varinfo));
    strcpy(varlist->name," ");
}
varinfo *getLastNode(varinfo* list){
    varinfo* node=list;
    while(node->next!=NULL){
        node=node->next;
    }
    return node;
}
void removeNode(varinfo*node){
    varinfo* tmpNode;
    tmpNode=node->next;
    if(tmpNode!=NULL)
        tmpNode->prev=node->prev;

    tmpNode=node->prev;
    tmpNode->next=node->next;
    free(node);
}
void addNode(varinfo*node,varinfo*list){
    varinfo* lastNode=getLastNode(list);
    lastNode->next=node;
    node->prev=lastNode;
}
int getIndexOf(varinfo*node){
    varinfo* tmp = node;
    int index=0;
    while(tmp->prev!=NULL){
        tmp=tmp->prev;
        index+=tmp->size;
    }
    return index;
}
varinfo* findByName(varinfo*list,char*name){
    varinfo* node=list;
    while(node->next!=NULL){
        if(node->name==NULL){
            node=node->next;
            continue;
        }
        if(strcmp(node->name,name)==0)
            return node;
        else
            node=node->next;
    }
    if(strcmp(node->name,name)==0)
        return node;
    return NULL;
}
int getListSize(varinfo*list){
    int size=0;
    varinfo*node=list;
    while(node->next!=NULL){
        size+=node->size;
        node=node->next;
    }
    size+=node->size;
    return size;
}

int parseLine(char *sArr[], int size)
{
    char string[BUFSIZ];
    fgets(string, BUFSIZ, stdin);
    if (string[strlen(string) - 1] == '\n')
        string[strlen(string) - 1] = ' ';
    int i = 0;
    char *ptr = strtok(string, " ");
    while (ptr != NULL)
    {
        if (i == size)  //사이즈 초과의 토큰이면
            return 1;//종료
        sArr[i] = ptr;
        
        ptr = strtok(NULL, " ");
        i++;
    }
    if(i==size)//정확한 사이즈의 토큰이면
        return 0;
    else return -1;//보다 작은 토큰이면
}

void dump_mem(const void *mem, size_t len)
{
    const char *buffer = (char *)mem;
    size_t i;
    printf("Here is the mem\n");
    for (i = 0; i < len; i++)
    {
        if (i > 0 && i % 16 == 0)
        {
            printf("\n");
        }
        printf("%02x ", buffer[i] & 0xff);
    }
    puts("");
}

void notEnoughMemory(int rem_byte)
{
    printf("There is not enough memory for the data, you can only use %d byte(s)\n",rem_byte);
}
void repeatPlz()
{
    printf("please input (1) or (2)\n");
}
int selectMode()
{
    char *sArr[1] = {NULL,};
    printf("Do you want to allocate date(1) or deallocate data(2)?\n");
    // scanInt(ans);
    if (parseLine(sArr, 1)==0)//인자의 수가 알맞을 때
    {
        if(strcmp(sArr[0],"1")==0)
            return 1;
        else if(strcmp(sArr[0],"2")==0)
            return 2;
    } 
    return 0;
}
void allocate()
{
    allocate_SelectTypeAndName();
}
void allocate_SelectTypeAndName()
{
    char* token[2]={NULL,};
    while(1){
        printf("what is the type of data you want to allocate and what will be the name of the data?\n");
        if(parseLine(token,2)==0){
            if (strcmp(token[0], "char")==0)
            {
                allocate_Char(token[1]);
            }
            else if (strcmp(token[0], "int") == 0)
            {
                allocate_Int(token[1]);
            }
            else if (strcmp(token[0], "double") == 0)
            {
                allocate_Double(token[1]);
            }
            else if (strcmp(token[0], "struct") == 0)
            {
                allocate_Struct(token[1]);
            }
            else{
                printf("wrong datatype: use (char)|(int)|(struct)|(double)\n");
                continue;
            }
            // printf("data type : %s \nname : %s\n", token[0], token[1]);
            return ;
        }else 
            printf("wrong usage: use (datatype) (name)\n");
    }
}
int isAlreadyName(char* name){
    if(findByName(varlist,name)!=NULL){
        printf("Already exist name!\n");
        return 1;
    }else
        return 0;
}
int isSizeOver(int size){
    int listSize=getListSize(varlist);
    if(listSize+size>256){
        notEnoughMemory(256-listSize);
        return 1;
    }else return 0;
}
void allocate_Char(char* name){
    char* token[1]={NULL,};
    char value;
    unsigned char listSize;
    if(isAlreadyName(name)) return ;
    if(isSizeOver(sizeof(char))) return ;
    while(1){
        printf("Please specify a value for the data type\n");
        if(parseLine(token,1)==0){
            value=atoi(token[0]);
            break;
        }else
            continue;
    }    
    varinfo* node=calloc(1,sizeof(varinfo));
    strcpy(node->name,name);
    node->size=1;
    
    listSize=getListSize(varlist);
    *(char*)(mem + (listSize) ) = value;
    
    dump_mem(mem,sizeof(mem));
    addNode(node,varlist);
    return;
}
void allocate_Int(char* name){
    char* token[1]={NULL,};
    int value;
    unsigned char listSize;
    if(isAlreadyName(name)) return ;
    if(isSizeOver(sizeof(int))) return ;
    while(1){
        printf("Please specify a value for the data type\n");
        if(parseLine(token,1)==0){
            value=atoi(token[0]);
            break;
        }else
            continue;
    }    
    varinfo* node=calloc(1,sizeof(varinfo));
    strcpy(node->name,name);
    node->size=4;
    

    listSize=getListSize(varlist);
    *(int*)(mem + (listSize) ) = value;
    
    dump_mem(mem,sizeof(mem));
    addNode(node,varlist);
    return;
}
void allocate_Double(char* name){
    char* token[1]={NULL,};
    double value;
    unsigned char listSize;
    if(isAlreadyName(name)) return ;
    if(isSizeOver(sizeof(double))) return ;
    while(1){
        printf("Please specify a value for the data type\n");
        if(parseLine(token,1)==0){
            value=atof(token[0]);
            break;
        }else
            continue;
    }    
    varinfo* node=calloc(1,sizeof(varinfo));
    strcpy(node->name,name);
    node->size=8;
    

    listSize=getListSize(varlist);
    *(double*)(mem + (listSize) ) = value;

    addNode(node,varlist);
    dump_mem(mem,sizeof(mem));
    return;
}
void allocate_Struct(char*name){
    if(isAlreadyName(name)) return ;
    printf("How many data should be in the struct\n");
    char* token[1];
    int memberCount;
    if(parseLine(token,1)==0){
        memberCount =atoi(token[0]);
        allocate_Struct_value(memberCount,name);
    }else printf("wrong\n");
}
void allocate_Struct_value(int memberCount,char*name){
    printf("Please specify each type and its value\n");
    varinfo* node=calloc(1,sizeof(varinfo));
    char* token[2];
    int structSize=0;
    int listSize=getListSize(varlist);
    for(int i=0;i<memberCount;i++){
        if(parseLine(token,2)==0){
            if (strcmp(token[0], "char")==0)
            {
                if(isSizeOver(structSize+1)) 
                {
                    free(node);
                    return;
                }
                else *(char*)(mem + (listSize)+structSize) = atoi(token[1]);
                structSize+=1;
            }
            else if (strcmp(token[0], "int") == 0)
            {
                if(isSizeOver(structSize+4)) 
                {
                    free(node);
                    return;
                }
                else *(int*)(mem + (listSize)+structSize) = atoi(token[1]);
                structSize+=4;
            }
            else if (strcmp(token[0], "double") == 0)
            {
                if(isSizeOver(structSize+8))
                {
                    free(node);
                    return;
                }
                else 
                *(double*)(mem + (listSize)+structSize) = atof(token[1]);
                structSize+=8;
            }
            else{
                memset((void*)(mem+listSize),0,structSize);
                free(node);
                printf("allocate struct fail\n");
                dump_mem(mem,sizeof(mem));
                return ;
            }
        }
        else {
            memset((void*)(mem+listSize),0,structSize);
            free(node);
            printf("allocate struct fail\n");
            dump_mem(mem,sizeof(mem));
            return ;
        }
    }
    strcpy(node->name,name);
    node->size=structSize;
    addNode(node,varlist);
    dump_mem(mem,sizeof(mem));
}


void deallocate()
{
    char* token[1];
    varinfo* nodeToRemove;
    int indexOfDest=0;
    printf("what is the name of data you want to deallocate?\n");
    if(parseLine(token,1)==0){
        nodeToRemove = findByName(varlist,token[0]);
        if(nodeToRemove==NULL){
            printf("cant't find name :\"%s\"\n",token[0]);
            return ;
        }else{
            indexOfDest=getIndexOf(nodeToRemove);
            int indexOfSrc = indexOfDest+nodeToRemove->size;
            int sizeOfSrc = sizeof(mem)-(indexOfSrc);
            memcpy((void*)(mem+indexOfDest),(void*)(mem+indexOfSrc),sizeOfSrc);
            memset((void*)(mem +indexOfDest+sizeOfSrc),0,nodeToRemove->size);
            removeNode(nodeToRemove);
            dump_mem(mem,sizeof(mem));
        }
    }else dump_mem(mem,sizeof(mem));
}
