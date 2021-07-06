#include "raylib.h"
#include "frameMapping.c"

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1

enum STATE{IDLE, WALKING, ATTACKING, DYING};
enum ENTITY_CLASS{ASSASSIN, RIFLEMAN, HERO};

enum STATE lowerState, upperState;
enum ENTITY_CLASS class;

int LOWER_IDLE;
int LOWER_WALKING;
int UPPER_IDLE;
int UPPER_WALKING;
int UPPER_ATTACKING;
int BODY_DYING;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [texture] example - texture rectangle");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D textureAtlas = LoadTexture("resources/Atlas/assassin_atlas_div.png"); 
    //Texture2D textureAtlas = LoadTexture("resources/Atlas/rifleman_atlas_div.png"); 
    int GRID[2] = {135, 135};
    int animUpperRow = 2;
    int animUpperCol = 0;
    int animUpperNumFrames = 7;
    int animLowerRow = 0;
    int animLowerCol = 0;
    int animLowerNumFrames = 7;
    int animation = 0;

    Vector2 position = { 350.0f, 280.0f };
    Rectangle frameUpperRec = { animUpperCol*GRID[0], animUpperRow*GRID[1], GRID[0], GRID[1] };
    Rectangle frameLowerRec = { animLowerCol*GRID[0], animLowerRow*GRID[1], GRID[0], GRID[1] };
    int currentUpperFrame = 0;
    int currentLowerFrame = 0;

    int framesCounter = 0;
    int framesSpeed = 8;            // Number of spritesheet frames shown by second

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        framesCounter++;

        switch (class){
            case ASSASSIN:
                GRID[0] = ASSASSIN_GRID[0];
                GRID[1] = ASSASSIN_GRID[1];
                LOWER_IDLE = ASSASSIN_LEGS_IDLE_ROW;
                LOWER_WALKING = ASSASSIN_LEGS_WALKING_ROW;
                UPPER_IDLE = ASSASSIN_UPPER_IDLE_ROW;
                UPPER_WALKING = ASSASSIN_UPPER_WALKING_ROW;
                UPPER_ATTACKING = ASSASSIN_UPPER_ATTACKING_ROW;
                BODY_DYING = ASSASSIN_BODY_DYING_ROW;
                break;
            case RIFLEMAN:
                GRID[0] = ASSASSIN_GRID[0];
                GRID[1] = ASSASSIN_GRID[1];
                LOWER_IDLE = ASSASSIN_LEGS_IDLE_ROW;
                LOWER_WALKING = ASSASSIN_LEGS_WALKING_ROW;
                UPPER_IDLE = ASSASSIN_UPPER_IDLE_ROW;
                UPPER_WALKING = ASSASSIN_UPPER_WALKING_ROW;
                UPPER_ATTACKING = ASSASSIN_UPPER_ATTACKING_ROW;
                BODY_DYING = ASSASSIN_BODY_DYING_ROW;
                break;
            default:
                break;
        }

        switch (animation)
        {
        case 0:
            // IDLE
            animLowerRow = 0;
            animLowerNumFrames = 7;
            animUpperRow = UPPER_IDLE;
            animUpperNumFrames = 7;
            break;
        case 1:
            // WALKING
            animLowerRow = 1;
            animLowerNumFrames = 8;
            animUpperRow = UPPER_WALKING;
            animUpperNumFrames = 8;
            break;
        case 2:
            // ATTACK STILL
            animLowerRow = 0;
            animLowerNumFrames = 7;
            animUpperRow = UPPER_ATTACKING;
            animUpperNumFrames = 4;
            break;
        case 3:
            // ATTACK WALKING
            animLowerRow = 1;
            animLowerNumFrames = 8;
            animUpperRow = UPPER_ATTACKING;
            animUpperNumFrames = 4;
            break;
        case 4:
            // DYING
            animLowerRow = 5;
            animLowerNumFrames = 6;
            animUpperRow = -1;
            animUpperNumFrames = 1;
            break;
        }

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentUpperFrame++;
            currentLowerFrame++;

            if (currentUpperFrame > animUpperNumFrames-1) currentUpperFrame = (animation == 4 ? animUpperNumFrames-1 : 0);
            if (currentLowerFrame > animLowerNumFrames-1) currentLowerFrame = (animation == 4 ? animLowerNumFrames-1 : 0);

            frameUpperRec = (Rectangle) { currentUpperFrame*GRID[0], animUpperRow*GRID[1], GRID[0], GRID[1] };
            frameLowerRec = (Rectangle) { currentLowerFrame*GRID[0], animLowerRow*GRID[1], GRID[0], GRID[1] };
        }

        if (IsKeyPressed(KEY_RIGHT)) framesSpeed++;
        else if (IsKeyPressed(KEY_LEFT)) framesSpeed--;

        if (IsKeyPressed(KEY_UP)) {
            animation--;
            currentUpperFrame = 0;
            currentLowerFrame = 0;
        } else if (IsKeyPressed(KEY_DOWN)) {
            animation++;
            currentUpperFrame = 0;
            currentLowerFrame = 0;
        }
        if (framesSpeed > MAX_FRAME_SPEED) framesSpeed = MAX_FRAME_SPEED;
        else if (framesSpeed < MIN_FRAME_SPEED) framesSpeed = MIN_FRAME_SPEED;

        if (animation > 4) animation = 4;
        else if (animation < 0) animation = 0;


        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
            DrawText(TextFormat("%02i FPS", framesSpeed), 575, 210, 10, DARKGRAY);
            DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, DARKGRAY);
            Color selected = GRAY;
            Color unselected = LIGHTGRAY;

            for (int i = 0; i < 5; i++) {
                char temp[30] = "";
                if (i == 0) strcat(temp, "IDLE");
                if (i == 1) strcat(temp, "WALKING");
                if (i == 2) strcat(temp, "ATTACK STILL");
                if (i == 3) strcat(temp, "ATTACK WALKING");
                if (i == 4) strcat(temp, "DYING");

                DrawText(temp, 5, 200 + i*25, 20, (i == animation ? selected : unselected));
            }
            DrawText(TextFormat("LowerY %02i", animLowerRow), 5, 325, 20, BLUE);
            DrawText(TextFormat("UpperY %02i", animUpperRow), 5, 350, 20, BLUE);

            DrawTextureRec(textureAtlas, frameLowerRec, position, WHITE);  // Draw part of the texture
            DrawTextureRec(textureAtlas, frameUpperRec, position, WHITE);  // Draw part of the texture

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(textureAtlas);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}