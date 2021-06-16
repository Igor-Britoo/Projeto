#include "gameConfig.c"

int main(void) {
    if (isFullscreen) SetConfigFlags(FLAG_FULLSCREEN_MODE); // Fullscreen
    InitWindow(screenWidth, screenHeight, gameName);
    SetTargetFPS(60);      // Set our game to run at 60 frames-per-second

    // Load assets
    Texture2D backgroundTex = LoadTexture("textures/cyberpunk_street_background.png");
    Texture2D midgroundTex = LoadTexture("textures/cyberpunk_street_midground.png");
    Texture2D foregroundTex = LoadTexture("textures/cyberpunk_street_foreground.png");
    Texture2D characterTex = LoadTexture("textures/hero_atlas.png");    
    Texture2D swordsmanTex = LoadTexture("textures/hero_atlas.png");        

    // Player Init
    Player player = CreatePlayer(100, (Vector2){0, 308},characterTex.width, characterTex.height);

    // Camera init
    float camMinX = 0; // Usado no avanço da câmera e na limitação de movimentação para trás do player
    float camMaxX = 0; // Usado no avanço da câmera
    Camera2D camera = CreateCamera(player.entity.position, (Vector2) {screenWidth/2.0f, screenHeight/2.0f}, 0.0f, 1.00f);

    // Environment Init
    Background nearBackgroundPool[5], middleBackgroundPool[5], farBackgroundPool[5]; // Pool de backgrounds. Sempre que um "sai" da tela, é destruído e um novo é gerado no "final" do mapa
    int numNearBackground = 0, numMiddleBackground = 0, numFarBackground = 0; // Usado para posicionamento correto das novas imagens geradas
    for (int i = 0; i < numBackgroundRendered; i++) {
        farBackgroundPool[i] = CreateBackground(&player, farBackgroundPool, BACKGROUND, &numFarBackground, 2.0f);
        middleBackgroundPool[i] = CreateBackground(&player, middleBackgroundPool, MIDDLEGROUND, &numMiddleBackground, 2.0f);
        nearBackgroundPool[i] = CreateBackground(&player, nearBackgroundPool, FOREGROUND, &numNearBackground, 2.0f);
    }
    Props envProps[1] = {
        {{0,430,screenWidth*5,5}, 1, 0, 1}// Floor
        };


    Enemy enemies[10];
    enemies[0] = CreateEnemy(BOSS,500, (Vector2){700, 308},characterTex.width, characterTex.height);

    // Main game loop
    while (!WindowShouldClose()) {
        // Updates
        float deltaTime = GetFrameTime();
        UpdatePlayer(&player, deltaTime, envProps, camMinX);
        // Atualizar limites de câmera e posição
        camMinX = (camMinX < camera.target.x - camera.offset.x ? camera.target.x - camera.offset.x : camMinX);
        UpdateClampedCameraPlayer(&camera, &player, envProps, deltaTime, screenWidth, screenHeight, &camMinX, camMaxX);
        UpdateProps(&player, envProps, deltaTime, camMinX);

        UpdateBackground(&player, nearBackgroundPool, numBackgroundRendered, deltaTime, &numNearBackground, camMinX, &camMaxX);
        UpdateBackground(&player, middleBackgroundPool, numBackgroundRendered, deltaTime, &numMiddleBackground, camMinX, &camMaxX);
        UpdateBackground(&player, farBackgroundPool, numBackgroundRendered,deltaTime, &numFarBackground, camMinX, &camMaxX);

        // Draw cycle
        BeginDrawing();

            ClearBackground(GetColor(0x052c46ff));

            BeginMode2D(camera);

                // Desenhar os backgrounds
                for (int i = 0; i < numBackgroundRendered; i++) {
                    DrawTextureEx(backgroundTex, (Vector2){farBackgroundPool[i].position.x, farBackgroundPool[i].position.y}, 0.0f, farBackgroundPool[i].scale, WHITE);
                }       
                // Desenhar os middlegrounds
                for (int i = 0; i < numBackgroundRendered; i++) {
                    DrawTextureEx(midgroundTex, (Vector2){middleBackgroundPool[i].position.x, middleBackgroundPool[i].position.y}, 0.0f, middleBackgroundPool[i].scale, WHITE);
                }       
                // Desenhar os foregrounds
                for (int i = 0; i < numBackgroundRendered; i++) {
                    DrawTextureEx(foregroundTex, (Vector2){nearBackgroundPool[i].position.x, nearBackgroundPool[i].position.y}, 0.0f, nearBackgroundPool[i].scale, WHITE);
                }       

                // Draw player
                float charWidth = player.entity.animationFrameWidth*player.entity.characterWidthScale;
                float charHeight= player.entity.animationFrameHeight*player.entity.characterHeightScale;
                Rectangle dstRect = {player.entity.position.x, player.entity.position.y, charWidth, charHeight};
                DrawTexturePro(characterTex, player.entity.currentAnimationFrameRect, dstRect, (Vector2) {0, 0}, 0, WHITE);

                // Draw Enemy
                enemies[0].entity.isFacingRight = 0;
                charWidth = enemies[0].entity.animationFrameWidth*enemies[0].entity.characterWidthScale;
                charHeight= enemies[0].entity.animationFrameHeight*enemies[0].entity.characterHeightScale;
                dstRect = (Rectangle){enemies[0].entity.position.x, enemies[0].entity.position.y, charWidth, charHeight};
                DrawTexturePro(swordsmanTex, enemies[0].entity.currentAnimationFrameRect, dstRect, (Vector2) {0, 0}, 0, WHITE);

                DrawLine(player.entity.position.x, player.entity.position.y, player.entity.position.x+player.entity.animationFrameWidth, player.entity.position.y+player.entity.animationFrameHeight, RED);
                DrawLine(enemies[0].entity.position.x, enemies[0].entity.position.y, enemies[0].entity.position.x+enemies[0].entity.animationFrameWidth, enemies[0].entity.position.y+enemies[0].entity.animationFrameHeight, RED);


                // Draw props
                for (int i = 0; i < 1; i++) { // TODO 1 is "props[]"'s size
                    if (!envProps[i].isInvisible)
                        DrawRectangleRec(envProps[i].rect, WHITE);
                }


            EndMode2D();

            // TODO o que está nessa região fica "parado" em relação à câmera

            // Debbug
            char debbugMsg[20];
            //DrawText("Far:", 0, 0, 20, WHITE);
            sprintf(debbugMsg, "%f", player.entity.position.y);
            DrawText(debbugMsg, 0, 0, 20, WHITE);
            sprintf(debbugMsg, "%f", enemies[0].entity.position.y);
            DrawText(debbugMsg, 0, 25, 20, WHITE);

        EndDrawing();
    }

    // Unload
    UnloadTexture(backgroundTex);
    UnloadTexture(midgroundTex);
    UnloadTexture(foregroundTex);
    UnloadTexture(characterTex);
    UnloadTexture(swordsmanTex);

    CloseWindow();
    return 0;
}

