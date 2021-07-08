#include "gameConfig.c"
#include "screenScore.c"

int main(void) {
    if (isFullscreen) SetConfigFlags(FLAG_FULLSCREEN_MODE); // Fullscreen
    InitWindow(screenWidth, screenHeight, gameName);
    SetTargetFPS(60);
    SetExitKey(-1);
    enum GAME_STATE gameState = MENU;
    HideCursor();

    // Load assets
    Texture2D characterTexDiv = LoadTexture("resources/Atlas/hero_atlas_div.png");    
    Texture2D miscAtlas = LoadTexture("resources/Atlas/misc_atlas.png");        
    Texture2D backgroundAtlas = LoadTexture("resources/Atlas/background_atlas.png");        
    Texture2D midgroundAtlas = LoadTexture("resources/Atlas/midground_atlas.png");        
    Texture2D envPropsAtlas = LoadTexture("resources/Atlas/env_props_atlas.png");        
    Texture2D foregroundAtlas = LoadTexture("resources/Atlas/foreground_atlas.png");

    Texture2D *enemyTex = (Texture2D *)malloc(numEnemyClasses*sizeof(Texture2D));
    enemyTex[SWORDSMAN] = LoadTexture("resources/Atlas/hero_atlas_div.png");
    enemyTex[ASSASSIN] = LoadTexture("resources/Atlas/assassin_atlas_div.png");
    enemyTex[GUNNER] = LoadTexture("resources/Atlas/gunner_atlas_div.png");
    enemyTex[SNIPERSHOOTER] = LoadTexture("resources/Atlas/hero_atlas_div.png");
    enemyTex[DRONE] = LoadTexture("resources/Atlas/hero_atlas_div.png");
    enemyTex[TURRET] = LoadTexture("resources/Atlas/hero_atlas_div.png");
    enemyTex[BOSS] = LoadTexture("resources/Atlas/hero_atlas_div.png");

    InitAudioDevice();              // Initialize audio device
    SetMasterVolume(0.3f);
    Music ambience = LoadMusicStream("resources/Audio/ambience.mp3");
    Sound *fxSoundPool = (Sound *)malloc(10*sizeof(Sound));
    fxSoundPool[FX_MAGNUM] = LoadSound("resources/Audio/magnumShot.ogg"); 
    fxSoundPool[FX_SWORD] = LoadSound("resources/Audio/meleeAtaque.ogg"); 
    fxSoundPool[FX_CHANGE_SELECTION] = LoadSound("resources/Audio/menuSelectionChange.ogg"); 
    fxSoundPool[FX_SELECTED] = LoadSound("resources/Audio/menuSelected.ogg"); 
    fxSoundPool[FX_ENTITY_LANDING] = LoadSound("resources/Audio/entityLanding.ogg"); 
    fxSoundPool[FX_GRENADE_LAUNCH] = LoadSound("resources/Audio/grenadeLaunch.ogg"); 
    fxSoundPool[FX_GRENADE_BOUNCING] = LoadSound("resources/Audio/grenadeBouncing.ogg"); 
    fxSoundPool[FX_GRENADE_EXPLOSION] = LoadSound("resources/Audio/grenadeExplosion.ogg"); 
    fxSoundPool[FX_HURT] = LoadSound("resources/Audio/hurt.ogg"); 
    fxSoundPool[FX_DYING] = LoadSound("resources/Audio/dying.ogg"); 

    SetSoundVolume(fxSoundPool[FX_ENTITY_LANDING], 1.5f);
    SetSoundVolume(fxSoundPool[FX_GRENADE_EXPLOSION], 2);
    SetSoundVolume(fxSoundPool[FX_HURT], 1.2f);
    SetSoundVolume(fxSoundPool[FX_DYING], 2);

    PlayMusicStream(ambience);

    // MENU
    Texture2D menuBackground = LoadTexture("resources/Menu/menu_fundo.png");
    Texture2D logo = LoadTexture("resources/Menu/logo.png");
    int currentOption = 1;
    int nextScreen = -1;
    bool changeScreen = false;

    // Scores
    FILE *fptr;
    int numOfScores = 5;
    char data[100];
    char fileName[200];
    strcpy(fileName, "resources/Text/scores.txt");
    char *name;
    int choice = 0;
    Score *scorePool = (Score *)malloc(2*numOfScores*sizeof(Score));
    int indexPos = 0;
    for (int i = 0; i < 2*numOfScores; i+=2) {
        indexPos++;
        strcpy(scorePool[i].name," ");
        strcpy(scorePool[i+1].point,"0");
    }
    if (CheckIfFileExists(fileName)) {
        ReadScore(fptr, fileName, scorePool);
    } else {
        WriteScore(fptr, fileName, scorePool);
    }

Menu:
    currentOption = 5;
    nextScreen = -1;
    changeScreen = false;
    while (!changeScreen) {
        UpdateMusicStream(ambience);
        if (gameState == MENU) {
            if (IsKeyPressed(KEY_DOWN)) {
                currentOption++;
                PlaySoundMulti(fxSoundPool[FX_CHANGE_SELECTION]);
            } else if (IsKeyPressed(KEY_UP)) {
                PlaySoundMulti(fxSoundPool[FX_CHANGE_SELECTION]);
                currentOption--;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                nextScreen = currentOption;
                PlaySoundMulti(fxSoundPool[FX_SELECTED]);
            }

            if (currentOption > 3) currentOption = 1;
            if (currentOption < 1) currentOption = 3;
            
            BeginDrawing();
                ClearBackground(GetColor(0x052c46ff));
                DrawTexturePro(menuBackground, (Rectangle){0, 0, menuBackground.width, menuBackground.height}, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2) {0, 0}, 0, WHITE);
                DrawTexturePro(logo, (Rectangle){0, 0, logo.width, logo.height}, (Rectangle){(screenWidth - logo.width)/2, (screenHeight - logo.height)/2, logo.width, logo.height}, (Vector2) {0, 0}, 0, WHITE);
                DrawText("START GAME", screenWidth/2 - MeasureText("START GAME", 40)/2, screenHeight/2 + 150, 40,(currentOption == 1 ? YELLOW : WHITE));
                DrawText("HIGHSCORES", screenWidth/2 - MeasureText("HIGHSCORES", 40)/2, screenHeight/2 + 200, 40,(currentOption == 2 ? YELLOW : WHITE));
                DrawText("EXIT", screenWidth/2 - MeasureText("EXIT", 40)/2, screenHeight/2 + 250, 40,(currentOption == 3 ? YELLOW : WHITE));
            EndDrawing();

            if (nextScreen == 1) {
                gameState = ACTIVE;
                changeScreen = true;
            } else if (nextScreen == 2) {
                gameState = SCORE;
                currentOption = 1;
                nextScreen = -1;
            } else if (nextScreen == 3) {
                gameState = ACTIVE;
                changeScreen = true;
                goto Quit;
            }

        }
        else if (gameState == SCORE) {
            
            if (IsKeyPressed(KEY_ENTER)) {
                nextScreen = currentOption;
                PlaySoundMulti(fxSoundPool[FX_SELECTED]);
            }

            currentOption = 1;

            BeginDrawing();
                ClearBackground(GetColor(0x052c46ff));
                DrawTexturePro(menuBackground, (Rectangle){0, 0, menuBackground.width, menuBackground.height}, (Rectangle){0, 0, screenWidth, screenHeight}, (Vector2) {0, 0}, 0, WHITE);
                int pos = 0;
                    DrawText("HIGHSCORES",screenWidth/2 - MeasureText("HIGHSCORES", 70)/2, 60, 70, WHITE);
                for (int i = 0; i < 10; i+=2) {
                    pos++;
                    DrawText(TextFormat("%01d", pos), screenWidth/3 - MeasureText(TextFormat("%01d", pos), 60)/2 + 20, 180 + pos*70, 60, YELLOW);
                    DrawText(scorePool[i].name,screenWidth/2 - MeasureText(scorePool[i].name, 60)/2, 180 + pos*70, 60, WHITE);
                    DrawText(scorePool[i+1].point, screenWidth/3 *2 , 180 + pos*70, 60, WHITE);
                }
                DrawText("BACK", screenWidth/2 - MeasureText("BACK", 40)/2, screenHeight/2 + 250, 40, YELLOW);
            EndDrawing();

            if (nextScreen == 1) {
                gameState = MENU;
                currentOption = 1;
                nextScreen = -1;
            }
        }
    }

    /////// INÍCIO DO JOGO
    // Controle de fluxo do jogo
    float time = 0;
    int difficulty = 0;

    // Player Init
    Player player = CreatePlayer(100, (Vector2){122, 200},122, 122);

    // Camera init
    float camMinX = 0; // Usado no avanço da câmera e na limitação de movimentação para trás do player
    float camMaxX = 0; // Usado no avanço da câmera
    Camera2D camera = CreateCamera(player.entity.position, (Vector2) {screenWidth/2.0f, screenHeight/2.0f}, 0.0f, 1.00f);

    // General Init
    Bullet *bulletsPool = (Bullet *)malloc(maxNumBullets*sizeof(Bullet));
    Grenade *grenadesPool = (Grenade *)malloc(maxNumGrenade*sizeof(Grenade));
    Ground *groundPool = (Ground *)malloc(maxNumGrounds*sizeof(Ground));
    EnvProps *envPropsPool = (EnvProps *)malloc(maxNumEnvProps*sizeof(EnvProps));
    Enemy *enemyPool = (Enemy *)malloc(maxNumEnemies*sizeof(Enemy));
    Particle *particlePool = (Particle *)malloc(maxNumParticles*sizeof(Particle));
    MSGSystem *msgPool = (MSGSystem *)malloc(maxNumMSGs*sizeof(MSGSystem));
    Background *nearBackgroundPool = (Background *)malloc(numBackgroundRendered*sizeof(Background));
    Background *middleBackgroundPool = (Background *)malloc(numBackgroundRendered*sizeof(Background));
    Background *farBackgroundPool = (Background *)malloc(numBackgroundRendered*sizeof(Background));

    int maxCount = 0;
    int numNearBackground = 0, numMiddleBackground = 0, numFarBackground = 0; // Usado para posicionamento correto das novas imagens geradas
    maxCount = fmax(maxNumGrounds, maxNumGrenade);
    maxCount = fmax(maxCount, maxNumEnvProps);
    maxCount = fmax(maxCount, maxNumEnemies);
    maxCount = fmax(maxCount, maxNumBullets);
    maxCount = fmax(maxCount, maxNumMSGs);
    maxCount = fmax(maxCount, numBackgroundRendered);
    for (int i = 0; i < maxCount; i++) {
        if (i < maxNumBullets) {
            bulletsPool[i].isActive = false;
        }

        if (i < maxNumGrenade) {
            grenadesPool[i].isActive = false;
        }

        if (i < maxNumGrounds) {
            groundPool[i].isActive = false;
        }

        if (i < maxNumEnvProps) {
            envPropsPool[i].isActive = false;
        }

        if (i < maxNumEnemies) {
            enemyPool[i].isAlive = false;
        }

        if (i < maxNumMSGs) {
            msgPool[i].isActive = false;
        }

        if (i < maxNumParticles) {
            particlePool[i].isActive = false;
        }

    }

    // Criar chão
    CreateGround(groundPool, (Vector2){0,screenHeight-60},screenWidth*7,5, true, true, false, true, false, -1); // Chão (esse é sempre existente)

    // Criar chunks
    for (int i = 0; i < numBackgroundRendered; i++) {
        farBackgroundPool[i] = CreateBackground(&player, enemyPool, envPropsPool, farBackgroundPool, groundPool, backgroundAtlas, BACKGROUND, &numFarBackground, i, difficulty);
        middleBackgroundPool[i] = CreateBackground(&player, enemyPool, envPropsPool, middleBackgroundPool, groundPool, midgroundAtlas, MIDDLEGROUND, &numMiddleBackground, i, difficulty);
        nearBackgroundPool[i] = CreateBackground(&player, enemyPool, envPropsPool, nearBackgroundPool, groundPool, foregroundAtlas, FOREGROUND, &numNearBackground, i, difficulty);
    }


    int framesCounter = 0;
    int received_points, letterCount = 0;
    char received_name[3 + 1] = "\0";      // NOTE: One extra space required for line ending char '\0'
    // Loop do jogo
    while (!WindowShouldClose()) {
        framesCounter++;
        UpdateMusicStream(ambience);   // Update music buffer with new stream data
        UpdateDifficulty(&difficulty, camMinX, time);

        // Game State
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (gameState == ACTIVE) {
                gameState = PAUSE;
                nextScreen = -1;
                currentOption = 1;
            }
        }

        // Verificar Game Over
        if (player.entity.lowerAnimation.currentAnimationState == DEAD) {
            gameState = GAMEOVER;
        }

        // Jogo em andamento
        if (gameState == ACTIVE) {
            // Atualizar fluxo
            float deltaTime = GetFrameTime();
            time += deltaTime;
            
            // Atualizar player
            UpdatePlayer(&player, enemyPool, bulletsPool, grenadesPool, deltaTime, groundPool, envPropsPool, particlePool,fxSoundPool, msgPool, camMinX, difficulty);

            // Atualizar limites de câmera e posição
            camMinX = (camMinX < camera.target.x - camera.offset.x ? camera.target.x - camera.offset.x : camMinX);
            UpdateClampedCameraPlayer(&camera, &player, deltaTime, screenWidth, screenHeight, &camMinX, &camMaxX);

                for (int i = 0; i < maxNumEnemies; i++) {
                    if (enemyPool[i].isAlive) 
                        UpdateEnemy(&enemyPool[i], &player, bulletsPool, deltaTime, groundPool, envPropsPool, fxSoundPool, particlePool, msgPool, camMinX, difficulty);
                }

                for (int i = 0; i < maxNumBullets; i++) {
                    if (bulletsPool[i].isActive) 
                        UpdateBullets(&bulletsPool[i], enemyPool, &player, msgPool, groundPool, envPropsPool, fxSoundPool, particlePool, deltaTime, camMaxX, difficulty);
                }

                for (int i = 0; i < maxNumGrenade; i++) {
                    if (grenadesPool[i].isActive)
                        UpdateGrenades(&grenadesPool[i], enemyPool, &player, msgPool, groundPool, envPropsPool, particlePool, fxSoundPool, deltaTime, difficulty);
                }

                for (int i = 0; i < maxNumGrounds; i++) {
                    if (groundPool[i].isActive) 
                        UpdateGrounds(&player, &groundPool[i], deltaTime, camMinX);
                }

                for (int i = 0; i < maxNumEnvProps; i++) {
                    if (envPropsPool[i].isActive)
                        UpdateEnvProps(&player, enemyPool, &envPropsPool[i], groundPool, particlePool, fxSoundPool, msgPool, deltaTime, camMinX);
                }

                for (int i = 0; i < maxNumParticles; i++) {
                    if (particlePool[i].isActive) 
                        UpdateParticles(&particlePool[i], deltaTime, camMinX);
                }

                for (int i = 0; i < maxNumMSGs; i++) {
                    if (msgPool[i].isActive) 
                        UpdateMSGs(&msgPool[i], deltaTime);
                }

                for (int i = 0; i < numBackgroundRendered; i++) {
                    UpdateBackground(&player, nearBackgroundPool, i, foregroundAtlas, enemyPool, envPropsPool, groundPool, deltaTime, &numNearBackground, camMinX, &camMaxX, difficulty);
                    UpdateBackground(&player, middleBackgroundPool, i, midgroundAtlas, enemyPool, envPropsPool, groundPool, deltaTime, &numMiddleBackground, camMinX, &camMaxX, difficulty);
                    UpdateBackground(&player, farBackgroundPool, i, backgroundAtlas, enemyPool, envPropsPool, groundPool, deltaTime, &numFarBackground, camMinX, &camMaxX, difficulty);
                }
            

        }

        // Draw cycle
        
        if (gameState == ACTIVE || gameState == PAUSE) {
            BeginDrawing();
                ClearBackground(GetColor(0x052c46ff));
                BeginMode2D(camera);
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////////////// OS BACKGROUNDS PRECISAM SER DESENHADOS ANTES DE QUALQUER COISA
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    
                    // Desenhar os backgrounds
                    for (int i = 0; i < numBackgroundRendered; i++) {
                        DrawTextureRec(farBackgroundPool[i].canvas.texture, (Rectangle) { 0, 0, (float)farBackgroundPool[i].canvas.texture.width, (float)-farBackgroundPool[i].canvas.texture.height },
                        (Vector2) { farBackgroundPool[i].position.x, farBackgroundPool[i].position.y }, WHITE);
                    }       
                    // Desenhar os middlegrounds
                    for (int i = 0; i < numBackgroundRendered; i++) {
                        DrawTextureRec(middleBackgroundPool[i].canvas.texture, (Rectangle) { 0, 0, (float)middleBackgroundPool[i].canvas.texture.width, (float)-middleBackgroundPool[i].canvas.texture.height },
                        (Vector2) { middleBackgroundPool[i].position.x, middleBackgroundPool[i].position.y }, WHITE);
                    }       
                    // Desenhar os foregrounds
                    for (int i = 0; i < numBackgroundRendered; i++) {
                        DrawTextureRec(nearBackgroundPool[i].canvas.texture, (Rectangle) { 0, 0, (float)nearBackgroundPool[i].canvas.texture.width, (float)-nearBackgroundPool[i].canvas.texture.height },
                        (Vector2) { nearBackgroundPool[i].position.x, nearBackgroundPool[i].position.y }, WHITE);
                    }

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   

                    
                    for (int i = 0; i < maxNumGrounds; i++) {
                        if (groundPool[i].isActive)
                            if (!groundPool[i].isInvisible)
                                DrawRectangleRec(groundPool[i].rect, WHITE);
                    }

                    for (int i = 0; i < maxNumEnvProps; i++) {
                        if (envPropsPool[i].isActive) {
                            DrawTexturePro(envPropsAtlas, envPropsPool[i].frameRect, envPropsPool[i].drawableRect, (Vector2) {0, 0}, 0, WHITE);
                        }
                    }

                    for (int i = 0; i < maxNumEnemies; i++) {
                        if (enemyPool[i].isAlive) 
                            DrawEnemy(&enemyPool[i], enemyTex, false, false, false); //enemypool, enemytex, detecção, vida, colisão
                    }

                    for (int i = 0; i < maxNumBullets; i++) {
                        if (bulletsPool[i].isActive) 
                            DrawBullet(&bulletsPool[i], miscAtlas, false); //bulletspool, miscAtlas, colisão                        
                    }

                    for (int i = 0; i < maxNumGrenade; i++) {
                        if (grenadesPool[i].isActive)
                            DrawGrenade(&grenadesPool[i], miscAtlas, false); //grenadespool, miscAtlas, colisão      
                    }

                    // Draw player
                    DrawPlayer(&player, characterTexDiv, false);

                    for (int i = 0; i < maxNumParticles; i++) {
                        if (particlePool[i].isActive) 
                            DrawParticle(&particlePool[i], miscAtlas); //grenadespool, miscAtlas                        
                    }

                    // Msgs acima de tudo
                    for (int i = 0; i < maxNumMSGs; i++) {
                        if (msgPool[i].isActive) 
                            DrawMSG(&msgPool[i]); 

                    }
                EndMode2D();

                DrawText(TextFormat("Difficulty: %d", difficulty), screenWidth - 300, 20, 40, WHITE);
                // HUD
                // Timer
                int min = (int) (time/60);
                int sec = time - min*60;
                DrawText(TextFormat("%02d:%02d", min, sec), screenWidth/2 - 40*5/2, 20, 40, WHITE);
                
                // Player HP
                int HPBarWidth = 250;
                float percentHP = ((float) player.entity.currentHP / (float) player.entity.maxHP);
                int currentHPBarWidth = percentHP * HPBarWidth;
                DrawRectangle(7, 47, HPBarWidth, 15, DARKGRAY); 
                DrawRectangle(7, 47, currentHPBarWidth, 15, (percentHP < 0.33f ? RED : percentHP < 0.67f ? YELLOW : GREEN)); 
                DrawRectangleLines(7, 47, HPBarWidth, 15, WHITE); 
                
                // Player Ammo
                for (int i = 0; i < 3; i++) // aumentar a espessura da borda
                    DrawRectangleLines(300+i, 7+i, 300-2*i, 80-2*i, WHITE); 
                
                DrawText("Ammo", 320, 17, 20, WHITE);
                DrawText(TextFormat("%003d", player.entity.magnumAmmo), 320, 44, 20, WHITE);
                DrawText("Grenade", 440, 17, 20, WHITE);
                DrawText(TextFormat("%003d", player.entity.grenadeAmmo), 440, 44, 20, WHITE);

                // Player points
                DrawText(TextFormat("%00000000000000015ld", player.points), 7, 7, 30, WHITE);
                
                // Pause menu
                if (gameState == PAUSE) {
                    if (IsKeyPressed(KEY_DOWN)) {
                        currentOption++;
                        PlaySoundMulti(fxSoundPool[FX_CHANGE_SELECTION]);
                    } else if (IsKeyPressed(KEY_UP)) {
                        PlaySoundMulti(fxSoundPool[FX_CHANGE_SELECTION]);
                        currentOption--;
                    }

                    if (IsKeyPressed(KEY_ENTER)) {
                        nextScreen = currentOption;
                        PlaySoundMulti(fxSoundPool[FX_SELECTED]);
                    }

                    if (currentOption > 2) currentOption = 1;
                    if (currentOption < 1) currentOption = 2;
                    
                    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.7f));
                    DrawText("PAUSED", screenWidth/2 - MeasureText("PAUSED", 80)/2, screenHeight/2 - 150, 80,WHITE);
                    
                    DrawText("Resume", screenWidth/2 - MeasureText("Resume", 40)/2, screenHeight/2 + 150, 40,(currentOption == 1 ? YELLOW : WHITE));
                    DrawText("Exit to menu", screenWidth/2 - MeasureText("Exit to menu", 40)/2, screenHeight/2 + 200, 40,(currentOption == 2 ? YELLOW : WHITE));

                    if (nextScreen == 1) {
                        gameState = ACTIVE;
                    } else if (nextScreen == 2) {
                        gameState = GAMEOVER;
                    }
                }
            EndDrawing();
        }
        else if (gameState == GAMEOVER) {
            BeginDrawing();
                ClearBackground(GetColor(0x052c46ff));
                if(player.points > atoi(scorePool[9].point)) {
                    int key = GetCharPressed();
                    
                    // Check if more characters have been pressed on the same frame
                    while (key > 0){
                        // NOTE: Only allow keys in range [32..125]
                        if ((key >= 32) && (key <= 125) && (letterCount < 3))
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
                    DrawText(TextFormat("POINTS: %d", player.points), 600, 250, 20, RED);
                    if(IsKeyPressed(KEY_ENTER) && letterCount==3){
                        received_name[letterCount + 1] = '\0';
                        received_points= player.points;
                        if(received_points > atoi(scorePool[9].point)) { // Verifica se está no top 5
                            UpdateScores(scorePool, received_name, received_points);
                            WriteScore(fptr, fileName, scorePool);
                        }
                        gameState = SCORE;
                        goto Menu;
                    }

                    DrawText(received_name, (screenWidth/2 - 100) + 5, screenHeight/2 + 8, 80, WHITE);
                    DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, 3), 315, 250, 20, DARKGRAY);

                    if (letterCount < 3)
                    {
                    // Draw blinking underscore char
                        if (((framesCounter/20)%2) == 0){
                            DrawText("_", (screenWidth/2 - 100) + 8 + MeasureText(received_name, 80), screenHeight/2 + 12, 80, WHITE);
                        }
                    }
                    else DrawText("Press ENTER to confirm", 230, 330, 20, GRAY);
                    
                    if(letterCount > 0)DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
                } else {
                    gameState = MENU;
                    goto Menu;
                }
            EndDrawing();
        }

    }

