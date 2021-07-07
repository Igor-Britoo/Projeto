#include "raylib/raylib.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define MAX_INPUT_CHARS 3
#define screenWidth 1280
#define screenHeight 720

typedef enum {RECEIVE_NAME, RANKING}Screen;

typedef struct Ranking_information{
    char name[4];
    int points;
}Ranking_information;

void Save_name(char received_name[MAX_INPUT_CHARS+1], int letterCount, int framesCounter){
    
    DrawText(received_name, (screenWidth/2 - 100) + 5, screenHeight/2 + 8, 80, WHITE);
    DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, MAX_INPUT_CHARS), 315, 250, 20, DARKGRAY);

    if (letterCount < MAX_INPUT_CHARS)
    {
    // Draw blinking underscore char
        if (((framesCounter/20)%2) == 0){
            DrawText("_", (screenWidth/2 - 100) + 8 + MeasureText(received_name, 80), screenHeight/2 + 12, 80, WHITE);
        }
    }
    else DrawText("Press ENTER to confirm", 230, 330, 20, GRAY);
    
    if(letterCount > 0)DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
}

void Update_ranking(Ranking_information *ranking){
    for(int i=0;i<5;i++){
        int x;
        char y[4];

        for(int a=0;a<5;a++){
            if(a!=i && ranking[a].points<ranking[i].points){
                x = ranking[i].points;
                ranking[i].points = ranking[a].points;
                ranking[a].points = x;
                strcpy(y, ranking[i].name);
                strcpy(ranking[i].name, ranking[a].name);
                strcpy(ranking[a].name, y);
            }
        }
    }
}

void Draw_score_screen(Ranking_information *ranking ){

    DrawText("RANKING",screenWidth/2 - MeasureText("RANKING", 70)/2, 60, 70, WHITE);

    DrawText("1st", screenWidth/5 - MeasureText("1st", 60)/2 + 20, 180, 60, YELLOW);
    DrawText("2nd", screenWidth/5 - MeasureText("2nd", 60)/2 + 20, 250, 60, YELLOW);
    DrawText("3rd", screenWidth/5 - MeasureText("3rd", 60)/2 + 20, 320, 60, YELLOW);
    DrawText("4th", screenWidth/5 - MeasureText("4th", 60)/2 + 20, 390, 60, YELLOW);
    DrawText("5th", screenWidth/5 - MeasureText("5th", 60)/2 + 20, 460, 60, YELLOW);

    DrawText(ranking[0].name,screenWidth/2 - MeasureText(ranking[0].name, 60)/2, 180, 60, WHITE);
    DrawText(ranking[1].name,screenWidth/2 - MeasureText(ranking[1].name, 60)/2, 250, 60, WHITE);
    DrawText(ranking[2].name,screenWidth/2 - MeasureText(ranking[2].name, 60)/2, 320, 60, WHITE);
    DrawText(ranking[3].name,screenWidth/2 - MeasureText(ranking[3].name, 60)/2, 390, 60, WHITE);
    DrawText(ranking[4].name,screenWidth/2 - MeasureText(ranking[4].name, 60)/2, 460, 60, WHITE);

    for(int i=0; i<5;i++){
        if(ranking[i].points==-1)ranking[i].points=0;
    }

    DrawText(TextFormat("%i", ranking[0].points), screenWidth/3 *2 , 180, 60, WHITE);
    DrawText(TextFormat("%i", ranking[1].points), screenWidth/3 *2 , 250, 60, WHITE);
    DrawText(TextFormat("%i", ranking[2].points), screenWidth/3 *2 , 320, 60, WHITE);
    DrawText(TextFormat("%i", ranking[3].points), screenWidth/3 *2 , 390, 60, WHITE);
    DrawText(TextFormat("%i", ranking[4].points), screenWidth/3 *2 , 460, 60, WHITE);

}

int main(void)
{
    Ranking_information *ranking = NULL;
    ranking = (Ranking_information*) malloc(5*sizeof(Ranking_information));

    for(int i=0; i<5;i++){
        ranking[i].points=-1;
        strcpy(ranking[i].name, "\0");
    }

    char received_name[MAX_INPUT_CHARS + 1] = "\0";      // NOTE: One extra space required for line ending char '\0'
    int received_points, letterCount = 0, framesCounter = 0, current_screen = RECEIVE_NAME;
    srand(framesCounter);

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "RANKING");
    SetTargetFPS(60);               // Set our game to run at 10 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {   
        if(current_screen == RECEIVE_NAME){
            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();
            // Check if more characters have been pressed on the same frame
            while (key > 0){
                // NOTE: Only allow keys in range [32..125]
                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                {
                    received_name[letterCount] = (char)key;
                    received_name[letterCount+1] ='\0';
                    letterCount++;
                }
                key = GetCharPressed();  // Check next character in the queue
            }
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                received_name[letterCount] = '\0';
            }
            if(IsKeyPressed(KEY_ENTER) && letterCount==MAX_INPUT_CHARS){
                received_name[letterCount + 1] = '\0';
                received_points= rand()%10000;
                if(received_points > ranking[4].points){
                    ranking[4].points = received_points;
                    strcpy(ranking[4].name,received_name);
                    Update_ranking(ranking);
                }
                current_screen=RANKING;
            }

        }else if(current_screen==RANKING){
            if(IsKeyPressed(KEY_R)){
                for(int i=letterCount;i>=0;i--){
                    received_name[i] = '\0';
                    if (i == 0) letterCount = 0;
                }
                current_screen=RECEIVE_NAME;  
            }   
        }
        framesCounter++;
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);

            if(current_screen==RECEIVE_NAME){
                Save_name(received_name, letterCount, framesCounter);
            }else Draw_score_screen(ranking);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}