Background CreateBackground(Player *player, Background *backgroundPool, enum BACKGROUND_TYPES bgType, int *numBackground, float scale) {
    Background dstBackground;
    int numBg = *numBackground;

    switch (bgType)
    {
    case BACKGROUND:
        dstBackground.width = 512*scale;
        dstBackground.height = 192*scale;
        dstBackground.position.y = 20;
        dstBackground.relativePosition = -0.05f; // Velocidade do parallax (quando menor, mais lento)
        break;
    case MIDDLEGROUND:
        dstBackground.width = 512*scale;
        dstBackground.height = 192*scale;
        dstBackground.position.y = 20;
        dstBackground.relativePosition = -0.025f; // Velocidade do parallax (quando menor, mais lento)
        break;
    case FOREGROUND:
        dstBackground.width = 704*scale;
        dstBackground.height = 192*scale;
        dstBackground.position.y = 70;
        dstBackground.relativePosition = 0.0f; // Velocidade do parallax (quando menor, mais lento)
        break;
    default:
        break;
    }

    dstBackground.bgType = bgType;
    dstBackground.scale = scale;
    dstBackground.position.x = numBg * dstBackground.width;
    dstBackground.originalX = dstBackground.position.x;
    *numBackground = numBg + 1;

    return dstBackground;
}

