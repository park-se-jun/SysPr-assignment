#include "20170286.h"

int main(int32_t argc, char *argv[])
{

    int32_t fd;
    memset(graph, -1, sizeof(graph));
    memset(node, -1, sizeof(node));
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
    initProgram();
    inNode(0, fd);
    inNode(1, fd);
    inNode(2, fd);
    inNode(3, fd);
    inNode(4, fd);
    return 0;
}
