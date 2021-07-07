#include <stdlib.h>     
#include <string.h>     
#include <stdio.h>
#include <sys/stat.h>

typedef struct score {
    char name[10];
    char point[15];
} Score;

int CheckIfFileExists(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else  
        return 0;
}

void ReadScore (FILE *fptr, char* file, Score *scorePool) {
    char temp[100];
    
    if ((fptr = fopen (file,"r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    fscanf(fptr,"%s", &temp);
    fclose(fptr);
    int init_size = strlen(temp);
    char *ptr = strtok(temp, ">");
    int i = 0;
    while(ptr != NULL) {
        printf("%s\n", ptr);
        Score *curScore = scorePool + i;
        if (i % 2 == 0)
            strcpy(curScore->name, ptr);
        else
            strcpy(curScore->point, ptr);
        i++;
        ptr = strtok(NULL, ">");
    }

}

void WriteScore(FILE *fptr, char* file, Score *scorePool) {
    char temp[100];
    char delim[] = ">";
    strcpy(temp, "");

    if ((fptr = fopen (file,"w")) == NULL) {
        printf("Error!");   
        exit(1);             
    }

    for (int i = 0; i < 10; i+=2) {
        strcat(temp, scorePool[i].name);
        strcat(temp, delim);
        strcat(temp, scorePool[i+1].point);
        strcat(temp, delim);
    }

    fprintf(fptr,"%s",temp);
    fclose(fptr);
}

void UpdateScores(Score *scorePool, char* name, long* points) {
    char pointHolder[100];
    char nameHolder[100];
    char pointHolder2[100];
    char nameHolder2[100];
    char strPoints[15];
    sprintf(strPoints, "%ld", points); //strPoints com a pontuação em string

    
    for (int i = 0; i < 10; i+=2) {
        if (atoi(scorePool[i+1].point) < points) {
            //Início da permuta
            strcpy(nameHolder, scorePool[i].name);
            strcpy(pointHolder, scorePool[i+1].point);
            strcpy(scorePool[i].name, name);
            strcpy(scorePool[i+1].point, strPoints);

            for (int j = i+2; j < 10; j+=2) {
                strcpy(nameHolder2, scorePool[j].name);
                strcpy(pointHolder2, scorePool[j+1].point);

                strcpy(scorePool[j].name, nameHolder);
                strcpy(scorePool[j+1].point, pointHolder);

                strcpy(nameHolder, nameHolder2);
                strcpy(pointHolder, pointHolder2);
                
            }
            return;
        }
    }
}