Player CreatePlayer (int maxHP, Vector2 position, float imageWidth, float imageHeight) {
    Player newPlayer;
    newPlayer.entity.maxHP = maxHP;
    newPlayer.entity.currentHP = maxHP;
    newPlayer.entity.position.x = position.x;
    newPlayer.entity.position.y = position.y;
    newPlayer.entity.velocity.x = 0;
    newPlayer.entity.velocity.y = 0;
    newPlayer.currentWeaponID = -1;
    newPlayer.currentAmmo = 0;
    newPlayer.entity.walkSpeed = 400;
    newPlayer.entity.sprintSpeed = 800;
    newPlayer.entity.jumpSpeed = 250;
    newPlayer.entity.isGrounded = true;

    newPlayer.entity.animationFrameSpeed = 0.08f;
    newPlayer.entity.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newPlayer.entity.animationFrameHeight = 122;//imageHeight;
    newPlayer.entity.currentAnimationFrame = 0;
    newPlayer.entity.currentAnimationState = IDLE;
    newPlayer.entity.isFacingRight = 1;
    newPlayer.entity.timeSinceLastFrame = 0;
    newPlayer.entity.currentAnimationFrameRect.x = 0.0f;
    newPlayer.entity.currentAnimationFrameRect.y = 0.0f;
    newPlayer.entity.currentAnimationFrameRect.width = newPlayer.entity.animationFrameWidth;
    newPlayer.entity.currentAnimationFrameRect.height = newPlayer.entity.animationFrameHeight;
    newPlayer.entity.characterWidthScale = 1.00f;
    newPlayer.entity.characterHeightScale = 1.00f;

    return newPlayer;
}

Enemy CreateEnemy(enum ENEMY_CLASSES class, int maxHP, Vector2 position, float imageWidth, float imageHeight) {
    Enemy newEnemy;

    newEnemy.target = (Vector2){-1, -1};
    newEnemy.class = class;
    newEnemy.viewDistance = 150;

    newEnemy.entity.maxHP = maxHP;
    newEnemy.entity.currentHP = maxHP;
    newEnemy.entity.position.x = position.x;
    newEnemy.entity.position.y = position.y;
    newEnemy.entity.velocity.x = 0;
    newEnemy.entity.velocity.y = 0;
    newEnemy.entity.walkSpeed = 300;
    newEnemy.entity.sprintSpeed = 800;
    newEnemy.entity.jumpSpeed = 250;
    newEnemy.entity.isGrounded = false;

    newEnemy.entity.animationFrameSpeed = 0.08f;
    newEnemy.entity.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newEnemy.entity.animationFrameHeight = 122;//imageHeight;
    newEnemy.entity.currentAnimationFrame = 0;
    newEnemy.entity.currentAnimationState = IDLE;
    newEnemy.entity.isFacingRight = 1;
    newEnemy.entity.timeSinceLastFrame = 0;
    newEnemy.entity.currentAnimationFrameRect.x = 0.0f;
    newEnemy.entity.currentAnimationFrameRect.y = 0.0f;
    newEnemy.entity.currentAnimationFrameRect.width = newEnemy.entity.animationFrameWidth;
    newEnemy.entity.currentAnimationFrameRect.height = newEnemy.entity.animationFrameHeight;
    newEnemy.entity.characterWidthScale = 1.00f;
    newEnemy.entity.characterHeightScale = 1.00f;
    return newEnemy;
}

Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom) {
    Camera2D newCam = {0};
    newCam.target = target;
    newCam.offset = offset;
    newCam.rotation = rotation;
    newCam.zoom = zoom;

    return newCam;
}

