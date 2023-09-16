#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		printf("cannot open file\n");
		// fprintf(stderr, "%s: %s\n", fp, strerror);
		exit(1);
	}

	char *buf;
	size_t size;
	buf = (char *)malloc(size);
	int counter = 0;
	char **fortune = malloc(sizeof(int) * 3);
	
	for (int i = 0; i < 3; i++)
        {
                fortune[i] = (char *)malloc(sizeof(char) * 100);
	}
	
	int i = 0;
	char cur[100] = "";
	char blank[1] = "";
	char *delim = "%\n";
	int numFortune = 1;
	while (getline(&buf, &size, fp) != EOF)
	{
		if(counter < 3){
			counter ++;
			continue;	
		}
		if(strcmp(buf, delim) == 0){
			cur[strlen(cur) - 1] = '\0';
			strcpy(fortune[numFortune - 1], cur);
			numFortune ++;
			strcpy(cur, blank);
			continue;
		}	
		strcat(cur, buf);
	}
	cur[strlen(cur) - 1] = '\0';
	strcpy(fortune[numFortune - 1], cur);
	printf("%s", fortune[2]);
}
