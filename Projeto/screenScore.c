#include "raylib.h"
#include <stdlib.h>     
#include <string.h>     
#include <stdio.h>

typedef struct score {
    char name[3];
    char point[15];
} Score;

void ReadScore (FILE *fptr, Score *scorePool) {
    char temp[100];
    
    if ((fptr = fopen ("resources/Text/teste.txt","r")) == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    fscanf(fptr,"%s", &temp);
    fclose(fptr);
    int init_size = strlen(temp);
    char *ptr = strtok(temp, ">");
    int i = 0;
    while(ptr != NULL)
    {
        printf("%s\n", ptr);
        Score *curScore = scorePool + i;
        if (i % 2 == 0)
            strcpy(curScore->name, ptr);
        else
            strcpy(curScore->point, ptr);
            //curScore->point = atoi(ptr);
        i++;
        ptr = strtok(NULL, ">");
    }

}


void WriteScore(FILE *fptr, Score *scorePool) {
    char temp[100];
    char delim[] = ">";
    strcpy(temp, "");

    fptr = fopen ("resources/Text/teste.txt","w");
    if(fptr == NULL) {
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


int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - mouse input");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    FILE *fptr;
    char data[100];
    char *name;
    int choice = 0;
    Score *scorePool = (Score *)malloc(10*sizeof(Score));

    strcpy(scorePool[0].name,"ABC");
    strcpy(scorePool[1].point,"123");
    strcpy(scorePool[2].name,"DEF");
    strcpy(scorePool[3].point,"234");
    strcpy(scorePool[4].name,"GHI");
    strcpy(scorePool[5].point,"345");
    strcpy(scorePool[6].name,"JKL");
    strcpy(scorePool[7].point,"456");
    strcpy(scorePool[8].name,"MNO");
    strcpy(scorePool[9].point,"567");

    WriteScore(fptr, scorePool);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        ReadScore (fptr, scorePool);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            // Imprimir ranking
            
            int pos = 0;
            for (int i = 0; i < 10; i+=2) {
                pos++;
                DrawText(TextFormat("%01d", pos), 0, pos*25, 20, RED);
                DrawText(scorePool[i].name, 23, pos*25, 20, BLUE);
                DrawText(scorePool[i+1].point, 20 + 100, pos*25, 20, BLUE);
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