Quit:
    // Unload
    UnloadTexture(backgroundAtlas);
    UnloadTexture(midgroundAtlas);
    UnloadTexture(foregroundAtlas);
    UnloadTexture(envPropsAtlas);
    UnloadTexture(characterTexDiv);
    UnloadTexture(miscAtlas);
    UnloadTexture(logo);
    UnloadTexture(menuBackground);
    for (int i = 0; i < numBackgroundRendered; i++) {
        UnloadRenderTexture(farBackgroundPool[i].canvas);
        UnloadRenderTexture(nearBackgroundPool[i].canvas);
        UnloadRenderTexture(middleBackgroundPool[i].canvas);
    }
    for (int i = 0; i < numEnemyClasses; i++)
        UnloadTexture(enemyTex[i]);

    UnloadMusicStream(ambience);
    StopSoundMulti();       // We must stop the buffer pool before unloading

    for (int i = FX_MAGNUM; i <= FX_DYING; i++)
        UnloadSound(fxSoundPool[i]);     // Unload sound data
    CloseAudioDevice(); 

    free(fxSoundPool);
    free(enemyTex);
    free(bulletsPool);
    free(grenadesPool);
    free(groundPool);
    free(envPropsPool);
    free(enemyPool);
    free(particlePool);
    free(msgPool);
    free(nearBackgroundPool);
    free(middleBackgroundPool);
    free(farBackgroundPool); 

    CloseWindow();
    return 0;
}

Background CreateBackground(Player *player, Enemy *enemyPool, EnvProps *envPropsPool, Background *backgroundPool, Ground *groundPool, Texture2D srcAtlas, enum BACKGROUND_TYPES bgType, int *numBackground, int id, int difficulty) {
    Background dstBackground;
    int numBg = *numBackground;

    switch (bgType)
    {
    case BACKGROUND:
        dstBackground.relativePosition = -0.05f; // Velocidade do parallax (quanto menor, mais lento)
        PopulateChunk(numBg, envPropsPool, groundPool, enemyPool, difficulty);
        break;
    case MIDDLEGROUND:
        dstBackground.relativePosition = -0.025f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    case FOREGROUND:
        dstBackground.relativePosition = 0.0f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    }


    dstBackground.id = id;
    dstBackground.position.y = 0;
    dstBackground.canvas = PaintCanvas(srcAtlas, bgType, groundPool, numBg);
    dstBackground.width = dstBackground.canvas.texture.width;
    dstBackground.height = dstBackground.canvas.texture.height;
    dstBackground.bgType = bgType;
    dstBackground.position.x = numBg * dstBackground.width;
    dstBackground.originalX = dstBackground.position.x;
    *numBackground = numBg + 1;

    return dstBackground;
}

