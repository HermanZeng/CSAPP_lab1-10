/*
*name: Zeng Heming
*ID: 5140219405
*/
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int valid;
	int tag;
	int access;
} line;


int getSetBits(long int addr, int s, int b);
int getTagBits(long int addr, int s, int b);
void initial(line** cache, int set_num, int line_num);
void update(line** cache, int row, int col, int e);
void place(long int address, line** cache, char opt, int s, int e, int b, int* hit, int* miss, int* eviction);


int main(int argc, char* argv[])
{
	int s, e, b;
	char path[30];
	for (int i = 0; i < argc; i++){
		if (strcmp(argv[i], "-s") == 0) s = strtol(argv[i + 1], NULL, 10);
		if (strcmp(argv[i], "-E") == 0) e = strtol(argv[i + 1], NULL, 10);
		if (strcmp(argv[i], "-b") == 0) b = strtol(argv[i + 1], NULL, 10);
		if (strcmp(argv[i], "-t") == 0) strcpy(path,  argv[i + 1]);
	}
	
    line** cache = NULL;
	int set_num = 0x1 << s;
	int line_num = 0x1 << e;
	cache = (line**)malloc(sizeof(line*) * set_num);
	for (int i = 0; i < set_num; i++)
		cache[i] = (line*)malloc(sizeof(line) * line_num);
	for (int i = 0; i < set_num; i++)
		for (int j = 0; j < line_num; j++)
		{
			//printf("pass\n");
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].access = 0;
		}
    
    int hit = 0;
	int miss = 0;
	int eviction = 0;
    
	FILE* input = fopen(path, "r");
	while (!feof(input)){
		char temp = fgetc(input);
		if (temp == 'I')
		{
			char buffer[30];
			fgets(buffer, 30, input);
			continue;
		}
		else
		{
			if (feof(input)) break;
			char opt = fgetc(input);
			temp = fgetc(input);
			//temp = fgetc(input);
			char address[15], size[5];
			int i = 0;
			char ch;
			while ((ch = fgetc(input)) != ',')
			{
				address[i] = ch;
				i++;
			}
			address[i] = '\0';
			i = 0;
			while (1)
			{
				if (feof(input)) { i++;  break; }
				if ((ch = fgetc(input)) == '\n') { i++; break; }
				size[i] = ch;
				i++;
			}
			size[i] = '\0';
			//address[i] = '\0';
			//printf("%s", address);
			long int add = strtol(address, NULL, 16);
			//int s = strtol(size, NULL, 10);
            printf("%s\n", size);
            place(add, cache, opt, s, e, b, &hit, &miss, &eviction);
			printf("%s ", address);
			printf("%ld ", add);
            printf("%d %d %d\n", hit, miss, eviction);
			//printf("%d\n", s);
			//if (feof(input)) { printf("end"); break; }
		}
	}
    
    printSummary(hit, miss, eviction);
	//system("pause");
	return 0;
}

int getSetBits(long int addr, int s, int b)
{
	int mask;
	mask = 0x7fffffff >> (31 - s);
	addr = addr >> b;
	return (mask & addr);
}

int getTagBits(long int addr, int s, int b)
{
	int mask;
	mask = 0x7fffffff >> (31 - s - b);
	addr = addr >> (s + b);
	return (mask & addr);
}

void initial(line** cache, int set_num, int line_num)
{
	cache = (line**)malloc(sizeof(line*) * set_num);
	for (int i = 0; i < set_num; i++)
		cache[i] = (line*)malloc(sizeof(line) * line_num);
	for (int i = 0; i < set_num; i++)
		for (int j = 0; j < line_num; j++)
		{
			//printf("pass\n");
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].access = 0;
		}
	//printf("%d %d %d\n", cache[0][0].valid, cache[0][0].tag, cache[0][0].access);
	return;
}

void place(long int address, line** cache, char opt, int s, int e, int b, int* hit, int* miss, int* eviction)
{
	int set_index = getSetBits(address, s, b);
	printf("%d\n", set_index);
	int tag = getTagBits(address, s, b);
	printf("%d\n", set_index);
	int i;
	for (i = 0; i < e; i++)
	{
		//printf("pass\n");
		if (cache[set_index][i].valid == 1 && cache[set_index][i].tag == tag)
		{
			if (opt == 'M') *hit += 2;
			else *hit += 1;
			update(cache, set_index, i, e);
			return;
		}
	}
	printf("miss\n");
	*miss += 1;
	for (i = 0; i < e; i++)
	{
		if (cache[set_index][i].valid == 0)
		{
			cache[set_index][i].valid = 1;
			cache[set_index][i].tag = tag;
			update(cache, set_index, i, e);
			if (opt == 'M') *hit += 1;
			return;
		}
	}
	*eviction += 1;
	for (i = 0; i < e; i++)
	{
		if (cache[set_index][i].access == 1)
		{
			cache[set_index][i].valid = 1;
			cache[set_index][i].tag = tag;
			update(cache, set_index, i, e);
			if (opt == 'M') *hit += 1;
			return;
		}
	}
	return;
}

void update(line** cache, int row, int col, int e)
{
	for (int i = 0; i < e; i++)
	{
		if (cache[row][col].valid == 1 && cache[row][i].access > cache[row][col].access)
			cache[row][i].access -= 1;
	}
	cache[row][col].access = e;
}

