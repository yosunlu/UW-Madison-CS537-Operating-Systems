#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
        char usageError[200] = "USAGE: \n\tbadger-fortune -f <file> -n <number> (optionally: -o <output file>) \n\t\t OR \n\tbadger-fortune -f <file> -b <batch file> (optionally: -o <output file>)\n";
        char flagTypeError[30] = "ERROR: Invalid Flag Types\n";
        char doubleUsageError1[100] = "ERROR: You can't specify a specific fortune number in conjunction with batch mode\n";
        char doubleUsageError2[100] = "ERROR: You can't use batch mode when specifying a fortune number using -n\n";

        // check if argc < 5
        if (argc < 5)
        {
                fprintf(stdout, "%s", usageError);
                exit(1);
        }

        // check if flags are correct

        int fFlag = 0;
        int fNum = 0;
        int nFlag = 0;
        int nNum = 0;
        int bFlag = 0;
        int bNum = 0;
        int oFlag = 0;
        int oNum = 0;

        for (int i = 1; i < argc; i++)
        {
                if (strcmp(argv[i], "-f") == 0)
                {
                        fNum = i;
                        fFlag = 1;
                }
                else if (strcmp(argv[i], "-n") == 0)
                {
                        if (bFlag == 1)
                        {
                                fprintf(stdout, "%s", doubleUsageError1);
                                exit(1);
                        }
                        nNum = i;
                        nFlag = 1;
                }
                else if (strcmp(argv[i], "-b") == 0)
                {
                        if (nFlag == 1)
                        {
                                fprintf(stdout, "%s", doubleUsageError2);
                                exit(1);
                        }
                        bNum = i;
                        bFlag = 1;
                }
                else if (strcmp(argv[i], "-o") == 0)
                {
                        oNum = i;
                        oFlag = 1;
                }
        }

        if (nFlag == 0 && bFlag == 0)
        {
                fprintf(stdout, "%s", flagTypeError);
                exit(1);
        }

        if (nFlag == 1 && fFlag == 0 && bFlag == 0 && oFlag == 0)
        {
                fprintf(stdout, "%s", flagTypeError);
                exit(1);
        }

        if (fFlag == 0 && (nFlag == 1 || bFlag == 1))
        {
                fprintf(stdout, "%s", "ERROR: No fortune file was provided\n");
                exit(1);
        }

        // open the file and check if successfully opened

        FILE *fp = fopen(argv[fNum + 1], "r");
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
        if (nFlag == 1 && argc == 5)
        {
                fortuneIndex = atoi(argv[nNum + 1]); // parse the index
                if (fortuneIndex < 0 || fortuneIndex > num[0])
                {
                        fprintf(stdout, "%s", "ERROR: Invalid Fortune Number\n");
                        exit(1);
                }
                printf("%s", fortune[fortuneIndex - 1]);
        }
        // batch mode
        else if (bFlag == 1 && argc == 5)
        {
                // open the batch file and check if successfully opened
                FILE *fp = fopen(argv[bNum + 1], "r");
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

                if (fclose(fp) != 0)
                {
                        printf("Error while closing the file.\n");
                        exit(1);
                }
        }

        else if (argc > 5)
        {
                // output mode
                if (oFlag == 1)
                {
                        FILE *newFile = fopen(argv[oNum + 1], "w");
                        if (newFile == NULL)
                        {
                                printf("cannot open file\n");
                                return 1;
                        }
                        // number mode
                        if (nFlag == 1)
                        {
                                fortuneIndex = atoi(argv[nNum + 1]);
                                fputs(fortune[fortuneIndex - 1], newFile);
                                if (fclose(newFile) != 0)
                                {
                                        printf("Error while closing the file.\n");
                                        exit(0);
                                }
                        }

                        // batch mode
                        else if (bFlag == 1)
                        {
                                FILE *fp = fopen(argv[bNum + 1], "r");
                                if (fp == NULL)
                                {
                                        printf("cannot open file\n");
                                        return 1;
                                }
                                int batchCounter = 0;
                                while (getline(&buf, &size, fp) != EOF)
                                {
                                        fortuneIndex = atoi(buf);
                                        if (fortuneIndex <= 0 || fortuneIndex > num[0])
                                        {
                                                fputs("ERROR: Invalid Fortune Number\n\n", newFile);
                                                continue;
                                        }

                                        if (strcmp(&buf[strlen(buf) - 1], "\n") == 0)
                                        { // check if last line
                                                fputs(fortune[fortuneIndex - 1], newFile);
                                                fputs("\n\n", newFile);
                                        }

                                        else
                                        {
                                                fputs(fortune[fortuneIndex - 1], newFile);
                                        }
                                        batchCounter++;
                                }
                                // check if the batch file is empty
                                if (batchCounter == 0)
                                {
                                        fprintf(stdout, "%s", "ERROR: Batch File Empty\n");
                                        exit(1);
                                }

                                if (fclose(newFile) != 0)
                                {
                                        printf("Error while closing the file.\n");
                                        exit(0);
                                }
                        }
                }
        }

        if (fclose(fp) != 0)
        {
                printf("Error while closing the file.\n");
                exit(1);
        }

        
        for (int i = 0; i < num[0]; i++)
        {
                free(fortune[i]);
        }
        free(fortune);
        free(buf);
}