Player CreatePlayer (int maxHP, Vector2 position, int width, int height) {
    Player newPlayer;
    newPlayer.entity.maxHP = maxHP;
    newPlayer.entity.currentHP = maxHP;
    newPlayer.entity.position.x = position.x;
    newPlayer.entity.position.y = position.y;
    newPlayer.entity.velocity.x = 0.0f;
    newPlayer.entity.velocity.y = 0.0f;
    newPlayer.entity.momentum.x = 0.0f;
    newPlayer.entity.momentum.y = 0.0f;
    newPlayer.entity.grenadeAmmo = 0;
    newPlayer.entity.magnumAmmo = 350;
    newPlayer.entity.maxXSpeed = 400;
    newPlayer.entity.sprintSpeed = 800;
    newPlayer.entity.jumpSpeed = 250;
    newPlayer.entity.isGrounded = false;
    newPlayer.entity.eyesOffset = (Vector2) {55, 0};
    newPlayer.entity.type = PLAYER;
    newPlayer.points = 0;

    newPlayer.entity.timeSinceDeath = 0;
    newPlayer.entity.width = width;
    newPlayer.entity.height = height;
    newPlayer.entity.upperAnimation.animationFrameSpeed = 0.08f;
    newPlayer.entity.upperAnimation.animationFrameWidth = PLAYER_GRID[0];
    newPlayer.entity.upperAnimation.animationFrameHeight = PLAYER_GRID[0];
    newPlayer.entity.upperAnimation.currentAnimationFrame = 0;
    newPlayer.entity.upperAnimation.currentAnimationState = IDLE;
    newPlayer.entity.upperAnimation.isFacingRight = 1;
    newPlayer.entity.upperAnimation.timeSinceLastFrame = 0;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.x = 0.0f;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.y = 0.0f;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.width = newPlayer.entity.upperAnimation.animationFrameWidth;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.height = newPlayer.entity.upperAnimation.animationFrameHeight;
    newPlayer.entity.lowerAnimation.animationFrameSpeed = 0.08f;
    newPlayer.entity.lowerAnimation.animationFrameWidth = PLAYER_GRID[0];
    newPlayer.entity.lowerAnimation.animationFrameHeight = PLAYER_GRID[0];
    newPlayer.entity.lowerAnimation.currentAnimationFrame = 0;
    newPlayer.entity.lowerAnimation.currentAnimationState = IDLE;
    newPlayer.entity.lowerAnimation.isFacingRight = 1;
    newPlayer.entity.lowerAnimation.timeSinceLastFrame = 0;
    newPlayer.entity.lowerAnimation.currentAnimationFrameRect.x = 0.0f;
    newPlayer.entity.lowerAnimation.currentAnimationFrameRect.y = 0.0f;
    newPlayer.entity.lowerAnimation.currentAnimationFrameRect.width = newPlayer.entity.lowerAnimation.animationFrameWidth;
    newPlayer.entity.lowerAnimation.currentAnimationFrameRect.height = newPlayer.entity.lowerAnimation.animationFrameHeight;
    newPlayer.entity.characterWidthScale = 1.00f;
    newPlayer.entity.characterHeightScale = 1.00f;

    newPlayer.entity.drawableRect = (Rectangle) {position.x - width/2, position.y - height/2, width * newPlayer.entity.characterWidthScale, height * newPlayer.entity.characterHeightScale};
    newPlayer.entity.collisionBox = (Rectangle) {position.x - width/2, position.y - height/2, width * 0.8f, height};
    newPlayer.entity.collisionHead = (Circle) {(Vector2){position.x - width/2, position.y - height/2}, width * 0.8f};

    return newPlayer;
}

void CreateEnemy(Enemy *enemyPool, enum ENEMY_CLASSES class, Vector2 position, int width, int height) {

    for (int i = 0; i < maxNumEnemies; i++) {
        Enemy *newEnemy = enemyPool + i;
        if (!newEnemy->isAlive) {
            newEnemy->target = (Vector2){-1, -1};
            newEnemy->class = class;
            newEnemy->behavior = NONE;
            newEnemy->behaviorChangeInterval = 3.5f; // Tempo em segundos para tentar alterar comportamento
            newEnemy->timeSinceLastBehaviorChange = 0;
            newEnemy->noDetectionTime = 0;
            newEnemy->loseTargetInterval = 5;
            newEnemy->spawnLocation = (Vector2){position.x, position.y};
            newEnemy->maxDistanceToSpawn = 1000;
            newEnemy->isAlive = true;
            newEnemy->timeSinceLastAttack = 0;
            newEnemy->id = i;
            newEnemy->entity.type = ENEMY;

            newEnemy->entity.timeSinceDeath = 0;
            newEnemy->entity.position = newEnemy->spawnLocation;
            newEnemy->entity.velocity.x = 0.0f;
            newEnemy->entity.velocity.y = 0.0f;
            newEnemy->entity.momentum.x = 0.0f;
            newEnemy->entity.momentum.y = 0.0f;
            newEnemy->entity.maxXSpeed = 200;
            newEnemy->entity.sprintSpeed = 800;
            newEnemy->entity.jumpSpeed = 250;
            newEnemy->entity.isGrounded = false;
            newEnemy->entity.eyesOffset = (Vector2) {55, 40};
            newEnemy->entity.upPressed = 0;
            newEnemy->entity.downPressed = 0;

            newEnemy->entity.width = width;
            newEnemy->entity.height = height;
            newEnemy->entity.lowerAnimation.animationFrameSpeed = 0.10f;
            newEnemy->entity.lowerAnimation.currentAnimationFrame = 0;
            newEnemy->entity.lowerAnimation.currentAnimationState = IDLE;
            newEnemy->entity.lowerAnimation.isFacingRight = 1;
            newEnemy->entity.lowerAnimation.timeSinceLastFrame = 0;
            newEnemy->entity.lowerAnimation.currentAnimationFrameRect.x = 0.0f;
            newEnemy->entity.lowerAnimation.currentAnimationFrameRect.y = 0.0f;
            newEnemy->entity.upperAnimation.animationFrameSpeed = 0.10f;
            newEnemy->entity.upperAnimation.currentAnimationFrame = 0;
            newEnemy->entity.upperAnimation.currentAnimationState = IDLE;
            newEnemy->entity.upperAnimation.isFacingRight = 1;
            newEnemy->entity.upperAnimation.timeSinceLastFrame = 0;
            newEnemy->entity.upperAnimation.currentAnimationFrameRect.x = 0.0f;
            newEnemy->entity.upperAnimation.currentAnimationFrameRect.y = 0.0f;
            newEnemy->entity.characterWidthScale = 1.00f;
            newEnemy->entity.characterHeightScale = 1.00f;
            newEnemy->entity.lowerAnimation.animationFrameWidth = 0;
            newEnemy->entity.lowerAnimation.animationFrameHeight = 0;
            newEnemy->entity.upperAnimation.animationFrameWidth = 0;
            newEnemy->entity.upperAnimation.animationFrameHeight = 0;

            newEnemy->entity.GRID[0] = 0;
            newEnemy->entity.GRID[1] = 0;
            newEnemy->entity.LEGS_IDLE_ROW = 0;
            newEnemy->entity.LEGS_IDLE_NUM_FRAMES = 0;
            newEnemy->entity.LEGS_WALKING_ROW = 0;
            newEnemy->entity.LEGS_WALKING_NUM_FRAMES = 0;
            newEnemy->entity.UPPER_IDLE_ROW = 0;
            newEnemy->entity.UPPER_IDLE_NUM_FRAMES = 0;
            newEnemy->entity.UPPER_WALKING_ROW = 0;
            newEnemy->entity.UPPER_WALKING_NUM_FRAMES = 0;
            newEnemy->entity.UPPER_ATTACKING_ROW = 0;
            newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = 0;
            newEnemy->entity.BODY_DYING_ROW = 0;
            newEnemy->entity.BODY_DYING_NUM_FRAMES = 0;

            //Valores para range de ataque e de visão selecionados de forma arbitraria, atualizar posteriormente
            newEnemy->entity.maxHP = 100;
            newEnemy->entity.currentHP = newEnemy->entity.maxHP;
            switch (class){
                case SWORDSMAN:
                    newEnemy->viewDistance = 600;
                    newEnemy->attackRange = 0;
                    newEnemy->attackSpeed = 0.8f; // Ataques por segundo
                    newEnemy->entity.GRID[0] = PLAYER_GRID[0];
                    newEnemy->entity.GRID[1] = PLAYER_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = PLAYER_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = PLAYER_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.UPPER_IDLE_ROW = PLAYER_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = PLAYER_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = PLAYER_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = PLAYER_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = PLAYER_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = PLAYER_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = PLAYER_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = PLAYER_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = PTS_KILL_SWORDSMAN;
                    break;
                case ASSASSIN:
                    newEnemy->viewDistance = 600;
                    newEnemy->attackRange = 15;
                    newEnemy->attackSpeed = 0.8f; // Ataques por segundo
                    newEnemy->entity.maxXSpeed = 300;
                    newEnemy->entity.GRID[0] = ASSASSIN_GRID[0];
                    newEnemy->entity.GRID[1] = ASSASSIN_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = ASSASSIN_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = ASSASSIN_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = ASSASSIN_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = ASSASSIN_LEGS_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_IDLE_ROW = ASSASSIN_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = ASSASSIN_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = ASSASSIN_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = ASSASSIN_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = ASSASSIN_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = ASSASSIN_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = ASSASSIN_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = ASSASSIN_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = PTS_KILL_ASSASSIN;
                    break;
                case GUNNER:
                    newEnemy->viewDistance = 600;
                    newEnemy->attackRange = 400;
                    newEnemy->entity.eyesOffset = (Vector2) {55, 25};
                    newEnemy->attackSpeed = 2; // Ataques por segundo
                    newEnemy->entity.GRID[0] = GUNNER_GRID[0];
                    newEnemy->entity.GRID[1] = GUNNER_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = GUNNER_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = GUNNER_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = GUNNER_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = GUNNER_LEGS_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_IDLE_ROW = GUNNER_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = GUNNER_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = GUNNER_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = GUNNER_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = GUNNER_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = GUNNER_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = GUNNER_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = GUNNER_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = PTS_KILL_GUNNER;
                    break;
                case SNIPERSHOOTER:
                    newEnemy->viewDistance = 1000;
                    newEnemy->attackRange = 1000;
                    newEnemy->attackSpeed = 0.2f; // Ataques por segundo
                    newEnemy->entity.GRID[0] = PLAYER_GRID[0];
                    newEnemy->entity.GRID[1] = PLAYER_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = PLAYER_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = PLAYER_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.UPPER_IDLE_ROW = PLAYER_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = PLAYER_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = PLAYER_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = PLAYER_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = PLAYER_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = PLAYER_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = PLAYER_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = PLAYER_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = 0;
                    break;
                case DRONE:
                    newEnemy->viewDistance = 600;
                    newEnemy->attackRange = 200;
                    newEnemy->attackSpeed = 0.8f; // Ataques por segundo
                    newEnemy->entity.GRID[0] = PLAYER_GRID[0];
                    newEnemy->entity.GRID[1] = PLAYER_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = PLAYER_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = PLAYER_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.UPPER_IDLE_ROW = PLAYER_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = PLAYER_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = PLAYER_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = PLAYER_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = PLAYER_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = PLAYER_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = PLAYER_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = PLAYER_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = 0;
                    break;
                case TURRET:
                    newEnemy->viewDistance = 600;
                    newEnemy->attackRange = 200;
                    newEnemy->attackSpeed = 0.8f; // Ataques por segundo
                    newEnemy->entity.GRID[0] = PLAYER_GRID[0];
                    newEnemy->entity.GRID[1] = PLAYER_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = PLAYER_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = PLAYER_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.UPPER_IDLE_ROW = PLAYER_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = PLAYER_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = PLAYER_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = PLAYER_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = PLAYER_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = PLAYER_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = PLAYER_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = PLAYER_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = 0;
                    break;
                case BOSS:
                    newEnemy->viewDistance = 600;
                    newEnemy->attackRange = 200;
                    newEnemy->attackSpeed = 0.8f; // Ataques por segundo
                    newEnemy->entity.GRID[0] = PLAYER_GRID[0];
                    newEnemy->entity.GRID[1] = PLAYER_GRID[1];
                    newEnemy->entity.LEGS_IDLE_ROW = PLAYER_LEGS_IDLE_ROW;
                    newEnemy->entity.LEGS_IDLE_NUM_FRAMES = PLAYER_LEGS_IDLE_NUM_FRAMES;
                    newEnemy->entity.LEGS_WALKING_ROW = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.LEGS_WALKING_NUM_FRAMES = PLAYER_LEGS_WALKING_ROW;
                    newEnemy->entity.UPPER_IDLE_ROW = PLAYER_UPPER_IDLE_ROW;
                    newEnemy->entity.UPPER_IDLE_NUM_FRAMES = PLAYER_UPPER_IDLE_NUM_FRAMES;
                    newEnemy->entity.UPPER_WALKING_ROW = PLAYER_UPPER_WALKING_ROW;
                    newEnemy->entity.UPPER_WALKING_NUM_FRAMES = PLAYER_UPPER_WALKING_NUM_FRAMES;
                    newEnemy->entity.UPPER_ATTACKING_ROW = PLAYER_UPPER_ATTACKING_ROW;
                    newEnemy->entity.UPPER_ATTACKING_NUM_FRAMES = PLAYER_UPPER_ATTACKING_NUM_FRAMES;
                    newEnemy->entity.BODY_DYING_ROW = PLAYER_BODY_DYING_ROW;
                    newEnemy->entity.BODY_DYING_NUM_FRAMES = PLAYER_BODY_DYING_NUM_FRAMES;
                    newEnemy->pointsWorth = 0;
                    break;
                default:
                    break;
            }
        
            newEnemy->entity.lowerAnimation.animationFrameWidth = newEnemy->entity.GRID[0];
            newEnemy->entity.lowerAnimation.animationFrameHeight = newEnemy->entity.GRID[1];
            newEnemy->entity.lowerAnimation.currentAnimationFrameRect.width = newEnemy->entity.lowerAnimation.animationFrameWidth;
            newEnemy->entity.lowerAnimation.currentAnimationFrameRect.height = newEnemy->entity.lowerAnimation.animationFrameHeight;
            newEnemy->entity.upperAnimation.animationFrameWidth = newEnemy->entity.GRID[0];
            newEnemy->entity.upperAnimation.animationFrameHeight = newEnemy->entity.GRID[1];
            newEnemy->entity.upperAnimation.currentAnimationFrameRect.width = newEnemy->entity.upperAnimation.animationFrameWidth;
            newEnemy->entity.upperAnimation.currentAnimationFrameRect.height = newEnemy->entity.upperAnimation.animationFrameHeight;

            newEnemy->entity.drawableRect = (Rectangle) {position.x - width/2, position.y - height/2, width * newEnemy->entity.characterWidthScale, height * newEnemy->entity.characterHeightScale};
            newEnemy->entity.collisionBox = (Rectangle) {position.x - width/2, position.y - height/2, width * 0.8f, height};
            newEnemy->entity.collisionHead = (Circle) {(Vector2){position.x - width/2, position.y - height/2}, width * 0.8f};

            return;
        }
    }

}

