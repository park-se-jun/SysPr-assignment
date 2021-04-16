#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define MAX_NODE 5
#define READ_PIPE 0
#define WRITE_PIPE 1
int graph[5][5][2]; //fd graph 전역변수
int node[MAX_NODE]; //각 프로세스(노드)의 pid 전역변수
typedef enum _signal_type// 파이프를 통해 전달되는 시그널 종류
{
    YOUR_TURN,
    FINISHED_JOB,
    SHOUD_EXIT,
    GOOD_BYE
} signalType;

void initProgram();
bool printOneLine(int32_t fd);
ssize_t rcvSignal(int start, int end);
ssize_t sendSignal(int start, int end, signalType signal);
void openPipeBetween(int node1, int node2);
void closePipeExcept(int node);
void closePipeAll();
void inNode(int nowNode, int32_t fd);



//초기화 함수( 파이프 열고 닫고 포크하고)
void initProgram()
{
    int myPid;
    openPipeBetween(0, 1);
    openPipeBetween(1, 2);
    openPipeBetween(2, 3);
    openPipeBetween(3, 4);
    openPipeBetween(4, 0);
    myPid = getpid();
    node[0] = myPid;
    for (int i = 1; i < MAX_NODE; i++)
    {
        if ((node[i] = fork()) == 0)
        {
            closePipeExcept(i);
            myPid = getpid();
            node[i] = myPid;
            break;
        }
    }
    if (myPid == node[0])
    {
        closePipeExcept(0);
    }
    return;
}

//개행까지 출력하는 함수 파일의 끝에서 true를 출력
bool printOneLine(int32_t fd)
{
    char oneChar;
    ssize_t isEof = 1;
    printf("%d ", getpid());
    while (1)
    {
        isEof = read(fd, &oneChar, 1);
        if (isEof == 0)
        {
            printf("\n");
            return true; //eof
        }
        else
        {
            printf("%c", oneChar);
            if (oneChar == '\n')
                return false; //개행
        }
    }
}

//start 프로세스(노드)에서 end 프로세스(노드)로 가는 신호를 받는 함수(end)프로세스에서 작동
ssize_t rcvSignal(int start, int end)
{
    signalType signal;
    read(graph[start][end][READ_PIPE], &signal, sizeof(signal));
    return signal;
}

//start 프로세스(노드) 에서 end 프로세스(노드)로 신호를 보내는 함수(start)프로세스에서 작동
ssize_t sendSignal(int start, int end, signalType signal)
{
    return write(graph[start][end][WRITE_PIPE], &signal, sizeof(signal));
}

//노드1과 노드2 사이에 파이프 2개를 여는 함수
void openPipeBetween(int node1, int node2)
{
    pipe(graph[node1][node2]);
    pipe(graph[node2][node1]);
}

//node에 해당하는 파이프를 제외하고 다 닫는 함수
void closePipeExcept(int node)
{
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            if (i == node || j == node)
            {
                if (i == node)
                {
                    close(graph[i][j][0]);
                    graph[i][j][0] = -1;
                }
                if (j == node)
                {
                    close(graph[i][j][1]);
                    graph[i][j][1] = -1;
                }
            }
            else
            {
                close(graph[i][j][0]);
                close(graph[i][j][1]);
                graph[i][j][0] = -1;
                graph[i][j][1] = -1;
            }
            // printf("node %d: graph[%d][%d] : { %d ,%d }\n", node, i, j[i][j][0][i][j][1]);
        }
    }
}

//열려있는 모든 파이프를 닫는 함수
void closePipeAll()
{
    for (int i = 0; i < MAX_NODE; i++)
    {
        for (int j = 0; j < MAX_NODE; j++)
        {
            close(graph[i][j][0]);
            close(graph[i][j][1]);
        }
    }
}

//nowNode에서 일어나는 동작
void inNode(int nowNode, int32_t fd)
{
    if (node[nowNode] == getpid())
    {
        signalType signal;
        bool iKnow = false;
        bool direction = true;
        bool meetEOF = false;
        bool isInitial = false;
        int nextNode = nowNode + 1;
        int prevNode = nowNode - 1;
        if (nextNode == MAX_NODE)
            nextNode = 0;
        if (prevNode == -1)
            prevNode = MAX_NODE - 1;
        if (nowNode == 0) // 시작노드(메시지 전송을 처음에 하는 노드)  경우
            isInitial = true;//처음이라고 알려줌
        if (isInitial) //처음일 경우 (1번만 실행)
        {
            meetEOF = printOneLine(fd);
            if (meetEOF)
            {
                iKnow = true;
                printf("%d read all data\n", node[nowNode]);
                sendSignal(nowNode, nextNode, FINISHED_JOB);
            }
            else
                sendSignal(nowNode, nextNode, YOUR_TURN);
            isInitial = false;
        }

        while (1)
        {
            if (direction)
                signal = rcvSignal(prevNode, nowNode);
            else
                signal = rcvSignal(nextNode, nowNode);
            switch (signal)
            {
            case YOUR_TURN:
                meetEOF = printOneLine(fd);
                if (meetEOF)
                {
                    iKnow = true;
                    printf("%d read all data\n", node[nowNode]);
                    sendSignal(nowNode, nextNode, FINISHED_JOB);
                }
                else
                    sendSignal(nowNode, nextNode, YOUR_TURN);
                break;
            case FINISHED_JOB:
                if (iKnow)
                {
                    direction = false;
                    sendSignal(nowNode, nextNode, SHOUD_EXIT);
                }
                else
                {
                    sendSignal(nowNode, nextNode, FINISHED_JOB);
                }
                break;
            case SHOUD_EXIT:
                
                if (nowNode == MAX_NODE - 1)
                {
                    printf("%d I'm exiting...\n", node[nowNode]);
                    sendSignal(nowNode, prevNode, GOOD_BYE);
                    closePipeAll();
                    return ;
                }
                else if(nextNode !=0)
                {
                    direction = false;
                    sendSignal(nowNode, nextNode, SHOUD_EXIT);
                }
                break;
            case GOOD_BYE:
                printf("%d I'm exiting...\n", node[nowNode]);
                sendSignal(nowNode, prevNode, GOOD_BYE);
                closePipeAll();
                return;
            default:
                break;
            }
        }
    }
    else
        return;
}
