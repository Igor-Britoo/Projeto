#include <stdlib.h>     
#include <string.h>     
#include <stdio.h>

typedef struct score {
    char name[3];
    int point;
} Score;

int main(){
    FILE *fptr;
    Score scorePool[10];
    char data[100];
    char *name;
    int choice = 0;


    printf("(1) Para escrever ou (2) Para ler: ");
    scanf("%d",&choice);

    if (choice == 1) {
    fptr = fopen ("resources/Text/teste.txt","w");
        if(fptr == NULL)
        {
            printf("Error!");   
            exit(1);             
        }

        printf("Enter num: ");
        scanf("%s",&data);

        fprintf(fptr,"%s",data);
        fclose(fptr);
    }

    if (choice == 2) {
        if ((fptr = fopen ("resources/Text/teste.txt","r")) == NULL){
            printf("Error! opening file");
            exit(1);
        }

        fscanf(fptr,"%s", &data);

        fclose(fptr);

        int init_size = strlen(data);

        char *ptr = strtok(data, ">");
        int i = 0;
        while(ptr != NULL)
        {
            //printf("%s\n", ptr);
            Score *curScore = scorePool + i;
            if (i % 2 == 0)
                strcpy(curScore->name, ptr);
            //else
                curScore->point = atoi(ptr);
            i++;
            ptr = strtok(NULL, ">");
        }

        for (int i = 0; i < 10; i++) {
            printf("%s\n", scorePool[i].name);
            printf("%d\n", scorePool[i].point);
        }
        
    }
   

    return 0;
}