void CreateBullet(Entity *entity, Bullet *bulletsPool, enum BULLET_TYPE bulletType, enum ENTITY_TYPES srcEntity) {
    // Procurar lugar vago na pool
    for (int i = 0; i < maxNumBullets; i++) {
        Bullet *bullet_i = bulletsPool + i;
        if (!bullet_i->isActive) {
            bullet_i->id = i;
            bullet_i->srcEntity = srcEntity;
            bullet_i->bulletType = bulletType;
            bullet_i->direction.x = entity->lowerAnimation.isFacingRight;
            bullet_i->direction.y = (entity->upPressed ? -1 : entity->downPressed ? 1 : 0);
            bullet_i->angle = (bullet_i->direction.y/bullet_i->direction.x == -1 ? -45 : bullet_i->direction.y/bullet_i->direction.x == 1 ? 45 : 0);
            
            float offset = 0;
            bullet_i->position.x = entity->position.x + bullet_i->direction.x * offset * entity->width;
            bullet_i->position.y = entity->position.y + entity->eyesOffset.y; // Ajustar TODO
            
            bullet_i->width = 20; // Tem que tunar
            bullet_i->height = 7; // Tem que tunar
            bullet_i->power = 40; // Tem que tunar
            bullet_i->lifeTime = 0;
            bullet_i->isActive = true;

            bullet_i->animation.animationFrameSpeed = 0.08f;
            bullet_i->animation.animationFrameWidth = MISC_GRID[0];
            bullet_i->animation.animationFrameHeight = MISC_GRID[1];
            bullet_i->animation.currentAnimationFrame = 0;
            bullet_i->animation.isFacingRight = bullet_i->direction.x;
            bullet_i->animation.timeSinceLastFrame = 0;
            bullet_i->animation.currentAnimationFrameRect.x = 0.0f;
            bullet_i->animation.currentAnimationFrameRect.y = 0.0f;
            bullet_i->animation.currentAnimationFrameRect.width = bullet_i->animation.animationFrameWidth;
            bullet_i->animation.currentAnimationFrameRect.height = bullet_i->animation.animationFrameHeight;

            bullet_i->drawableRect = (Rectangle) {bullet_i->position.x, bullet_i->position.y, abs(bullet_i->animation.currentAnimationFrameRect.width), abs(bullet_i->animation.currentAnimationFrameRect.height)};
            bullet_i->collisionBox = (Rectangle) {bullet_i->position.x - bullet_i->width/2, bullet_i->position.y - bullet_i->height/2, bullet_i->width, bullet_i->height};

            return;
        }
    }
}

void CreateGrenade(Entity *entity, Grenade *grenadePool, enum ENTITY_TYPES srcEntity) {
    // Procurar lugar vago na pool
    for (int i = 0; i < maxNumGrenade; i++) {
        Grenade *curGrenade = grenadePool + i;
        if (!curGrenade->isActive) {
            curGrenade->id = i;
            curGrenade->srcEntity = srcEntity;
            curGrenade->direction.x = entity->lowerAnimation.isFacingRight;
            curGrenade->direction.y = (entity->upPressed ? -1 : entity->downPressed ? 1 : 0);
            curGrenade->angle = GetRandomValue(0, 270);
            curGrenade->velocity = (Vector2) {curGrenade->direction.x*200 + entity->velocity.x/2,-200+entity->velocity.y};

            
            curGrenade->position.x = entity->position.x;
            curGrenade->position.y = entity->position.y;
            
            curGrenade->power = 40; // Tem que tunar
            curGrenade->lifeTime = 0;
            curGrenade->isActive = true;

            curGrenade->animation.animationFrameSpeed = 0.08f;
            curGrenade->animation.animationFrameWidth = MISC_GRID[0];
            curGrenade->animation.animationFrameHeight = MISC_GRID[1];
            curGrenade->animation.currentAnimationFrame = 0;
            curGrenade->animation.isFacingRight = curGrenade->direction.x;
            curGrenade->animation.timeSinceLastFrame = 0;
            curGrenade->animation.currentAnimationFrameRect.x = 0.0f;
            curGrenade->animation.currentAnimationFrameRect.y = 0.0f;
            curGrenade->animation.currentAnimationFrameRect.width = curGrenade->animation.animationFrameWidth;
            curGrenade->animation.currentAnimationFrameRect.height = curGrenade->animation.animationFrameHeight;

            curGrenade->drawableRect = (Rectangle) {curGrenade->position.x, curGrenade->position.y, abs(curGrenade->animation.currentAnimationFrameRect.width), abs(curGrenade->animation.currentAnimationFrameRect.height)};
            curGrenade->collisionCircle = (Circle) {(Vector2) {curGrenade->position.x, curGrenade->position.y}, 20};

            return;
        }
    }
}

int CreateGround(Ground *groundPool, Vector2 position, int width, int height, bool canBeStepped, bool followCamera, bool blockPlayer, bool isInvisible, bool isFromObject, enum OBJECTS_TYPES objType) {
    for (int i = 0; i < maxNumGrounds; i++) {
         Ground *curGround = groundPool + i;
         if (!curGround->isActive) {
             curGround->rect = (Rectangle) {position.x, position.y, width, height};
             curGround->canBeStepped = canBeStepped;
             curGround->followCamera = followCamera;
             curGround->blockPlayer = blockPlayer;
             curGround->isInvisible = isInvisible;
             curGround->isActive = true;
             curGround->isFromObject = isFromObject;
             curGround->objType = objType;

             return i;
        }
    }
}

void CreateParticle(Vector2 srcPosition, Vector2 velocity, Particle *particlePool, enum PARTICLE_TYPES type, float animTime, float angularVelocity, Vector2 scaleRange, bool isLoopable, int facingRight) {
    
    // Procurar lugar vago na pool
    for (int i = 0; i < maxNumParticles; i++) {
        Particle *curParticle = particlePool + i;
        if (!curParticle->isActive) {
            curParticle->id = i;
            curParticle->type = type;
            curParticle->position = srcPosition;
            curParticle->angle = 0;
            curParticle->isActive = true;
            curParticle->angularVelocity = angularVelocity;
            curParticle->scaleRange = scaleRange;
            curParticle->scale = 1;
            curParticle->scaleUp = true;
            curParticle->velocity = velocity;
            curParticle->isFacingRight = facingRight;

            switch (type)
            {
            case EXPLOSION:
                curParticle->frameRow = MISC_EXPLOSION_ROW;
                curParticle->numFrames = MISC_EXPLOSION_NUM_FRAMES;
                break;
            case SMOKE:
                curParticle->frameRow = MISC_SMOKE_EXPLOSION_ROW;
                curParticle->numFrames = MISC_SMOKE_EXPLOSION_NUM_FRAMES;
                break;
            case BLOOD_SPILL:
                curParticle->frameRow = MISC_BLOOD_SPILL_ROW;
                curParticle->numFrames = MISC_BLOOD_SPILL_NUM_FRAMES;
                break;
            case MAGNUM_SHOOT:
                curParticle->frameRow = MISC_BULLET_BLAZE_ROW;
                curParticle->numFrames = MISC_BULLET_BLAZE_NUM_FRAMES;
                break;
            default:
                break;
            }

            curParticle->animationFrameSpeed = 0.08f;
            if (isLoopable)
                curParticle->lifeTime = animTime;
            else
                curParticle->lifeTime = curParticle->animationFrameSpeed * curParticle->numFrames;
            curParticle->currentAnimationFrame = 0;
            curParticle->timeSinceLastFrame = 0;
            curParticle->frameRect.x = 0.0f;
            curParticle->frameRect.y = 0.0f;
            curParticle->frameRect.width = MISC_GRID[0];
            curParticle->frameRect.height = MISC_GRID[1];
            curParticle->width = MISC_GRID[0];
            curParticle->height = MISC_GRID[1];
            curParticle->loopAllowed = isLoopable;

            curParticle->drawableRect = (Rectangle) {curParticle->position.x, curParticle->position.y, abs(curParticle->frameRect.width), abs(curParticle->frameRect.height)};

            return;
        }
    }
}

void CreateMSG(Vector2 srcPosition, MSGSystem *msgPool, int value) {
    // Procurar lugar vago na pool
    for (int i = 0; i < maxNumMSGs; i++) {
        MSGSystem *curMsg = msgPool + i;
        if (!curMsg->isActive) {
            curMsg->id = i;
            curMsg->position = srcPosition;
            curMsg->position.y -= 60;
            curMsg->isActive = true;
            curMsg->lifeTime = 0;
            curMsg->msg = value;
            curMsg->color = RED;
            curMsg->colorId = 0;
            return;
        }
    }
}

