#include <stdio.h>
#include <stdlib.h>
#include "20170286.h"
#define START_AT_END 1
#define START_AT_SET 0
int main()
{
	for(int i= SIGNED_CHAR;i<=DOUBLE;i++)//sigend_char ~ double 까지 연속으로 정의된 매크로
		readFile("input", i);
	return 0;
}
void readFile(char *filename, int flag) //파일을 flag에 맞게 읽는다
{
	FILE *fp;
	int fileSize;
	void *memorySpace;
	char tmpStr[64];
	fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp) / BYTE;
	// printf("파일의  크기는 %ld 비트입니다.\n", ftell(fp)); 
	// printf("파일의 크기는 %d byte입니다\n", fileSize);
	if(ftell(fp)%64!=0){
		printf("파일의 크기가 64의 배수가 아닙니다.\n");
		exit(1);
	}
	memorySpace = calloc(1, fileSize);
	fseek(fp, 0, SEEK_SET);
	int i = 0;
	while (!feof(fp))
	{
		switch (flag)
		{
		case SIGNED_CHAR:
			fread(tmpStr, 1 * BYTE, 1, fp);
			*((unsigned char *)memorySpace + i) = (unsigned char)stringToBit(tmpStr, 1 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		case ASCII_CODES:
			fread(tmpStr, 1 * BYTE, 1, fp);
			*((unsigned char *)memorySpace + i) = (unsigned char)stringToBit(tmpStr, 1 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		case UNSIGNED_CHAR:
			fread(tmpStr, 1 * BYTE, 1, fp);
			*((unsigned char *)memorySpace + i) = (unsigned char)stringToBit(tmpStr, 1 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		case SIGNED_INT:
			fread(tmpStr, 4 * BYTE, 1, fp);
			*((signed int *)memorySpace + i) = (unsigned int)stringToBit(tmpStr, 4 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		case UNSIGNED_INT:

			fread(tmpStr, 4 * BYTE, 1, fp);
			*((unsigned int *)memorySpace + i) = (unsigned int)stringToBit(tmpStr, 4 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		case FLOAT:

			fread(tmpStr, 4 * BYTE, 1, fp);
			*((unsigned int *)memorySpace + i) = (unsigned int)stringToBit(tmpStr, 4 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		case DOUBLE:

			fread(tmpStr, 8 * BYTE, 1, fp);
			*((uint64_t *)memorySpace + i) = (uint64_t)stringToBit(tmpStr, 8 * BYTE);//바꾼 비트가 변형되지 않게 각 변수형의 크기에 맞는 바이트의 부호가 없는 정수로 받음
			break;
		default:
			break;
		}
		i++;
	}
	printResult(memorySpace, fileSize, flag);//메모리 공간에 있는것을 플래그에 맞게 읽는 함수
	fclose(fp);
	free(memorySpace);
}
void printResult(void *memory, int memSize, int flag) // 비트에 저장되어 있는 것을 각 형식에 맞게 출력하는 함수 flag로 어떤 출력을 할것인지 판별
{
	void *mMem = memory;
	switch (flag)
	{
	case SIGNED_CHAR:
		printf("signed char:\n	");
		for (int i = 0; i < memSize / sizeof(char); i++)
		{
			printf("%d ", *(char *)(mMem + i));
		}
		printf("\n");
		break;
	case ASCII_CODES:
		printf("ASCII codes:\n	");
		for (int i = 0; i < memSize / sizeof(char); i++)
		{
			if (*((char *)mMem + i) < 0)
				printf(". ");
			else
				printf("%c ", *((char *)mMem + i));
		}
		printf("\n");
		break;
	case UNSIGNED_CHAR:
		printf("unsigned char:\n	");
		for (int i = 0; i < memSize / sizeof(unsigned char); i++)
		{
			printf("%d ", *((unsigned char *)mMem + i));
		}
		printf("\n");
		break;
	case SIGNED_INT:
		printf("signed int:\n	");
		for (int i = 0; i < memSize / sizeof(signed int); i++)
		{
			printf("%d ", *((int *)mMem + i));
		}
		printf("\n");
		break;
	case UNSIGNED_INT:
		printf("unsigned int:\n	");
		for (int i = 0; i < memSize / sizeof(unsigned int); i++)
		{
			printf("%u ", *((unsigned int *)mMem + i));
		}
		printf("\n");
		break;
	case FLOAT:
		printf("float:\n	");
		for (int i = 0; i < memSize / sizeof(float); i++)
		{
			printf("%.4f \n	", *((float *)mMem + i));
		}
		printf("\n");
		break;
	case DOUBLE:
		printf("double:\n	");
		for (int i = 0; i < memSize / sizeof(double); i++)
		{
			printf("%.4lf \n	", *((double *)mMem + i));
		}
		printf("\n");
		break;
	default:
		break;
	}
}
uint64_t stringToBit(char *str, int size) //0과1로이루어진 문자열을 비트로 바꿈
{
	// unsigned char res = 0x0;  //결과 비트를 저장할 곳
	// unsigned char msk = 0x01; //0000 0001 마스크
	uint64_t res = 0x0;
	uint64_t msk = 0x01;
	for (int i = 0; i < size; i++)
	{
		res = res + (unsigned char)(str[i] & msk); //0은 아스키코드 짝수니까 최하위 1비트는 0이고 1은 홀수니까 최하위 비트는 1이다  res에 해당 str의 i번째 비트를 더한다)
		if (i == size - 1)						   //마지막 루프에서 마지막 비트를 더해준 후 비트가 밀리는 것을 방지하고 반복문을 탈출한다
		{
			break;
		}
		else //마지막 루프가 아니면 더해진 결과를 왼쪽으로 민다
		{
			res = res << 1;
		}
	}
	//printf("res는 %d입니다\n", res);
	return res;
}
