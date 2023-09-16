#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
        char usageError[200] = "USAGE: \n\tbadger-fortune -f <file> -n <number> (optionally: -o <output file>) \n\t\t OR \n\tbadger-fortune -f <file> -b <batch file> (optionally: -o <output file>)\n";
        char flagTypeError[30] = "ERROR: Invalid Flag Types\n";
        char doubleUsageError1[100] = "ERROR: You can't use batch mode when specifying a fortune number using -n\n";
        char doubleUsageError2[100] = "ERROR: You can't specify a specific fortune number in conjunction with batch mode\n";

        // check if argc < 5
        if (argc < 5 )
        {
                fprintf(stdout, "%s", usageError);
                exit(1);
        }

        // check if flags are correct
       
//	if(argc == 5 && (strcmp(argv[1], "-n") == 0)){

//		fprintf(stdout, "%s", "ERROR: No fortune file was provided\n");
//              exit(1);
//	}	

	int fFlag = strcmp(argv[1], "-f");
        int modeFlagN = strcmp(argv[3], "-n");
        int modeFlagB = strcmp(argv[3], "-b");
        int modeFlagO = 0;
        if (argc > 5)
        {
                modeFlagO = strcmp(argv[5], "-o");
        }

        if (fFlag != 0)
        {
		if(strcmp(argv[1], "-n") == 0){
			fprintf(stdout, "%s", "ERROR: No fortune file was provided\n");
                	exit(1);
		}
		fprintf(stdout, "%s", flagTypeError);
                exit(1);
        }

        if (modeFlagB != 0)
        {
                if (modeFlagN != 0)
                {
                        fprintf(stdout, "%s", flagTypeError);
                        exit(1);
                }
        }
       
//      	if(strstr(argv[2], ".txt") == NULL){
//              fprintf(stdout, "%s", "ERROR: No fortune file was provided\n");
//	            exit(1);
//	} 
       
	// check -n then -b and vice versa
        if (argc > 5)
        {
                if ((modeFlagN == 0 && strcmp(argv[5], "-b") == 0))
                {
                        fprintf(stdout, "%s", doubleUsageError1);
                        exit(1);
                }
                else if ((modeFlagB == 0 && strcmp(argv[5], "-n") == 0))
                {
                        fprintf(stdout, "%s", doubleUsageError2);
                        exit(1);
                }
        }

        if (modeFlagO != 0)
        {
                fprintf(stdout, "%s", flagTypeError);
                exit(1);
        }

        // open the file and check if successfully opened

        FILE *fp = fopen(argv[2], "r");
        if (fp == NULL)
        {
                fprintf(stdout, "%s", "ERROR: Can't open fortune file\n");
                exit(1);
        }

        // store the number of fortunes and the max length of the fortune
        int num[2];
        size_t size;
        char *buf = NULL;
        int i = 0;
        int fortuneCounter = 0;

        while (i < 2)
        {
                if (getline(&buf, &size, fp) == EOF)
                {
                        break;
                }
                num[i] = atoi(buf);
                i++;
                fortuneCounter++;
        }
        if (fortuneCounter == 0)
        {
                fprintf(stdout, "%s", "ERROR: Fortune File Empty\n");
                exit(1);
        }

        // allocate space for a 2D array
        char **fortune = malloc(sizeof(char *) * num[0]);
        if (fortune == NULL)
        {
                printf("array not allocated");
                exit(1);
        }
        for (int i = 0; i < num[0]; i++)
        {
                fortune[i] = (char *)malloc(sizeof(char) * num[1]);
                if (fortune[i] == NULL)
                {
                        printf("array not allocated");
                        return 1;
                }
        }

        char *delim = "%\n";
        int numFortune = 0;
        int counter = 0;
        while (getline(&buf, &size, fp) != EOF)
        {
                if (counter < 1)
                { // skip the first line
                        counter++;
                        continue;
                }
                if (strcmp(buf, delim) == 0)
                {
                        numFortune++;
                        continue;
                }
                strcat(fortune[numFortune], buf);
        }

        // number mode
        int fortuneIndex = 0;
        if (strcmp(argv[3], "-n") == 0 && argc == 5 )
        {
                fortuneIndex = atoi(argv[4]); // parse the index
                if (fortuneIndex < 0 || fortuneIndex > num[0])
                {
                        fprintf(stdout, "%s", "ERROR: Invalid Fortune Number\n");
                        exit(1);
                }
                printf("%s", fortune[fortuneIndex - 1]);
        }
        // batch mode
        else if (strcmp(argv[3], "-b") == 0 && argc == 5 )
        {
                // open the batch file and check if successfully opened
                FILE *fp = fopen(argv[4], "r");
                if (fp == NULL)
                {
                        fprintf(stdout, "%s", "ERROR: Can't open batch file\n");
                        exit(1);
                }

                int batchCounter = 0;
                while (getline(&buf, &size, fp) != EOF)
                {
                        fortuneIndex = atoi(buf);
                        if (fortuneIndex < 0 || fortuneIndex > num[0])
                        {
                                fprintf(stdout, "%s", "ERROR: Invalid Fortune Number\n\n");
                                continue;
                        }
                        if (strcmp(&buf[strlen(buf) - 1], "\n") == 0)
                        { // check if last line
                                printf("%s\n\n", fortune[fortuneIndex - 1]);
                        }
                        else
                        {
                              	int len = strlen(fortune[fortuneIndex - 1]);
				fortune[fortuneIndex - 1][len - 2] = '\0';
			      	printf("%s", fortune[fortuneIndex - 1]);
                        }
                        batchCounter++;
                }
                // check if the batch file is empty
                if (batchCounter == 0)
                {
                        fprintf(stdout, "%s", "ERROR: Batch File Empty\n");
			exit(1);
                }
        }

        if (argc > 5)
        {
                // output mode
                if (strcmp(argv[5], "-o") == 0)
                {
                        FILE *newFile = fopen(argv[6], "w");
                        if (newFile == NULL)
                        {
                                printf("cannot open file\n");
                                return 1;
                        }
                        // number mode
                        if (strcmp(argv[3], "-n") == 0)
                        {
                                fortuneIndex = atoi(argv[4]);
                                fputs(fortune[fortuneIndex - 1], newFile);
                        }

                        // batch mode
                        else if (strcmp(argv[3], "-b") == 0)
                        {
                                FILE *fp = fopen(argv[4], "r");
                                if (fp == NULL)
                                {
                                        printf("cannot open file\n");
                                        return 1;
                                }

                                while (getline(&buf, &size, fp) != EOF)
                                {
                                        fortuneIndex = atoi(buf);
                                        if (strcmp(&buf[strlen(buf) - 1], "\n") == 0)
                                        { // check if last line
                                                fputs(fortune[fortuneIndex - 1], newFile);
                                                fputs("\n\n", newFile);
                                        }
                                        else
                                        {
                                                fputs(fortune[fortuneIndex - 1], newFile);
                                        }
                                }
                        }
                }
        }
}