void CreateEnvProp(EnvProps *envPropsPool, Ground *groundPool, enum OBJECTS_TYPES obType, Vector2 position, int width, int height) {
    for (int i = 0; i < maxNumEnvProps; i++) {
         EnvProps *curProp = envPropsPool + i;
         if (!curProp->isActive) {
             bool canBeStepped = false;
             bool followCamera = false;;
             bool blockPlayer = false;
             bool isInvisible = true; // o ground 
             int frameX = 0, frameY = 0, frameW = 0, frameH = 0;
             switch (obType)
             {
             case METAL_CRATE:
                frameX = OBJECTS_METAL_CRATE[0];
                frameY = OBJECTS_METAL_CRATE[1];
                curProp->type = obType;
                curProp->collisionRect = (Rectangle) {position.x, position.y, width, height};
                curProp->pointsWorth = 0;
                curProp->isDestroyable = false;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case AMMO_CRATE:
                frameX = OBJECTS_AMMO_CRATE[0];
                frameY = OBJECTS_AMMO_CRATE[1];
                curProp->collisionRect = (Rectangle) {position.x + 0.1f*width, position.y + 0.1f*height, 0.75f*width, 0.8f*height};
                curProp->pointsWorth = PTS_COLLECT_AMMO;
                curProp->isDestroyable = false;
                curProp->isCollectable = true;
                canBeStepped = false;
                blockPlayer = false;
                width *= 0.9f;
                height *= 0.9f;
                 break;
             case HP_CRATE:
                frameX = OBJECTS_HP_CRATE[0];
                frameY = OBJECTS_HP_CRATE[1];
                curProp->collisionRect = (Rectangle) {position.x + 0.1f*width, position.y + 0.1f*height, 0.75f*width, 0.8f*height};
                curProp->pointsWorth = PTS_COLLECT_HP;
                curProp->isDestroyable = false;
                curProp->isCollectable = true;
                canBeStepped = false;
                blockPlayer = false;
                width *= 0.9f;
                height *= 0.9f;
                 break;
            case CARD_CRATE1:
            //130 x 130 parece bom
                frameX = OBJECTS_CARD_CRATE1[0];
                frameY = OBJECTS_CARD_CRATE1[1];
                curProp->collisionRect = (Rectangle) {position.x + 0.15f * width, position.y + 0.3f * height, 0.7f * width, 0.7f * height};
                curProp->pointsWorth = PTS_DESTROY_CARD_CRATE;
                curProp->isDestroyable = true;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case CARD_CRATE2:
            //130 x 130 parece bom
                frameX = OBJECTS_CARD_CRATE2[0];
                frameY = OBJECTS_CARD_CRATE2[1];
                curProp->collisionRect = (Rectangle) {position.x + 0.15f * width, position.y + 0.3f * height, 0.7f * width, 0.7f * height};
                curProp->pointsWorth = PTS_DESTROY_CARD_CRATE;
                curProp->isDestroyable = true;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case CARD_CRATE3:
            //130 x 130 parece bom
                frameX = OBJECTS_CARD_CRATE3[0];
                frameY = OBJECTS_CARD_CRATE3[1];
                curProp->collisionRect = (Rectangle) {position.x, position.y + height/2, width, height/2};
                curProp->pointsWorth = PTS_DESTROY_CARD_CRATE;
                curProp->isDestroyable = true;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case TRASH_BIN:
                frameX = OBJECTS_TRASH_BIN[0];
                frameY = OBJECTS_TRASH_BIN[1];
                curProp->collisionRect = (Rectangle) {position.x+0.2f*width, position.y+0.08f*height, 0.6f*width, 0.9f*height};
                curProp->isDestroyable = false;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case EXPLOSIVE_BARREL:
                frameX = OBJECTS_EXPLOSIVE_BARREL[0];
                frameY = OBJECTS_EXPLOSIVE_BARREL[1];
                curProp->collisionRect = (Rectangle) {position.x+0.2f*width, position.y+0.08f*height, 0.6f*width, 0.9f*height};
                curProp->pointsWorth = PTS_DESTROY_EXPLOSIVE_BARREL;
                curProp->isDestroyable = true;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case METAL_BARREL:
             // 140x140 parece bom
                frameX = OBJECTS_METAL_BARREL[0];
                frameY = OBJECTS_METAL_BARREL[1];
                curProp->collisionRect = (Rectangle) {position.x+0.15f*width, position.y, 0.7f*width, height};
                curProp->isDestroyable = false;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             case GARBAGE_BAG1:
             //100 x100
                frameX = OBJECTS_GARBAGE_BAG1[0];
                frameY = OBJECTS_GARBAGE_BAG1[1];
                curProp->collisionRect = (Rectangle) {position.x+0.2f*width, position.y+0.08f*height, 0.6f*width, 0.9f*height};
                curProp->pointsWorth = PTS_DESTROY_GARBAGE_BAG;
                curProp->isDestroyable = true;
                curProp->isCollectable = false;
                canBeStepped = false;
                blockPlayer = false;
                 break;
             case GARBAGE_BAG2:
             // 80 x 80
                frameX = OBJECTS_GARBAGE_BAG2[0];
                frameY = OBJECTS_GARBAGE_BAG2[1];
                curProp->collisionRect = (Rectangle) {position.x+0.2f*width, position.y+0.08f*height, 0.6f*width, 0.9f*height};
                curProp->pointsWorth = PTS_DESTROY_GARBAGE_BAG;
                curProp->isDestroyable = true;
                curProp->isCollectable = false;
                canBeStepped = false;
                blockPlayer = false;
                 break;
             case TRASH_CONTAINER:
              // 220 x 220  
                frameX = OBJECTS_TRASH_CONTAINER[0];
                frameY = OBJECTS_TRASH_CONTAINER[1];
                curProp->collisionRect = (Rectangle) {position.x+0.1f*width, position.y+0.4f*height, 0.85f*width, 0.6f*height};
                curProp->pointsWorth = 0;
                curProp->isDestroyable = false;
                curProp->isCollectable = false;
                canBeStepped = true;
                blockPlayer = false;
                 break;
             default:
                 break;
             }
            frameW = OBJECTS_GRID[0];
            frameH = OBJECTS_GRID[1];
            curProp->id = i;
            curProp->type = obType;
            curProp->frameRect = (Rectangle) {frameX * frameW, frameY * frameH, frameW, frameH};
            curProp->groundID = CreateGround(groundPool, (Vector2){curProp->collisionRect.x, curProp->collisionRect.y}, curProp->collisionRect.width, curProp->collisionRect.height, canBeStepped, followCamera, blockPlayer, isInvisible, true, obType);
            curProp->drawableRect = (Rectangle) {position.x, position.y, width, height};
            curProp->isActive = true;

             return;
        }
    }
}

Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom) {
    Camera2D newCam = {0};
    newCam.target = target;
    newCam.offset = offset;
    newCam.rotation = rotation;
    newCam.zoom = zoom;

    return newCam;
}

void DestroyEnvProp(Player *player, Enemy *enemyPool,EnvProps *envPropsPool, Ground *groundsPool, Particle *particlePool, Sound *soundPool, MSGSystem *msgSystem, int envPropID, int difficulty) {
    EnvProps *envProp;
    if (envPropID != -1) 
        envProp = envPropsPool + envPropID;
    else
        envProp = envPropsPool + 0;

    int groundId = envProp->groundID;
    Ground *ground = groundsPool + groundId;
    ground->isActive = false;
    envProp->isActive = false;
    if (envPropID != -1) {
        player->points += envProp->pointsWorth;
        CreateMSG((Vector2) {envProp->drawableRect.x+envProp->drawableRect.width/2, envProp->drawableRect.y}, msgSystem, envProp->pointsWorth);
        if (envProp->type == EXPLOSIVE_BARREL) {
            ExplosionAOE(player, msgSystem, envProp, enemyPool, ground, particlePool, soundPool, 150, 150, (Vector2) {envProp->drawableRect.x+envProp->drawableRect.width/2, envProp->drawableRect.y+envProp->drawableRect.height/2}, PLAYER, difficulty);
            PlaySoundMulti(soundPool[FX_GRENADE_EXPLOSION]);
            CreateParticle((Vector2) {envProp->drawableRect.x+envProp->drawableRect.width/2, envProp->drawableRect.y+envProp->drawableRect.height/2}, (Vector2) {0, 0}, particlePool, SMOKE, 4, 0, (Vector2) {1, 1}, false, 1);
            CreateParticle((Vector2) {envProp->drawableRect.x+envProp->drawableRect.width/2, envProp->drawableRect.y+envProp->drawableRect.height/2}, (Vector2) {0, 0}, particlePool, EXPLOSION, 4, 0, (Vector2) {1, 1}, false, 1);
        } else if (GetRandomValue(1,100) <= fmin(difficulty*0.25f, 4)) {  // 2% de chance de dropar ammo ou hp
            CreateEnvProp(envPropsPool, groundsPool, (GetRandomValue(1,2) == 1 ? AMMO_CRATE : HP_CRATE), (Vector2) {envProp->drawableRect.x, envProp->drawableRect.y}, 130, 130);
        }
        
    }
}

void UpdateDifficulty(int *difficulty, float minX, float time) {
    *difficulty = (int) ((minX + 10*time)/(7*screenWidth));
}