void UpdatePlayer(Player *player, float delta, Props *props, float minX) {
    enum CHARACTER_STATE currentState = player->entity.currentAnimationState;
    player->entity.timeSinceLastFrame += delta;

    if (player->entity.currentAnimationState != DYING && player->entity.currentAnimationState != HURT) {
        if (IsKeyDown(KEY_LEFT)) {
            player->entity.position.x -= player->entity.walkSpeed*delta;
            player->entity.isFacingRight = -1;
            player->entity.currentAnimationState = WALKING;
            
        } else if (IsKeyDown(KEY_RIGHT)) {
            player->entity.position.x += player->entity.walkSpeed*delta;
            player->entity.isFacingRight = 1;
            player->entity.currentAnimationState = WALKING;
        } else {
            player->entity.currentAnimationState = IDLE;
        }

        if (IsKeyDown(KEY_SPACE) && player->entity.isGrounded) 
        {
            player->entity.velocity.y = -player->entity.jumpSpeed;
            player->entity.isGrounded = false;
        }

        if (IsKeyPressed(KEY_H))
            player->entity.currentAnimationState = HURT;

        if (IsKeyPressed(KEY_P))
            player->entity.currentHP = 0;
    }

    // Collision check
    int hitObstacle = 0;
    int hasFloorBelow = 0;
    Rectangle prect = {player->entity.position.x, player->entity.position.y, player->entity.animationFrameWidth, player->entity.animationFrameHeight};
    Rectangle prectGrav = {player->entity.position.x, player->entity.position.y+1, player->entity.animationFrameWidth, player->entity.animationFrameHeight};
    for (int i = 0; i < 1; i++)  // TODO 1 is "props[]"'s size
    {
        Props *eprop = props + i;
        Vector2 *p = &(player->entity.position);
        if (eprop->canBeStepped) {
            if (CheckCollisionRecs(eprop->rect, prect)) {
                hitObstacle = 1;
                player->entity.velocity.y = 0.0f;
                p->y = eprop->rect.y - player->entity.animationFrameHeight;
            }
            if (CheckCollisionRecs(eprop->rect, prectGrav)) {
                hasFloorBelow = 1;
            }
        }
    }
    
    if (!hitObstacle) 
    {
        player->entity.position.y += player->entity.velocity.y * delta;
        player->entity.velocity.y += GRAVITY * delta;
        player->entity.isGrounded = false;
    } 
    else player->entity.isGrounded = true;

    if (hasFloorBelow) {
        player->entity.velocity.y = 0;
        player->entity.isGrounded = true;
    }

    // Animation handlers
    if ((player->entity.currentAnimationState != DYING) && (player->entity.currentAnimationState != HURT)) {
        if (!player->entity.isGrounded) {
            if (player->entity.velocity.y < 0) {
                player->entity.currentAnimationState = JUMPING;
            } else if(player->entity.velocity.y > 0) {
                player->entity.currentAnimationState = FALLING;
            }
        }
    }

    // Atualização de estado quando o player morre
    if (player->entity.currentHP <= 0 && !(player->entity.currentAnimationState == DYING)) {
        player->entity.currentAnimationState = DYING;
    }

    int animRow = 0;
    if (currentState != player->entity.currentAnimationState) {
        player->entity.timeSinceLastFrame = 0.0f;
        player->entity.currentAnimationFrame = 0;
    }
    switch (player->entity.currentAnimationState)
    {
    case IDLE:
        animRow = 0;
        if (player->entity.timeSinceLastFrame >= player->entity.animationFrameSpeed) {
            player->entity.timeSinceLastFrame = 0.0f;
            player->entity.currentAnimationFrame++;
            if (player->entity.currentAnimationFrame > 5) player->entity.currentAnimationFrame = 0; // 5 porque são 6 frames para essa animação 
        }
        break;
    case WALKING:
        animRow = 1;
        if (player->entity.timeSinceLastFrame >= player->entity.animationFrameSpeed) {
            player->entity.timeSinceLastFrame = 0.0f;
            player->entity.currentAnimationFrame++;
            if (player->entity.currentAnimationFrame > 7) player->entity.currentAnimationFrame = 0; // 7 porque são 8 frames para essa animação 
        }
        break;
    case HURT:
        animRow = 2;
        if (player->entity.timeSinceLastFrame >= player->entity.animationFrameSpeed) {
            player->entity.timeSinceLastFrame = 0.0f;
            player->entity.currentAnimationFrame++;
            if (player->entity.currentAnimationFrame > 5) { // 5 porque são 6 frames para essa animação, depois muda o estado
                player->entity.currentAnimationFrame = 0; 
                player->entity.currentAnimationState = IDLE;
            } else if (player->entity.currentAnimationFrame < 2) {
                player->entity.position.x -= player->entity.isFacingRight*600*delta;
            }
        }
        break;
    case JUMPING:
        animRow = 3;
        if (player->entity.timeSinceLastFrame >= player->entity.animationFrameSpeed) {
            player->entity.timeSinceLastFrame = 0.0f;
            player->entity.currentAnimationFrame++;
            if (player->entity.currentAnimationFrame > 4) player->entity.currentAnimationFrame = 4; // 4 porque são 5 frames para essa animação, além disso, mantém o frame em "4"
        }
        break;
    case FALLING:
        animRow = 3;
        if (player->entity.timeSinceLastFrame >= player->entity.animationFrameSpeed) {
            player->entity.timeSinceLastFrame = 0.0f;
            player->entity.currentAnimationFrame++;
            player->entity.currentAnimationFrame = 5; // Mantém o frame em "4", pois a queda é apenas 1 frame
        }
        break;
    case DYING:
        animRow = 4;
        if (player->entity.timeSinceLastFrame >= player->entity.animationFrameSpeed) {
            player->entity.timeSinceLastFrame = 0.0f;
            player->entity.currentAnimationFrame++;
            if (player->entity.currentAnimationFrame > 6) { // 6 porque são 7 frames para essa animação, além disso, mantém o frame em "6"
                player->entity.currentAnimationFrame = 6; 
            } else {
                player->entity.position.x -= player->entity.isFacingRight*1000*delta;
            }
            if (!player->entity.isGrounded) player->entity.position.x -= player->entity.isFacingRight*1000*delta;
        }
        break;
    default:
        break;
    }

    // Update player animation frame Rect
    player->entity.currentAnimationFrameRect.x = (float)player->entity.currentAnimationFrame * player->entity.animationFrameWidth;
    player->entity.currentAnimationFrameRect.y = animRow * player->entity.animationFrameHeight;
    player->entity.currentAnimationFrameRect.width = player->entity.isFacingRight * player->entity.animationFrameWidth;

    // Limitar posição do player de acordo com o avanço da câmera
    if (player->entity.position.x < minX) {
        player->entity.position.x = minX;
    } 
}

