/*******************************************************************************************
*
*   raylib [textures] example - Background scrolling
*
*   This example has been created using raylib 2.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2019 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "C:/raylib/src/raylib.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Projeto");

    // NOTE: Be careful, background width must be equal or bigger than screen width
    // if not, texture should be draw more than two times for scrolling effect
    Texture2D background = LoadTexture("resources/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");

    float Background = 0.0f;
    float Midground = 0.0f;
    float Foreground = 0.0f;

    //----------------------------------------------------------------------------------------

    Texture2D character = LoadTexture("resources/walk.png");        // Texture loading


    //Posição do personagem
    //----------------------------------------------------------------------------------------
    float character_x=0.0f;
    float character_y=330.0f;
    Vector2 position = { character_x,character_y };
    //----------------------------------------------------------------------------------------

    
    int move_x = 1;                 //Velocidade de movimento

    Rectangle frameRec = { 0.0f, 0.0f, (float)character.width/34, (float)character.height };
    int currentFrame = 0;

    int framesCounter = 0;
    int framesSpeed = 8;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        framesCounter++;

        //Andar do personagem
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_RIGHT)){
            position.x +=move_x;
            if (framesCounter >= (60/framesSpeed)){
              framesCounter = 0;
              currentFrame++;

              if (currentFrame > 16) currentFrame = 1;

              frameRec.x = (float)currentFrame*(float)character.width/34;

            }
        }
        else if (IsKeyDown(KEY_LEFT)){
            position.x -=move_x;
            if (framesCounter >= (60/framesSpeed)){
              framesCounter = 0;
              currentFrame--;

              if (currentFrame > 34 || currentFrame <18) currentFrame = 33;

              frameRec.x = (float)currentFrame*(float)character.width/34;

            }
        }

        if(IsKeyReleased(KEY_RIGHT)){
            currentFrame = 0;
            frameRec.x = (float)currentFrame*(float)character.width/34;
        }
        else if(IsKeyReleased(KEY_LEFT)){
            currentFrame = 17;
            frameRec.x = (float)currentFrame*(float)character.width/34;
        }
        //----------------------------------------------------------------------------------
        

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(0x052c46ff));

            // Draw background image
            // NOTE: Texture is scaled twice its size
            DrawTextureEx(background, (Vector2){ Background, 20 }, 0.0f, 2.0f, WHITE);
            // Draw midground image
            DrawTextureEx(midground, (Vector2){ Midground, 20 }, 0.0f, 2.0f, WHITE);
            // Draw foreground image
            DrawTextureEx(foreground, (Vector2){ Foreground, 70 }, 0.0f, 2.0f, WHITE);
            // Draw the character's walk
            DrawTextureRec(character, frameRec, position, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(background);  // Unload background texture
    UnloadTexture(midground);   // Unload midground texture
    UnloadTexture(foreground);  // Unload foreground texture

    UnloadTexture(character);   // Unload character texture

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}