void UpdatePlayer(Player *player, Enemy *enemy, Bullet *bulletPool, Grenade *grenadePool, float delta, Ground *ground, EnvProps *envProps, Particle *particlePool, Sound *soundPool, MSGSystem *msgSystem, float minX, int difficulty) {
    enum CHARACTER_STATE currentLowerState = player->entity.lowerAnimation.currentAnimationState;
    enum CHARACTER_STATE currentUpperState = player->entity.upperAnimation.currentAnimationState;
    player->entity.lowerAnimation.timeSinceLastFrame += delta;
    player->entity.upperAnimation.timeSinceLastFrame += delta;

    if (player->entity.lowerAnimation.currentAnimationState != DEAD ) {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler de input do player                                     ///////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (player->entity.lowerAnimation.currentAnimationState != DYING ) {
            
            // Registro das teclas "up" e "down". A tecla "up" tem prioridade sobre a "down" por convenção
            player->entity.upPressed = false;
            player->entity.downPressed = false;
            if (IsKeyDown(KEY_UP)) {
                player->entity.upPressed = true;
            } else if (IsKeyDown(KEY_DOWN)) {
                player->entity.downPressed = true;
            }

            if (IsKeyDown(KEY_LEFT)) {
                player->entity.velocity.x -= player->entity.maxXSpeed;
                
            } else if (IsKeyDown(KEY_RIGHT)) {
                player->entity.velocity.x += player->entity.maxXSpeed;
            } else {
                player->entity.velocity.x = 0;
            }

            if (IsKeyDown(KEY_SPACE) && player->entity.isGrounded) 
            {
                player->entity.velocity.y = -2*player->entity.jumpSpeed;
                player->entity.isGrounded = false;
            }

            if (IsKeyPressed(KEY_T)) {
                if (player->entity.grenadeAmmo > 0) {
                    if (player->entity.upperAnimation.currentAnimationState != THROWING || (player->entity.upperAnimation.currentAnimationState == THROWING && player->entity.upperAnimation.currentAnimationFrame > 3)) {
                        CreateGrenade(&(player->entity), grenadePool, PLAYER);
                        PlaySoundMulti(soundPool[FX_GRENADE_LAUNCH]);
                        player->entity.grenadeAmmo--;
                        player->entity.upperAnimation.currentAnimationState = THROWING;
                        player->entity.upperAnimation.currentAnimationFrame = 0;
                        player->entity.upperAnimation.timeSinceLastFrame = 0;
                    }
                }
            }

            if (IsKeyPressed(KEY_R)) {
                if (player->entity.magnumAmmo > 0) {
                    if (player->entity.upperAnimation.currentAnimationState != ATTACKING || (player->entity.upperAnimation.currentAnimationState == ATTACKING && player->entity.upperAnimation.currentAnimationFrame > 1)) {
                        CreateBullet(&(player->entity), bulletPool, MAGNUM, PLAYER);
                        PlaySoundMulti(soundPool[FX_MAGNUM]);
                        player->entity.magnumAmmo--;
                        player->entity.upperAnimation.currentAnimationState = ATTACKING;
                        player->entity.upperAnimation.currentAnimationFrame = 0;
                        player->entity.upperAnimation.timeSinceLastFrame = 0;
                    }
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler de colisão do player                                   ///////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        EntityCollisionHandler(player, &(player->entity), enemy, ground, envProps, particlePool, soundPool, msgSystem, delta, difficulty);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler de física e gráfico do player                          ///////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        PhysicsAndGraphicsHandlers(&(player->entity), delta, currentLowerState, currentUpperState, PLAYER, BOSS);

        // Limitar posição do player de acordo com o avanço da câmera
        if (player->entity.position.x < minX + player->entity.width/2) {
            player->entity.position.x = minX + player->entity.width/2;
        } 

        // Atualizar caixa de colisão e retângulo de desenho
        player->entity.drawableRect = (Rectangle) {player->entity.position.x, player->entity.position.y, (int)(player->entity.width * player->entity.characterWidthScale), (int)(player->entity.height * player->entity.characterHeightScale)};
        player->entity.collisionBox = (Rectangle) {player->entity.position.x  - player->entity.width/2 + (player->entity.lowerAnimation.isFacingRight == -1 ? 0.43f : 0.23f) * player->entity.width, player->entity.position.y - player->entity.height/2, player->entity.width * 0.35f, player->entity.height};
        player->entity.collisionHead = (Circle) {(Vector2){player->entity.position.x - player->entity.lowerAnimation.isFacingRight * 0.1f * player->entity.width, player->entity.position.y - 0.15f * player->entity.height}, player->entity.width * 0.2f};
        
        // Caixa se morto
        if (player->entity.lowerAnimation.currentAnimationState == DYING && currentLowerState != DYING)
            PlaySoundMulti(soundPool[FX_DYING]);
        if (player->entity.lowerAnimation.currentAnimationState == DYING) {
            player->entity.collisionBox = (Rectangle) {player->entity.position.x  - player->entity.width + (player->entity.lowerAnimation.isFacingRight == -1 ? 0.43f : 0.23f) * player->entity.width, player->entity.position.y, player->entity.width, player->entity.height/2};
        }
    }
}

void UpdateEnemy(Enemy *enemy, Player *player, Bullet *bulletPool, float delta, Ground *ground, EnvProps *envProps, Sound *soundPool, Particle *particlePool, MSGSystem *msgSystem, int minX, int difficulty) {
    Entity *eEnt = &(enemy->entity);
    enum CHARACTER_STATE currentLowerState = eEnt->lowerAnimation.currentAnimationState;
    enum CHARACTER_STATE currentUpperState = eEnt->upperAnimation.currentAnimationState;
    eEnt->upperAnimation.timeSinceLastFrame += delta;
    eEnt->lowerAnimation.timeSinceLastFrame += delta;
    enemy->timeSinceLastBehaviorChange += delta;
    enemy->timeSinceLastAttack += delta;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de comportamento do enemy                              ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    SteeringBehavior(enemy, player, &(player->entity), bulletPool, soundPool, particlePool, delta, enemy->class);
    

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de colisão do enemy                                    ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EntityCollisionHandler(player, &(enemy->entity), enemy, ground, envProps, particlePool, soundPool, msgSystem, delta, difficulty);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de física e gráfico do enemy                           ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsAndGraphicsHandlers(&(enemy->entity), delta, currentLowerState, currentUpperState, ENEMY, enemy->class);

    eEnt->drawableRect = (Rectangle) {eEnt->position.x, eEnt->position.y, eEnt->width * eEnt->characterWidthScale,eEnt->height * eEnt->characterHeightScale};
    eEnt->collisionBox = (Rectangle) {eEnt->position.x  - eEnt->width/2 + (eEnt->lowerAnimation.isFacingRight == -1 ? 0.3f : 0.15f) * eEnt->width, eEnt->position.y - eEnt->height/2, eEnt->width * 0.5f, eEnt->height};
    eEnt->collisionHead = (Circle) {(Vector2){eEnt->position.x - eEnt->lowerAnimation.isFacingRight * 0.1f * eEnt->width, eEnt->position.y - 0.15f * eEnt->height}, eEnt->width * 0.2f};

    // Handler pós-morte
    if (eEnt->lowerAnimation.currentAnimationState == DYING && currentLowerState != DYING)
        PlaySoundMulti(soundPool[FX_DYING]);
    if (eEnt->lowerAnimation.currentAnimationState == DYING) {
        eEnt->collisionBox = (Rectangle) {eEnt->position.x  - eEnt->width + (eEnt->lowerAnimation.isFacingRight == -1 ? 0.43f : 0.23f) * eEnt->width, eEnt->position.y, eEnt->width, eEnt->height/2};
        eEnt->timeSinceDeath+=delta;
        if (eEnt->timeSinceDeath >= corpseTime) {
            enemy->isAlive = false;
        }
    }
}
     
void UpdateBullets(Bullet *bullet, Enemy *enemyPool, Player *player, MSGSystem *msgSystem, Ground *groundsPool, EnvProps *envPropsPool, Sound *soundPool, Particle *particlePool, float delta, int maxX, int difficulty) {
    bullet->lifeTime += delta;
    bullet->animation.timeSinceLastFrame += delta;
    // Checar colisão
    // Grounds
    for (int i = 0; i < maxNumGrounds; i++)
    {
        Ground *curGround = groundsPool + i;
        if (curGround->isActive) {
            if (CheckCollisionRecs(curGround->rect, bullet->collisionBox)) {
                for (int j = 0; j < maxNumEnvProps; j++) {
                    EnvProps *curProp = envPropsPool + j;
                    if (curProp->isActive) {
                        if (CheckCollisionRecs(curProp->collisionRect, bullet->collisionBox)) {
                            bullet->isActive = false;
                            if (curProp->isDestroyable) {
                                if (GetRandomValue(1,3) == 1) { 
                                    DestroyEnvProp(player, enemyPool, envPropsPool, groundsPool, particlePool, soundPool, msgSystem, curProp->id, difficulty);
                                    if (curProp->type == EXPLOSIVE_BARREL) {
                                        ExplosionAOE(player, msgSystem, envPropsPool, enemyPool, groundsPool, particlePool, soundPool, 150, 150, (Vector2) {curProp->drawableRect.x+curProp->drawableRect.width/2, curProp->drawableRect.y+curProp->drawableRect.height/2}, PLAYER, difficulty);
                                        PlaySoundMulti(soundPool[FX_GRENADE_EXPLOSION]);
                                        CreateParticle((Vector2) {curProp->drawableRect.x+curProp->drawableRect.width/2, curProp->drawableRect.y+curProp->drawableRect.height/2}, (Vector2) {0, 0}, particlePool, SMOKE, 4, 0, (Vector2) {1, 1}, false, 1);
                                        CreateParticle((Vector2) {curProp->drawableRect.x+curProp->drawableRect.width/2, curProp->drawableRect.y+curProp->drawableRect.height/2}, (Vector2) {0, 0}, particlePool, EXPLOSION, 4, 0, (Vector2) {1, 1}, false, 1);
                                    } else {
                                        if (GetRandomValue(1,100) <= fmin(difficulty*0.25f, 4)) {  // 2% de chance de dropar ammo ou hp
                                            CreateEnvProp(envPropsPool, groundsPool, (GetRandomValue(1,2) == 1 ? AMMO_CRATE : HP_CRATE), (Vector2) {curProp->drawableRect.x, curProp->drawableRect.y}, 130, 130);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // Props

    // Colisão com inimigos
    if (bullet->srcEntity == PLAYER) {
        for (int i = 0; i < maxNumEnemies; i++)
        {
            Enemy *currentEnemy = enemyPool + i;
            if (currentEnemy->isAlive) {
                if (currentEnemy->entity.lowerAnimation.currentAnimationState != DYING) {
                    if (CheckCollisionRecs(currentEnemy->entity.collisionBox, bullet->collisionBox) || CheckCollisionCircleRec(currentEnemy->entity.collisionHead.center, currentEnemy->entity.collisionHead.radius, bullet->collisionBox)) {
                        CreateParticle(currentEnemy->entity.position, (Vector2) {0,0}, particlePool, BLOOD_SPILL, 2.5f, 0, (Vector2){1,1}, false, bullet->direction.x);
                        bullet->isActive = false;
                        currentEnemy->entity.lowerAnimation.isFacingRight = -bullet->direction.x;
                        HurtEntity(&(currentEnemy->entity), soundPool, 50); // TODO damage
                        if (currentEnemy->entity.currentHP <= 0) {
                            KillEnemy(player, currentEnemy, msgSystem);
                        }
                    }
                }
            }
        }
    }

    // Colisão com Player
    if (bullet->srcEntity == ENEMY) {
        if (CheckCollisionRecs(player->entity.collisionBox, bullet->collisionBox) || CheckCollisionCircleRec(player->entity.collisionHead.center, player->entity.collisionHead.radius, bullet->collisionBox)) {
            bullet->isActive = false;
            HurtEntity(&(player->entity), soundPool, 20); // TODO damage
            CreateParticle(player->entity.position, (Vector2) {0,0}, particlePool, BLOOD_SPILL, 2.5f, 0, (Vector2){1,1}, false, bullet->direction.x);
            // TODO Causa dano ao player
            // TODO Criar animação de sangue
        }
    }

    // Se não tiver colisão, checar tempo de vida e atualizar posição
    int vel = 2200;
    if (bullet->lifeTime >= bulletLifeTime) {
        bullet->isActive = false;
    } else {
        if (bullet->position.x + bullet->animation.animationFrameWidth > maxX) {
            bullet->isActive = false;
            return;
        }
        if (bullet->angle == 0)
            bullet->position.x += vel * delta * bullet->direction.x;
        else {
            bullet->position.x += vel/1.41f * delta * bullet->direction.x;
            bullet->position.y += vel/1.41f * delta * bullet->direction.y;
        }
        // TODO fazer partícula
        // Animação
        if (bullet->animation.timeSinceLastFrame >= bullet->animation.animationFrameSpeed) {
            bullet->animation.timeSinceLastFrame = 0.0f;
            bullet->animation.currentAnimationFrame++;
            if (bullet->animation.currentAnimationFrame > MISC_BULLET_NUM_FRAMES - 1) {  // 1 porque são 2 frames
                bullet->animation.currentAnimationFrame = 0;
            }
        }
        
        bullet->animation.currentAnimationFrameRect.x = bullet->animation.currentAnimationFrame * bullet->animation.animationFrameWidth;
        bullet->animation.currentAnimationFrameRect.y = MISC_BULLET_ROW * bullet->animation.animationFrameHeight;
        bullet->animation.currentAnimationFrameRect.width = bullet->animation.isFacingRight * bullet->animation.animationFrameWidth;

        bullet->drawableRect = (Rectangle) {bullet->position.x, bullet->position.y, abs(bullet->animation.currentAnimationFrameRect.width), abs(bullet->animation.currentAnimationFrameRect.height)};
        int offsetX = 1;
        int offsetY = 1;
        
        if (bullet->direction.x == -1) {// pra esquerda
            if (bullet->direction.y == -1) { // pra cima
                offsetX = -abs(bullet->animation.animationFrameHeight/4);
                offsetY = -abs(bullet->animation.animationFrameHeight/4) - 12;
            } else if (bullet->direction.y == 1) { // pra baixo
                offsetX = -abs(bullet->animation.animationFrameHeight/2);
                offsetY = abs(bullet->animation.animationFrameHeight/4);
            } else { // reto
                offsetX = -abs(bullet->animation.animationFrameWidth) + 60;
                offsetY = -12;
            }
        } else { // pra direita
            if (bullet->direction.y == -1) { // pra cima
                offsetX = abs(bullet->animation.animationFrameHeight/4);
                offsetY = -abs(bullet->animation.animationFrameHeight/4) - 12;
            } else if (bullet->direction.y == 1) { // pra baixo
                offsetX = abs(bullet->animation.animationFrameHeight/4);
                offsetY = abs(bullet->animation.animationFrameHeight/4);
            } else { // reto
                offsetX = 40;
                offsetY = -12;
            }
        }
        bullet->collisionBox = (Rectangle) {bullet->position.x + offsetX, bullet->position.y + offsetY, bullet->width, bullet->height};
    }

}

void UpdateGrenades(Grenade *grenade, Enemy *enemy, Player *player, MSGSystem *msgSystem, Ground *ground, EnvProps *envProp, Particle *particlePool, Sound *soundPool, float delta, int difficulty) {
    grenade->lifeTime += delta;
    grenade->animation.timeSinceLastFrame += delta;
    grenade->angle += 5;
    Vector2 futureCenter = (Vector2) {grenade->collisionCircle.center.x + grenade->velocity.x * delta, grenade->collisionCircle.center.y + grenade->velocity.y * delta};

    // Checar colisão
    // Grounds
    for (int i = 0; i < maxNumGrounds; i++) {
        Ground *curGround = ground + i;
        int collisionThreshold = 5;
        if (curGround->isActive) {
            if (CheckCollisionCircleRec(futureCenter, grenade->collisionCircle.radius, curGround->rect)) {
                if (curGround->objType == -1) {
                    PlaySoundMulti(soundPool[FX_GRENADE_BOUNCING]);
                    if (grenade->collisionCircle.center.x + grenade->collisionCircle.radius - curGround->rect.x <= collisionThreshold || grenade->collisionCircle.center.x - grenade->collisionCircle.radius - curGround->rect.x - curGround->rect.width >= -collisionThreshold) {
                        grenade->velocity.x *= -0.6f;
                    }
                    if (grenade->collisionCircle.center.y + grenade->collisionCircle.radius - curGround->rect.y <= collisionThreshold || grenade->collisionCircle.center.y - grenade->collisionCircle.radius - curGround->rect.y - curGround->rect.height >= -collisionThreshold) {
                        grenade->velocity.y *= -0.6f;
                    }
                }
            }
        }
    }
    // Props
    // Colisão com inimigos
    if (grenade->srcEntity == PLAYER) {
        for (int i = 0; i < maxNumEnemies; i++)
        {
            Enemy *currentEnemy = enemy + i;
            if (currentEnemy->isAlive) {
                if (currentEnemy->entity.lowerAnimation.currentAnimationState != DYING) {
                    if (CheckCollisionCircleRec(futureCenter, grenade->collisionCircle.radius, currentEnemy->entity.collisionBox) || CheckCollisionCircles(currentEnemy->entity.collisionHead.center, currentEnemy->entity.collisionHead.radius, futureCenter, grenade->collisionCircle.radius)) {
                        grenade->isActive = false;
                        Vector2 particlePosition = grenade->position;
                        particlePosition.y -= grenade->drawableRect.height/2;
                        PlaySoundMulti(soundPool[FX_GRENADE_EXPLOSION]);
                        ExplosionAOE(player, msgSystem, envProp, enemy, ground, particlePool, soundPool, 100, 100, grenade->position, PLAYER, difficulty);
                        CreateParticle(grenade->position, (Vector2) {0, 0}, particlePool, SMOKE, 4, 0, (Vector2) {1, 1}, false, 1);
                        CreateParticle(grenade->position, (Vector2) {0, 0}, particlePool, EXPLOSION, 4, 0, (Vector2) {1, 1}, false, 1);
                    }
                }
            }
        }
    }

    // Se não tiver colisão, checar tempo de vida e atualizar posição
    if (grenade->lifeTime >= grenadeExplosionTime) {
        grenade->isActive = false;
        Vector2 particlePosition = grenade->position;
        particlePosition.y -= grenade->drawableRect.height/2;
        ExplosionAOE(player, msgSystem, envProp, enemy, ground, particlePool, soundPool, 100, 100, grenade->position, PLAYER, difficulty);
        PlaySoundMulti(soundPool[FX_GRENADE_EXPLOSION]);
        CreateParticle(particlePosition, (Vector2) {0, 0}, particlePool, SMOKE, 4, 0, (Vector2) {1, 1}, false, 1);
        CreateParticle(grenade->position, (Vector2) {0, 0}, particlePool, EXPLOSION, 4, 0, (Vector2) {1, 1}, false, 1);

    } else {
        grenade->velocity.y += delta * GRAVITY;
        grenade->position.x += delta * grenade->velocity.x;
        grenade->position.y += delta * grenade->velocity.y;
    }
    // TODO fazer partícula
    // Animação
    if (grenade->animation.timeSinceLastFrame >= grenade->animation.animationFrameSpeed) {
        grenade->animation.timeSinceLastFrame = 0.0f;
        grenade->animation.currentAnimationFrame++;
        if (grenade->animation.currentAnimationFrame > MISC_GRANADE_NUM_FRAMES - 1) {
            grenade->animation.currentAnimationFrame = 0;
        }
    }
    
    grenade->animation.currentAnimationFrameRect.x = (float)grenade->animation.currentAnimationFrame * grenade->animation.animationFrameWidth;
    grenade->animation.currentAnimationFrameRect.y = MISC_GRANADE_ROW * grenade->animation.animationFrameHeight;
    grenade->animation.currentAnimationFrameRect.width = grenade->animation.isFacingRight * grenade->animation.animationFrameWidth;

    grenade->drawableRect = (Rectangle) {grenade->position.x, grenade->position.y, abs(grenade->animation.currentAnimationFrameRect.width), abs(grenade->animation.currentAnimationFrameRect.height)};
    grenade->collisionCircle = (Circle) {(Vector2) {grenade->position.x, grenade->position.y}, 20};

}

void UpdateClampedCameraPlayer(Camera2D *camera, Player *player, float delta, int width, int height, float *minX, float *maxX) {
    // Dinamismo da câmera... zoomOut tá fazendo a tela andar indefinidamente
    /*if (player->entity.velocity.y != 0) //|| player->entity.velocity.y != 0)
        camera->zoom -= delta/20;
    else
        camera->zoom += delta/20;
    
    if (camera->zoom > 1) camera->zoom = 1;
    if (camera->zoom < 0.96f) camera->zoom = 0.96f;*/

    camera->target = player->entity.position;
    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    float minY = 0.00f, maxY = height; // Camera clamp controls. TODO, maxX

    *minX = fmaxf(0.0f, *minX);
    *maxX = fmaxf(4*width, *maxX);

    Vector2 max = GetWorldToScreen2D((Vector2){ *maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ *minX, minY }, *camera);
    
    if (max.x < width) camera->offset.x = width - (max.x - width/2);
    if (max.y < height) camera->offset.y = height - (max.y - height/2);
    if (min.x > 0) camera->offset.x = width/2 - min.x;

}

void UpdateGrounds(Player *player, Ground *ground, float delta, float minX) {
    if (ground->followCamera)
        ground->rect.x = minX;

    if (ground->rect.x + ground->rect.width < minX) 
        ground->isActive = false;
}

void UpdateEnvProps(Player *player, Enemy *enemyPool, EnvProps *envPropsPool, Ground *groundsPool, Particle *particlePool, Sound *soundPool, MSGSystem *msgSystem, float delta, float minX) {
    if (envPropsPool->drawableRect.x + envPropsPool->drawableRect.width < minX) 
        DestroyEnvProp(&player, enemyPool, envPropsPool, groundsPool, particlePool, soundPool, msgSystem, -1, -1);
}

void UpdateParticles(Particle *curParticle, float delta, float minX) {
    if (curParticle->isActive) {
        if (curParticle->drawableRect.x + curParticle->drawableRect.width < minX)  {
            curParticle->isActive = false;
        }

        curParticle->lifeTime += delta;
        curParticle->timeSinceLastFrame += delta;

        curParticle->position.x += curParticle->velocity.x * delta;
        curParticle->position.y += curParticle->velocity.y * delta;
        curParticle->angle += curParticle->angularVelocity * delta;
        if (curParticle->scaleUp) {
            curParticle->scale++;
            if (curParticle->scale >= curParticle->scaleRange.y) {
                curParticle->scale = curParticle->scaleRange.y;
                curParticle->scaleUp = false;
            }
        } else {
            curParticle->scale--;
            if (curParticle->scale <= curParticle->scaleRange.x) {
                curParticle->scale = curParticle->scaleRange.x;
                curParticle->scaleUp = true;
            }
        }

        // TODO fazer partícula
        // Animação
        if (curParticle->timeSinceLastFrame >= curParticle->animationFrameSpeed) {
            curParticle->timeSinceLastFrame = 0.0f;
            curParticle->currentAnimationFrame++;
            if (curParticle->currentAnimationFrame > curParticle->numFrames - 1) { 
                if (curParticle->loopAllowed) {
                    curParticle->currentAnimationFrame = 0;
                } else {
                    curParticle->isActive = false;
                }
            }
        }
    
        curParticle->frameRect.x = curParticle->currentAnimationFrame * curParticle->width;
        curParticle->frameRect.y = curParticle->frameRow * curParticle->height;
        curParticle->frameRect.width = curParticle->width * curParticle->isFacingRight;

        curParticle->drawableRect = (Rectangle) {curParticle->position.x, curParticle->position.y, curParticle->width * curParticle->scale, curParticle->height * curParticle->scale};
    }
}

void UpdateMSGs(MSGSystem *curMsg, float delta) {
    curMsg->lifeTime += delta;
    curMsg->position.y -= delta*50;

    int time = (int)(curMsg->lifeTime*10);
        curMsg->colorId = time % 4;

    if (curMsg->lifeTime > msgTime)  {
        curMsg->isActive = false;
    }

    switch (curMsg->colorId)
    {
    case 0:
        curMsg->color = RED;
        break;
    case 1:
        curMsg->color = GREEN;
        break;
    case 2:
        curMsg->color = YELLOW;
        break;
    case 3:
        curMsg->color = BLUE;
        curMsg->colorId = 0;
        break;
    }


}

void UpdateBackground(Player *player, Background *backgroundPool, int i, Texture2D srcAtlas, Enemy *enemyPool, EnvProps *envPropsPool, Ground *groundPool, float delta, int *numBackground, float minX, float *maxX, int difficulty) {
    Background *bgP = backgroundPool + i;
    bgP->position.x = (bgP->originalX - minX*bgP->relativePosition);
    if (bgP->position.x+bgP->width < minX) {
        //"Deletar" bg e criar um novo
        BeginTextureMode(bgP->canvas);
        ClearBackground(GetColor(0x052c46ff));
        EndTextureMode();
        UnloadRenderTexture(bgP->canvas);
        *bgP = CreateBackground(player, enemyPool, envPropsPool, backgroundPool, groundPool, srcAtlas, bgP->bgType, numBackground, i, difficulty);
        if (*maxX <= bgP->position.x + bgP->width)
            *maxX = bgP->position.x + bgP->width;
    }
}

void DrawEnemy(Enemy *enemy, Texture2D *texture, bool drawDetectionCollision, bool drawLife, bool drawCollisionBox) {
    // Draw campo de visão
    if (drawDetectionCollision) {
        float eyesX = enemy->entity.position.x + enemy->entity.eyesOffset.x;
        float eyesY = enemy->entity.position.y + enemy->entity.eyesOffset.y;
        Rectangle detectionBox;
        if (enemy->entity.lowerAnimation.isFacingRight == 1) {
            detectionBox = (Rectangle){eyesX, eyesY, enemy->viewDistance, 5};
        } else {
            detectionBox = (Rectangle){eyesX-enemy->viewDistance, eyesY, enemy->viewDistance, 5};
        }
        DrawRectangle(detectionBox.x, detectionBox.y, detectionBox.width, detectionBox.height, RED);
    }

    // Draw vida acima de cada inimigo
    if (drawLife) {
        char state[30];
        sprintf(state, "%d", enemy->entity.currentHP);
        DrawText(state, enemy->entity.position.x, enemy->entity.position.y - 25, 20, RED);
    }

    // Draw das caixas de colisão
    if (drawCollisionBox) {
        DrawRectangle(enemy->entity.collisionBox.x, enemy->entity.collisionBox.y, enemy->entity.collisionBox.width, enemy->entity.collisionBox.height, WHITE);
        DrawCircle(enemy->entity.collisionHead.center.x, enemy->entity.collisionHead.center.y, enemy->entity.collisionHead.radius, YELLOW);
    }

    // Draw inimigos
    DrawTexturePro(texture[enemy->class], enemy->entity.lowerAnimation.currentAnimationFrameRect, enemy->entity.drawableRect, (Vector2) {enemy->entity.width/2, enemy->entity.height/2}, 0, WHITE);
    DrawTexturePro(texture[enemy->class], enemy->entity.upperAnimation.currentAnimationFrameRect, enemy->entity.drawableRect, (Vector2) {enemy->entity.width/2, enemy->entity.height/2}, 0, WHITE);
}

void DrawBullet(Bullet *bullet, Texture2D texture, bool drawCollisionBox) {
    Vector2 origin = (Vector2) {122/2, 122/2};
    DrawTexturePro(texture, bullet->animation.currentAnimationFrameRect, bullet->drawableRect, origin, bullet->angle, WHITE);
    // Draw das caixas de colisão
    if (drawCollisionBox) {
        DrawRectangle(bullet->collisionBox.x, bullet->collisionBox.y, bullet->collisionBox.width, bullet->collisionBox.height, BLUE);
    }

}

void DrawGrenade(Grenade *grenade, Texture2D texture, bool drawCollisionCircle) {
    Vector2 origin = (Vector2) {122/2, 122/2};
    DrawTexturePro(texture, grenade->animation.currentAnimationFrameRect, grenade->drawableRect, origin, grenade->angle, WHITE);
    // Draw das caixas de colisão
    if (drawCollisionCircle) {
        DrawCircle(grenade->collisionCircle.center.x, grenade->collisionCircle.center.y, grenade->collisionCircle.radius, BLUE);
    }
}

void DrawParticle(Particle *particle, Texture2D texture) {
    Vector2 origin = (Vector2) {MISC_GRID[0]/2, MISC_GRID[1]/2};
    DrawTexturePro(texture, particle->frameRect, particle->drawableRect, origin, particle->angle, WHITE);
}

void DrawMSG(MSGSystem *msg) {
    DrawText(TextFormat("%i", msg->msg), msg->position.x, msg->position.y, 15, msg->color);
}

void DrawPlayer(Player *player, Texture2D texture, bool drawCollisionBox) {
    // Draw das caixas de colisão
    if (drawCollisionBox) {
        DrawRectangle(player->entity.collisionBox.x, player->entity.collisionBox.y, player->entity.collisionBox.width, player->entity.collisionBox.height, WHITE);
        DrawCircle(player->entity.collisionHead.center.x, player->entity.collisionHead.center.y, player->entity.collisionHead.radius, YELLOW);
    }
    DrawTexturePro(texture, player->entity.lowerAnimation.currentAnimationFrameRect, player->entity.drawableRect, (Vector2) {player->entity.width/2, player->entity.height/2}, 0, WHITE);
    DrawTexturePro(texture, player->entity.upperAnimation.currentAnimationFrameRect, player->entity.drawableRect, (Vector2) {player->entity.width/2, player->entity.height/2}, 0, WHITE);
}

RenderTexture2D PaintCanvas(Texture2D atlas, enum BACKGROUND_TYPES bgLayer, Ground *groundPool, int relativeXPos) {
    int width = screenWidth;
    int height = screenHeight;
    RenderTexture2D canvas = LoadRenderTexture(width, height);
    switch(bgLayer) {
        case BACKGROUND:
            GenerateBackground(canvas, atlas, SKYSCRAPER);
        break;
        case MIDDLEGROUND:
            GenerateMidground(canvas, atlas, COMPLEX);
        break;
        case FOREGROUND:
            if (GetRandomValue(1,10) < 7)
                GenerateForeground(canvas, groundPool, atlas, URBAN_FOREST, relativeXPos);
            else
                GenerateForeground(canvas, groundPool, atlas, RESIDENTIAL, relativeXPos);
        break;
    }

    return canvas;
}

void GenerateBackground(RenderTexture2D canvas, Texture atlas, enum BACKGROUND_STYLE bgStyle) {
    int frameWidth;
    int frameHeight;
    int offset;
    int buildingRow;
    int buildingCol;
    BeginTextureMode(canvas);
    switch (bgStyle) {
    case SKYSCRAPER:
        frameWidth = BACKGROUND_GRID[0];
        frameHeight = BACKGROUND_GRID[1];
        buildingRow = BACKGROUND_SKYSCRAPER_ROW;
        offset = 5;
        for (int i = 0; i < 14; i++) { // totalProps max
            buildingCol = GetRandomValue(0, BACKGROUND_SKYSCRAPER_NUM_TYPES-1); // 4 Tipos
            if (GetRandomValue(1,5) >= 2) // 80% de Gerar
                DrawTexturePro(atlas, (Rectangle){buildingCol*frameWidth, buildingRow*frameHeight, frameWidth, frameHeight},
                    (Rectangle){offset + (i*(offset+frameWidth)), (canvas.texture.height - 2*frameHeight - GetRandomValue(20, 100)), frameWidth*1.2f, 2*GetRandomValue(frameHeight-10, frameHeight+10)}, (Vector2) {0, 0}, 0, WHITE);
        }
        break;
    default:
        break;
    }
    EndTextureMode();
}

void GenerateMidground(RenderTexture2D canvas, Texture atlas, enum MIDDLEGROUND_STYLE mgStyle) {
    int frameWidth = 0;
    int frameHeight = 0;
    int offset = 0;
    int buildingRow = 0;
    int buildingCol = 0;
    BeginTextureMode(canvas);
    switch (mgStyle) {
    case COMPLEX:
        frameWidth = MIDGROUND_GRID[0];
        frameHeight = MIDGROUND_GRID[1];
        offset = 30;
        buildingCol = MIDGROUND_SKYSCRAPER_COL;
        for (int j = 0; j < 4; j++) { //4 Unidades por chunk
            int numFloor = GetRandomValue(15,20);
            int heightScale = 2*GetRandomValue(-3,3);
            int widthScale = 2*GetRandomValue(-10,-5);
            int doubled = GetRandomValue(1,5);
            for (int i = 0; i < numFloor; i++) {
                buildingRow = GetRandomValue(0,MIDGROUND_SKYSCRAPER_NUM_TYPES-1); // 6 tipos
                DrawTexturePro(atlas, (Rectangle){buildingCol*frameWidth, buildingRow*frameHeight, frameWidth, frameHeight},
                    (Rectangle){offset + (j*(offset+2*frameWidth+widthScale)), (canvas.texture.height - 150) - i*(frameHeight+heightScale), frameWidth + widthScale, frameHeight + heightScale},
                    (Vector2) {0, 0}, 0, WHITE);
                if (doubled < 2) {
                    buildingRow = GetRandomValue(0,MIDGROUND_SKYSCRAPER_NUM_TYPES-1); // 6 tipos
                    DrawTexturePro(atlas, (Rectangle){buildingCol*frameWidth, buildingRow*frameHeight, -frameWidth, frameHeight},
                        (Rectangle){offset + frameWidth +widthScale+ (j*(offset+2*frameWidth+widthScale)), (canvas.texture.height - 150) - i*(frameHeight+heightScale), frameWidth + widthScale, frameHeight + heightScale},
                        (Vector2) {0, 0}, 0, WHITE);
                }
            }
        }
        break;
    default:
        break;
    }
    EndTextureMode();
 }

void GenerateForeground(RenderTexture2D canvas, Ground *groundPool, Texture atlas, enum FOREGROUND_STYLE fgStyle, int relativeXPos) {
    int frameWidth = FOREGROUND_GRID[0];
    int frameHeight = FOREGROUND_GRID[0];
    int overhang;
    int buildingRow;
    int numFloor;
    int tilesWidth;
    bool hasDoor;
    int isFlipped;
    int style;
    int buildType;
    bool generateGround;

    int willDraw;
    int numOfRows;
    int treeId;
    BeginTextureMode(canvas);
    switch (fgStyle) {
    case RESIDENTIAL:
        DrawRectangle(0, screenHeight-200, screenWidth, 200, DARKGRAY);
        numOfRows = GetRandomValue(1,1);
        for (int k = 0; k < numOfRows; k++) {
            int yOffset = k * (30);
            for (int l = 0; l < (int)(screenWidth/frameWidth)-2; l++) {
                int xOffset = 10 + l*frameWidth;
                generateGround = false;
                buildType = GetRandomValue(1,100);
                if (buildType < 90 ) {// Gerar prédio
                    overhang = 0;
                    buildingRow = 0;
                    numFloor = GetRandomValue(3,4);
                    tilesWidth = GetRandomValue(3,4);
                    l += tilesWidth;
                    hasDoor = false;
                    isFlipped = 1;
                    style = GetRandomValue(0,FOREGROUND_NUM_TYPES-1); // 2 estilos
                    for (int i = 0; i < numFloor; i++) {
                        for (int j = 0; j < tilesWidth; j++) {
                            overhang = 0;
                            if (j == 0) { //borda esquerda
                                buildingRow = FOREGROUND_EDGE_ROW;
                                isFlipped = 1;
                            } else if (j == tilesWidth - 1) { // borda direita
                                buildingRow = FOREGROUND_EDGE_ROW;
                                isFlipped = -1;
                            } else {
                                if (i == 0) { // andar térreo
                                    if (j == tilesWidth - 2 && !hasDoor) { // Se não teve porta até a penúltima casa, forçar porta
                                        buildingRow = FOREGROUND_DOOR_ROW;
                                    } else {
                                        if (!hasDoor) {
                                            buildingRow = (GetRandomValue(0,3) == 0 ? 1 : 2); // TODO possibilidades
                                            if (buildingRow == FOREGROUND_DOOR_ROW) {
                                                hasDoor = true;
                                            }
                                        } else { // Se já tem porta, parede
                                            buildingRow = FOREGROUND_WALL1_ROW;
                                        }
                                    }
                                } else { // Casas normais
                                    buildingRow = (GetRandomValue(0,3) == 0 ? 3 : 2); // TODO possibilidades
                                    if (GetRandomValue(1,100) <= 20) { //3% de chance de gerar um "balcão"
                                        generateGround = true;
                                    }
                                }
                            }
                            if (i == numFloor - 1) { // teto
                                overhang = 7;
                                buildingRow = FOREGROUND_ROOF_ROW;
                                CreateGround(groundPool,(Vector2) {xOffset+j*frameWidth-overhang + relativeXPos*canvas.texture.width, (canvas.texture.height - 150) - (i)*(frameHeight)-50 - (40 - yOffset)}, frameWidth+2*overhang, 20, true, false, false, true, false, -1);
                            }
                            DrawTexturePro(atlas, (Rectangle){style*frameWidth, buildingRow*frameHeight, isFlipped*frameWidth, frameHeight},
                                (Rectangle){xOffset+j*frameWidth-overhang, (canvas.texture.height - 150) - (i+1)*(frameHeight) - (40 - yOffset), frameWidth+2*overhang, frameHeight}, // deslocado 150 pixels acima do fundo da tela
                                (Vector2) {0, 0}, 0, WHITE);
                            if (generateGround) {
                                CreateGround(groundPool,(Vector2) {xOffset+j*frameWidth-overhang + relativeXPos*canvas.texture.width, (canvas.texture.height - 150) - (i)*(frameHeight)-50 - (40 - yOffset)}, frameWidth+2*overhang, 20, true, false, false, true, false, -1);
                                DrawTexturePro(atlas, (Rectangle){style*frameWidth, FOREGROUND_ROOF_ROW*frameHeight, isFlipped*frameWidth, frameHeight},
                                (Rectangle){xOffset+j*frameWidth-overhang, (canvas.texture.height - 150) - (i+1)*(frameHeight) - (40 - yOffset), frameWidth+2*overhang, frameHeight}, // deslocado 150 pixels acima do fundo da tela
                                (Vector2) {0, 0}, 0, WHITE);
                            }
                        }
                    }
                } else { // Gerar chip implant
                    if (GetRandomValue(1,2) == 1) {
                        int posX = xOffset;
                        l += FOREGROUND_CHIP_IMPLANT_RECT[2];
                        DrawTexturePro(atlas, (Rectangle){FOREGROUND_CHIP_IMPLANT_RECT[0]*frameWidth, FOREGROUND_CHIP_IMPLANT_RECT[1]*frameHeight, FOREGROUND_CHIP_IMPLANT_RECT[2]*frameWidth, FOREGROUND_CHIP_IMPLANT_RECT[3]*frameHeight},
                            (Rectangle){posX, (canvas.texture.height - 145) - FOREGROUND_CHIP_IMPLANT_RECT[3]*frameHeight/2 - (40 - yOffset), FOREGROUND_CHIP_IMPLANT_RECT[2]*frameWidth/2, FOREGROUND_CHIP_IMPLANT_RECT[3]*frameHeight/2}, // deslocado 150 pixels acima do fundo da tela
                            (Vector2) {0, 0}, 0, WHITE);
                        CreateGround(groundPool,(Vector2) {posX+35 + relativeXPos*screenWidth, (canvas.texture.height - 145) - FOREGROUND_CHIP_IMPLANT_RECT[3]*frameHeight/2 - (40 - yOffset)}, FOREGROUND_CHIP_IMPLANT_RECT[2]*frameWidth/2 - 70, 20, true, false, false, true, false, -1);
                    } else {
                        int posX = xOffset;
                        l += FOREGROUND_SUSHI_BAR_RECT[2];
                        DrawTexturePro(atlas, (Rectangle){FOREGROUND_SUSHI_BAR_RECT[0]*frameWidth, FOREGROUND_SUSHI_BAR_RECT[1]*frameHeight, FOREGROUND_SUSHI_BAR_RECT[2]*frameWidth, FOREGROUND_SUSHI_BAR_RECT[3]*frameHeight},
                            (Rectangle){posX, (canvas.texture.height - 145) - FOREGROUND_SUSHI_BAR_RECT[3]*frameHeight/2 - (40 - yOffset), FOREGROUND_SUSHI_BAR_RECT[2]*frameWidth/2, FOREGROUND_SUSHI_BAR_RECT[3]*frameHeight/2}, // deslocado 150 pixels acima do fundo da tela
                            (Vector2) {0, 0}, 0, WHITE);
                        CreateGround(groundPool,(Vector2) {posX+15 + relativeXPos*screenWidth, (canvas.texture.height - 145) - FOREGROUND_SUSHI_BAR_RECT[3]*frameHeight/2 + 2*0.14f*frameHeight - (40 - yOffset)}, FOREGROUND_SUSHI_BAR_RECT[2]*frameWidth/2 - 30, 20, true, false, false, true, false, -1);
                    }
                }
            }
        }
        break;
    case URBAN_FOREST:
        DrawRectangle(0, screenHeight-200, screenWidth, 200, DARKGREEN);
        numOfRows = GetRandomValue(2,3);
        int yOffset = 250;
        for (int i = 0; i < numOfRows; i++) {
            yOffset -= GetRandomValue(15,24);
            for (int k = frameWidth/2; k < screenWidth - frameWidth-50; k++){
                k+=49;
                willDraw = GetRandomValue(1,10);
                if (willDraw >= 2) { // 90% de chance de desenhar árvore
                    treeId = GetRandomValue(FOREGROUND_TREE1_COL, FOREGROUND_TREE3_COL);
                    DrawTexturePro(atlas, (Rectangle){treeId*frameWidth, FOREGROUND_TREE_ROW*frameHeight,  frameWidth, frameHeight},
                        (Rectangle){k + GetRandomValue(-5, 5), (canvas.texture.height - 150) - yOffset+ GetRandomValue(0, 7), frameWidth, frameHeight * (1 + GetRandomValue(0,3)/10)},
                        (Vector2) {0, 0}, 0, WHITE);
                }
            }
        }
        float ratio = (10* (float)frameWidth/ (float) screenWidth);
        for (int i = 0; i < (int)(screenWidth/frameWidth)+1; i++) {
            if (i == 0 || i == (int)(screenWidth/frameWidth)) {
                    DrawTexturePro(atlas, (Rectangle){FOREGROUND_STREET_WALL[0]*frameWidth, FOREGROUND_STREET_WALL[1]*frameHeight,  frameWidth, frameHeight},
                        (Rectangle){i*frameWidth*0.96f, (screenHeight - frameHeight - 150), frameWidth*0.96f, frameHeight},
                        (Vector2) {0, 0}, 0, WHITE);
                } else {
                    DrawTexturePro(atlas, (Rectangle){FOREGROUND_FENCE[0]*frameWidth, FOREGROUND_FENCE[1]*frameHeight,  frameWidth, frameHeight},
                        (Rectangle){i*frameWidth*0.96f, (screenHeight - frameHeight - 150), frameWidth*0.96f, frameHeight},
                        (Vector2) {0, 0}, 0, WHITE);

                    if (GetRandomValue(1,50) == 1) {
                        int Col = GetRandomValue(0,FOREGROUND_DECALS[2]-1);
                        int Row = GetRandomValue(0,FOREGROUND_DECALS[3]-1);
                        DrawTexturePro(atlas, (Rectangle){(FOREGROUND_DECALS[0]+Col)*frameWidth, (FOREGROUND_DECALS[1]+Row)*frameHeight,  frameWidth, frameHeight},
                        (Rectangle){i*frameWidth*0.96f + (0.96f*frameWidth)/2 - 0.25f*frameWidth, (screenHeight - 2*frameHeight/3 - 120 - GetRandomValue(30,55)), frameWidth*0.5f, frameHeight*0.5f},
                        (Vector2) {0, 0}, 0, WHITE);
                    }
                }
        }
        break;
    default:    
        break;
    }

    // Chão
    for (int i = 0; i < (int)(screenWidth/frameWidth)+1; i++) {
        DrawTexturePro(atlas, (Rectangle){0, FOREGROUND_STREET_ROW*frameHeight, frameWidth, frameHeight},
                        (Rectangle){i*frameWidth, (canvas.texture.height - 150), frameWidth, frameHeight},
                        (Vector2) {0, 0}, 0, WHITE);
    }

    // Poste
    for (int i = 0; i < 3; i++) {
        if (i == 1) { // Parada de ônibus
            if (GetRandomValue(1,10) == 1) {
                DrawTexturePro(atlas, (Rectangle){FOREGROUND_BUS_STOP[0]*frameWidth, FOREGROUND_BUS_STOP[1]*frameHeight, frameWidth, frameHeight},
                    (Rectangle){2*frameWidth + i*2*frameWidth , screenHeight - 1.15f*frameHeight - 125, 1.15f*frameWidth, 1.15f*frameHeight},
                    (Vector2) {0, 0}, 0, WHITE);
            }
        } else {
            DrawTexturePro(atlas, (Rectangle){FOREGROUND_LAMP_POST[0]*frameWidth, FOREGROUND_LAMP_POST[1]*frameHeight, frameWidth, frameHeight},
                (Rectangle){2*frameWidth + i*2*frameWidth , screenHeight - 1.15f*frameHeight - 125, 1.15f*frameWidth, 1.15f*frameHeight},
                (Vector2) {0, 0}, 0, WHITE);
        }
    }
    EndTextureMode();
}