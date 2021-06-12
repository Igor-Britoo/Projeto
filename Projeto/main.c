/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description: Project: N30 - N Scketch 
 *
 *        Version:  1.0
 *        Created:  11/06/2021 14:00:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:
 *
 * =====================================================================================
 */

#include "raylib.h"
#include <stdio.h>

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 1
#define GRAVIDADE 700
#define ALTURA_PERSONAGEM 100
#define LARGURA_PERSONAGEM 100



//typedef enum GameScreen {LOGO = 0, TITLE, GAMEPLAY, ENDING} GameScreen;

typedef struct Jogador{
    Vector2 posicao;
    Vector2 tamanho;
    float velocidade;
    bool direcaoPersonagem;
    bool colisao;
} Jogador;


//prototipos das funções
static void UpdateJogador(Jogador *jogador, float delta, Rectangle colisor);
static void DrawJogador(Jogador *jogador, Texture2D sprite, Rectangle frame);


    int
main ( int argc, char *argv[] )
{

    //Tamanho da Janela
    const int screenWidth = 1366;
    const int screenHeight = 768;

    const char titulo[14] = "Project: N30-N";

    

    InitWindow(screenWidth, screenHeight, titulo);

    //Inicializando texturas
        //Personagem
    Texture2D spritePersonagem = LoadTexture("resources/walk.png");
    Rectangle frameRec = {0, 0, (float)spritePersonagem.width/16, (float)spritePersonagem.height};
    Rectangle chao = {0, 750, screenWidth , screenHeight};
        //Background
    Texture2D background = LoadTexture("resources/cyberpunk_street_background.png");
    Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");

    


    //Inicializando Jogador
    static Jogador jogador;
    jogador.posicao = (Vector2){screenWidth/2, 0};
    jogador.velocidade = 300;
    jogador.tamanho.x = jogador.posicao.x + LARGURA_PERSONAGEM;
    jogador.tamanho.y = jogador.posicao.y + ALTURA_PERSONAGEM;

    //GameScreen telaAtual = LOGO;
    int framesCounter = 0;
    SetTargetFPS(60);
    float deltaTime;

    //Loop principal do jogo
    while(!WindowShouldClose()){
    deltaTime = GetFrameTime();

    BeginDrawing();

    ClearBackground(GetColor(0x052c46ff));

    DrawTextureEx(background, (Vector2){0 ,0}, 0, 3, WHITE);
    DrawTextureEx(midground, (Vector2){0 ,0}, 0, 3, WHITE);
    DrawTextureEx(foreground, (Vector2){0 ,200}, 0, 3, WHITE);
    
    UpdateJogador(&jogador, deltaTime, chao);
    DrawJogador(&jogador, spritePersonagem, frameRec);
    //DrawRectangleRec(chao, WHITE);


    EndDrawing();
    }


    UnloadTexture(spritePersonagem);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
    return 1; 
}		
/* ----------  end of function main  ---------- */

static void UpdateJogador(Jogador *jogador, float delta, Rectangle colisor){

    //gravidade
    jogador->posicao.y += GRAVIDADE * delta;

    if (IsKeyDown(KEY_RIGHT)){ 
        jogador->posicao.x += jogador->velocidade * delta;
        jogador->direcaoPersonagem = 1;
    }
    if (IsKeyDown(KEY_LEFT)){
        jogador->posicao.x -= jogador->velocidade * delta;
        jogador->direcaoPersonagem = 0;
    }
    if (IsKeyDown(KEY_UP)) jogador->posicao.y -= 5 * jogador->velocidade * delta;
    if (IsKeyDown(KEY_DOWN)) jogador->posicao.y += jogador->velocidade *  delta;

    jogador->tamanho.x = jogador->posicao.x + LARGURA_PERSONAGEM;
    jogador->tamanho.y = jogador->posicao.y + ALTURA_PERSONAGEM;

    if (CheckCollisionPointRec(jogador->tamanho, colisor)) jogador->posicao.y = colisor.y - ALTURA_PERSONAGEM;

}
    
static void DrawJogador(Jogador *jogador, Texture2D sprite, Rectangle frame){
    jogador->direcaoPersonagem == 1 ? 0 : (frame.width *=-1);
    DrawTextureRec(sprite, frame, jogador->posicao, WHITE);
}