void UpdateEnemy(Enemy *enemy, Player *player, float delta, Props *props) {
    Entity *eEnt = &(enemy->entity);
    enum CHARACTER_STATE currentState = eEnt->currentAnimationState;
    eEnt->timeSinceLastFrame += delta;

    if (enemy->target.x < 0 && enemy->target.y) { // comportamento quando não tem target
        int random = GetRandomValue(1, 5); // 5 possibilidades
        if (random <= 1) { // 10%
            // Mudar direção
            eEnt->isFacingRight = !eEnt->isFacingRight;
            eEnt->momentum.x = 0; // Parar
        } else { // 80%
            random = GetRandomValue(1,5);
            if (random <= 2) {// 20%
                // Alguma outra opção?
                eEnt->momentum.x = 10; // Tem que tunar
            } else {
                eEnt->momentum.x = 0; // Parar
            }
        }
    }

    // TODO Procurar target

    // Colisão
    int hitObstacle = 0;
    int hasFloorBelow = 0;
    Rectangle prect = {eEnt->position.x, eEnt->position.y, eEnt->animationFrameWidth, eEnt->animationFrameHeight};
    Rectangle prectGrav = {eEnt->position.x, eEnt->position.y+1, eEnt->animationFrameWidth, eEnt->animationFrameHeight};
    for (int i = 0; i < 1; i++)  // TODO 1 is "props[]"'s size
    {
        Props *eprop = props + i;
        Vector2 *p = &(eEnt->position);
        if (eprop->canBeStepped) {
            if (CheckCollisionRecs(eprop->rect, prect)) {
                hitObstacle = 1;
                eEnt->velocity.y = 0.0f;
                p->y = eprop->rect.y - eEnt->animationFrameHeight;
            }
            if (CheckCollisionRecs(eprop->rect, prectGrav)) {
                hasFloorBelow = 1;
            }
        }
    }
    
    if (!hitObstacle) 
    {
        eEnt->position.y += eEnt->velocity.y * delta;
        eEnt->velocity.y += GRAVITY * delta;
        eEnt->isGrounded = false;
    } 
    else eEnt->isGrounded = true;

    if (hasFloorBelow) {
        eEnt->velocity.y = 0;
        eEnt->isGrounded = true;
    }

    // Atualização da física
    eEnt->velocity.x += eEnt->isFacingRight*eEnt->momentum.x * delta;
    if (!eEnt->isGrounded) eEnt->velocity.y += GRAVITY * delta;

}

