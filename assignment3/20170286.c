#include "20170286.h"

int main(int32_t argc, char *argv[])
{

    int32_t fd;
    memset(graph, -1, sizeof(graph));//각 전역변수의 초기화
    memset(node, -1, sizeof(node));//각 전역변수의 초기화
    if (argc != 2)
    {
        printf("Usage: 20170286 <file>\n");
        return 0;
    }
    fd = open(argv[1], O_RDWR | O_SYNC); //파일디스크립터가 읽고 쓰기 모두 가능하게 함
    if (fd < 0)
    {
        printf("Error  Unable %d to open %s\n", fd, argv[1]);
        return 0;
    }
    initProgram();//파이프를 열고 닫고 포크하기
    inNode(0, fd);//0번 노드에서 일어나는 일
    inNode(1, fd);//1번 노드에서 일어나는 일
    inNode(2, fd);//2번 노드에서 일어나는 일
    inNode(3, fd);//3번 노드에서 일어나는 일
    inNode(4, fd);//4번 노드에서 일어나는 일
    return 0;
}