void UpdateClampedCameraPlayer(Camera2D *camera, Player *player, Props *props, float delta, int width, int height, float *minX, float maxX) {
    camera->target = player->entity.position;
    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    float minY = 0.00f, maxY = height; // Camera clamp controls. TODO, maxX
    
    *minX = fmaxf(0.0f, *minX);
    maxX = fmaxf(4*width, maxX);

    Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ *minX, minY }, *camera);
    
    if (max.x < width) camera->offset.x = width - (max.x - width/2);
    if (max.y < height) camera->offset.y = height - (max.y - height/2);
    if (min.x > 0) camera->offset.x = width/2 - min.x;
    if (min.y > 0) camera->offset.y = height/2 - min.y;
}

void UpdateProps(Player *player, Props *props, float delta, float minX) {
    Props *floor = props + 0;
    floor->rect.x = minX;
}

void UpdateBackground(Player *player, Background *backgroundPool, int poolLength, float delta, int *numBackground, float minX, float *maxX) {

    for (int i = 0; i < poolLength; i++) {
        Background *bgP = backgroundPool + i;
        bgP->position.x = (bgP->originalX - minX*bgP->relativePosition);
        if (bgP->position.x+bgP->width < minX) {
            //"Deletar" bg e criar um novo
            *bgP = CreateBackground(player, backgroundPool, bgP->bgType, numBackground, 2.0f);
            if (*maxX <= bgP->position.x + bgP->width)
                *maxX = bgP->position.x + bgP->width;
        }
    }
}

/*void CreateTexture() {
    // Define dimensões da imagem
    int width = 960;
    int height = 480;

    // Aloca memória para os pixels (em tipo Color)
    Color *pixels = (Color *)malloc(width*height*sizeof(Color));

    // Montagem da imagem
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (((x/32+y/32)/1)%2 == 0) pixels[y*width + x] = ORANGE;
            else pixels[y*width + x] = GOLD;
        }
    }

    // Transformar em Imagem (nesse momento ela vai para a CPU)
    Image checkedIm = {
        .data = pixels,             // We can assign pixels directly to data
        .width = width,
        .height = height,
        .format = UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };

    Texture2D checked = LoadTextureFromImage(checkedIm); // Lembrar do Dump da GPU no final do código...
    UnloadImage(checkedIm);         // Dump da imagem na CPU
}
*/