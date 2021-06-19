#include "gameConfig.c"

int main(void) {
    if (isFullscreen) SetConfigFlags(FLAG_FULLSCREEN_MODE); // Fullscreen
    InitWindow(screenWidth, screenHeight, gameName);
    SetTargetFPS(60);      // Set our game to run at 60 frames-per-second

    // Load assets
    Texture2D backgroundTex = LoadTexture("resources/Background/cyberpunk_street_background.png");
    Texture2D midgroundTex = LoadTexture("resources/Background/cyberpunk_street_midground.png");
    Texture2D foregroundTex = LoadTexture("resources/Background/cyberpunk_street_foreground.png");
    Texture2D characterTex = LoadTexture("resources/Atlas/hero_atlas.png");    
    Texture2D swordsmanTex = LoadTexture("resources/Atlas/hero_atlas.png");        

    // Player Init
    Player player = CreatePlayer(100, (Vector2){0, 150},characterTex.width, characterTex.height);

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

    // Enemy Init
    Enemy enemies[10];
    enemies[0] = CreateEnemy(BOSS,500, (Vector2){700, 150},characterTex.width, characterTex.height);

    // Main game loop
    while (!WindowShouldClose()) {
        // Updates
        float deltaTime = GetFrameTime();
        UpdatePlayer(&player, &enemies[0], deltaTime, envProps, camMinX);
        // Atualizar limites de câmera e posição
        camMinX = (camMinX < camera.target.x - camera.offset.x ? camera.target.x - camera.offset.x : camMinX);
        UpdateClampedCameraPlayer(&camera, &player, envProps, deltaTime, screenWidth, screenHeight, &camMinX, camMaxX);
        UpdateProps(&player, envProps, deltaTime, camMinX);

        UpdateBackground(&player, nearBackgroundPool, numBackgroundRendered, deltaTime, &numNearBackground, camMinX, &camMaxX);
        UpdateBackground(&player, middleBackgroundPool, numBackgroundRendered, deltaTime, &numMiddleBackground, camMinX, &camMaxX);
        UpdateBackground(&player, farBackgroundPool, numBackgroundRendered,deltaTime, &numFarBackground, camMinX, &camMaxX);

        UpdateEnemy(&enemies[0], &player, deltaTime, envProps);

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
                float charWidth = player.entity.animation.animationFrameWidth*player.entity.characterWidthScale;
                float charHeight= player.entity.animation.animationFrameHeight*player.entity.characterHeightScale;
                Rectangle dstRect = {player.entity.position.x, player.entity.position.y, charWidth, charHeight};
                DrawTexturePro(characterTex, player.entity.animation.currentAnimationFrameRect, dstRect, (Vector2) {0, 0}, 0, WHITE);

                // Draw Enemy
                charWidth = enemies[0].entity.animation.animationFrameWidth*enemies[0].entity.characterWidthScale;
                charHeight= enemies[0].entity.animation.animationFrameHeight*enemies[0].entity.characterHeightScale;
                dstRect = (Rectangle){enemies[0].entity.position.x, enemies[0].entity.position.y, charWidth, charHeight};
                DrawTexturePro(swordsmanTex, enemies[0].entity.animation.currentAnimationFrameRect, dstRect, (Vector2) {0, 0}, 0, WHITE);
                
                float eyesX = enemies[0].entity.position.x + enemies[0].entity.eyesOffset.x;
                float eyesY = enemies[0].entity.position.y + enemies[0].entity.eyesOffset.y;
                Rectangle detectionBox;
                if (enemies[0].entity.animation.isFacingRight == 1) {
                    detectionBox = (Rectangle){eyesX, eyesY, enemies[0].viewDistance, 5};
                } else {
                    detectionBox = (Rectangle){eyesX-enemies[0].viewDistance, eyesY, enemies[0].viewDistance, 5};
                }
                DrawRectangle(detectionBox.x, detectionBox.y, detectionBox.width, detectionBox.height, RED);

                // Draw props
                for (int i = 0; i < 1; i++) { // TODO 1 is "props[]"'s size
                    if (!envProps[i].isInvisible)
                        DrawRectangleRec(envProps[i].rect, WHITE);
                }


            EndMode2D();

            // TODO o que está nessa região fica "parado" em relação à câmera

    ////////////////////////////////////////////////// INÍCIO DE DEBBUG //////////////////////////////////////////////////
    /*
    char state[30];
    switch (enemies[0].entity.animation.currentAnimationState) {
    case IDLE:
        strcpy(state, "ANIMATION STATE: IDLE");
        break;
    case WALKING:
        strcpy(state, "ANIMATION STATE: WALKING");
        break;
    case JUMPING:
        strcpy(state, "ANIMATION STATE: JUMPING");
        break;
    case FALLING:
        strcpy(state, "ANIMATION STATE: FALLING");
        break;
    case ATTACKING:
        strcpy(state, "ANIMATION STATE: ATTACKING");
        break;
    default:
        break;
    }
    DrawText(state, 0, 600, 20, RED);

    switch (enemies[0].behavior) {
    case NONE:
        strcpy(state, "BEHAVIOR STATE: NONE");
        break;
    case ATTACK:
        strcpy(state, "BEHAVIOR STATE: ATTACK");
        break;
    case MOVE:
        strcpy(state, "BEHAVIOR STATE: MOVE");
        break;
    default:
        break;
    }
    DrawText(state, 0, 625, 20, RED);

    sprintf(state, "%f", enemies[0].entity.position.x);
    DrawText("ePx:", 0, 650, 20, RED);
    DrawText(state, 100, 650, 20, RED);
    sprintf(state, "%f", enemies[0].target.x);
    DrawText("tPx", 0, 675, 20, RED);
    DrawText(state, 100, 675, 20, RED);
    sprintf(state, "%f", enemies[0].noDetectionTime);
    DrawText("nDT:", 0, 700, 20, RED);
    DrawText(state, 100, 700, 20, RED);
*/
    ////////////////////////////////////////////////// FIM DE DEBBUG //////////////////////////////////////////////////

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
    newPlayer.entity.velocity.x = 0.0f;
    newPlayer.entity.velocity.y = 0.0f;
    newPlayer.entity.momentum.x = 0.0f;
    newPlayer.entity.momentum.y = 0.0f;
    newPlayer.currentWeaponID = -1;
    newPlayer.currentAmmo = 0;
    newPlayer.entity.walkSpeed = 400;
    newPlayer.entity.sprintSpeed = 800;
    newPlayer.entity.jumpSpeed = 250;
    newPlayer.entity.isGrounded = true;
    newPlayer.entity.eyesOffset = (Vector2) {55, 40};

    newPlayer.entity.animation.animationFrameSpeed = 0.08f;
    newPlayer.entity.animation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newPlayer.entity.animation.animationFrameHeight = 122;//imageHeight;
    newPlayer.entity.animation.currentAnimationFrame = 0;
    newPlayer.entity.animation.currentAnimationState = IDLE;
    newPlayer.entity.animation.isFacingRight = 1;
    newPlayer.entity.animation.timeSinceLastFrame = 0;
    newPlayer.entity.animation.currentAnimationFrameRect.x = 0.0f;
    newPlayer.entity.animation.currentAnimationFrameRect.y = 0.0f;
    newPlayer.entity.animation.currentAnimationFrameRect.width = newPlayer.entity.animation.animationFrameWidth;
    newPlayer.entity.animation.currentAnimationFrameRect.height = newPlayer.entity.animation.animationFrameHeight;
    newPlayer.entity.characterWidthScale = 1.00f;
    newPlayer.entity.characterHeightScale = 1.00f;

    return newPlayer;
}

Enemy CreateEnemy(enum ENEMY_CLASSES class, int maxHP, Vector2 position, float imageWidth, float imageHeight) {
    Enemy newEnemy;

    newEnemy.target = (Vector2){-1, -1};
    newEnemy.class = class;
    newEnemy.behavior = NONE;
    newEnemy.viewDistance = 600;
    newEnemy.attackRange = 200;
    newEnemy.behaviorChangeInterval = 3.5f; // Tempo em segundos para tentar alterar comportamento
    newEnemy.timeSinceLastBehaviorChange = 0;
    newEnemy.noDetectionTime = 0;
    newEnemy.loseTargetInterval = 5;
    newEnemy.spawnLocation = (Vector2){position.x, position.y};
    newEnemy.maxDistanceToSpawn = 1000;

    newEnemy.entity.maxHP = maxHP;
    newEnemy.entity.currentHP = maxHP;
    newEnemy.entity.position = newEnemy.spawnLocation;
    newEnemy.entity.velocity.x = 0.0f;
    newEnemy.entity.velocity.y = 0.0f;
    newEnemy.entity.momentum.x = 0.0f;
    newEnemy.entity.momentum.y = 0.0f;
    newEnemy.entity.walkSpeed = 200;
    newEnemy.entity.sprintSpeed = 800;
    newEnemy.entity.jumpSpeed = 250;
    newEnemy.entity.isGrounded = false;
    newEnemy.entity.eyesOffset = (Vector2) {55, 40};

    newEnemy.entity.animation.animationFrameSpeed = 0.10f;
    newEnemy.entity.animation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newEnemy.entity.animation.animationFrameHeight = 122;//imageHeight;
    newEnemy.entity.animation.currentAnimationFrame = 0;
    newEnemy.entity.animation.currentAnimationState = IDLE;
    newEnemy.entity.animation.isFacingRight = 1;
    newEnemy.entity.animation.timeSinceLastFrame = 0;
    newEnemy.entity.animation.currentAnimationFrameRect.x = 0.0f;
    newEnemy.entity.animation.currentAnimationFrameRect.y = 0.0f;
    newEnemy.entity.animation.currentAnimationFrameRect.width = newEnemy.entity.animation.animationFrameWidth;
    newEnemy.entity.animation.currentAnimationFrameRect.height = newEnemy.entity.animation.animationFrameHeight;
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

void UpdatePlayer(Player *player, Enemy *enemy, float delta, Props *props, float minX) {
    enum CHARACTER_STATE currentState = player->entity.animation.currentAnimationState;
    player->entity.animation.timeSinceLastFrame += delta;

    if (player->entity.animation.currentAnimationState != DYING && player->entity.animation.currentAnimationState != HURT) {
        if (IsKeyDown(KEY_LEFT)) {
            player->entity.position.x -= player->entity.walkSpeed*delta;
            player->entity.animation.isFacingRight = -1;
            player->entity.animation.currentAnimationState = WALKING;
            
        } else if (IsKeyDown(KEY_RIGHT)) {
            player->entity.position.x += player->entity.walkSpeed*delta;
            player->entity.animation.isFacingRight = 1;
            player->entity.animation.currentAnimationState = WALKING;
        } else {
            player->entity.animation.currentAnimationState = IDLE;
        }

        if (IsKeyDown(KEY_SPACE) && player->entity.isGrounded) 
        {
            player->entity.velocity.y = -player->entity.jumpSpeed;
            player->entity.isGrounded = false;
        }

        if (IsKeyPressed(KEY_H))
            player->entity.animation.currentAnimationState = HURT;

        if (IsKeyPressed(KEY_P))
            player->entity.currentHP = 0;
        
    }

    // Collision check
    int hitObstacle = 0;
    int hasFloorBelow = 0;
    Rectangle prect = {player->entity.position.x, player->entity.position.y, player->entity.animation.animationFrameWidth, player->entity.animation.animationFrameHeight};
    Rectangle prectGrav = {player->entity.position.x, player->entity.position.y+1, player->entity.animation.animationFrameWidth, player->entity.animation.animationFrameHeight};
    for (int i = 0; i < 1; i++)  // TODO 1 is "props[]"'s size
    {
        Props *eprop = props + i;
        Vector2 *p = &(player->entity.position);
        if (eprop->canBeStepped) {
            if (CheckCollisionRecs(eprop->rect, prect)) {
                hitObstacle = 1;
                player->entity.velocity.y = 0.0f;
                p->y = eprop->rect.y - player->entity.animation.animationFrameHeight;
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
    if ((player->entity.animation.currentAnimationState != DYING) && (player->entity.animation.currentAnimationState != HURT)) {
        if (!player->entity.isGrounded) {
            if (player->entity.velocity.y < 0) {
                player->entity.animation.currentAnimationState = JUMPING;
            } else if(player->entity.velocity.y > 0) {
                player->entity.animation.currentAnimationState = FALLING;
            }
        }
    }

    // Atualização de estado quando o player morre
    if (player->entity.currentHP <= 0 && !(player->entity.animation.currentAnimationState == DYING)) {
        player->entity.animation.currentAnimationState = DYING;
    }

    // Update player animation frame Rect
    int animRow = GetAnimRow(&(player->entity), delta, currentState);
    player->entity.animation.currentAnimationFrameRect.x = (float)player->entity.animation.currentAnimationFrame * player->entity.animation.animationFrameWidth;
    player->entity.animation.currentAnimationFrameRect.y = animRow * player->entity.animation.animationFrameHeight;
    player->entity.animation.currentAnimationFrameRect.width = player->entity.animation.isFacingRight * player->entity.animation.animationFrameWidth;

    // Limitar posição do player de acordo com o avanço da câmera
    if (player->entity.position.x < minX) {
        player->entity.position.x = minX;
    } 
}

void UpdateEnemy(Enemy *enemy, Player *player, float delta, Props *props) {
    Entity *eEnt = &(enemy->entity);
    enum CHARACTER_STATE currentState = eEnt->animation.currentAnimationState;
    eEnt->animation.timeSinceLastFrame += delta;
    enemy->timeSinceLastBehaviorChange += delta;

    // Steering behavior
    RangedSteeringBehavior(enemy, player, delta);
    
    // Colisão
    {   
    int hitObstacle = 0;
    int hasFloorBelow = 0;
    Rectangle prect = {eEnt->position.x, eEnt->position.y, eEnt->animation.animationFrameWidth, eEnt->animation.animationFrameHeight};
    Rectangle prectGrav = {eEnt->position.x, eEnt->position.y+1, eEnt->animation.animationFrameWidth, eEnt->animation.animationFrameHeight};
    for (int i = 0; i < 1; i++)  // TODO 1 is "props[]"'s size
    {
        Props *eprop = props + i;
        Vector2 *p = &(eEnt->position);
        if (eprop->canBeStepped) {
            if (CheckCollisionRecs(eprop->rect, prect)) {
                hitObstacle = 1;
                eEnt->velocity.y = 0.0f;
                p->y = eprop->rect.y - eEnt->animation.animationFrameHeight;
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
    }

    // Atualização da física
    eEnt->velocity.x += eEnt->animation.isFacingRight*eEnt->momentum.x * delta;
    if (eEnt->velocity.x > eEnt->walkSpeed) {
        eEnt->velocity.x = eEnt->walkSpeed;
    } else if (eEnt->velocity.x < -eEnt->walkSpeed) {
        eEnt->velocity.x = -eEnt->walkSpeed;
    }
    eEnt->position.x += eEnt->velocity.x * delta;

    // Atualização de Estado
    if ((eEnt->animation.currentAnimationState != DYING) && (eEnt->animation.currentAnimationState != HURT)) {
        if (eEnt->animation.currentAnimationState != ATTACKING) {
            if (eEnt->isGrounded) {
                if (eEnt->velocity.x != 0) {
                    eEnt->animation.currentAnimationState = WALKING;
                } else {
                    eEnt->animation.currentAnimationState = IDLE;
                }
            } else if (!eEnt->isGrounded) {
                if (eEnt->velocity.y < 0) {
                    eEnt->animation.currentAnimationState = JUMPING;
                } else if(eEnt->velocity.y > 0) {
                    eEnt->animation.currentAnimationState = FALLING;
                }
            }
        } else {

        }
    }
    
    // Atualização de estado quando o inimigo morre
    if (eEnt->currentHP <= 0 && !(eEnt->animation.currentAnimationState == DYING)) {
        eEnt->animation.currentAnimationState = DYING;
    }

    // Update enemy animation frame Rect
    int animRow = GetAnimRow(eEnt, delta, currentState);
    eEnt->animation.currentAnimationFrameRect.x = (float)eEnt->animation.currentAnimationFrame * eEnt->animation.animationFrameWidth;
    eEnt->animation.currentAnimationFrameRect.y = animRow * eEnt->animation.animationFrameHeight;
    eEnt->animation.currentAnimationFrameRect.width = eEnt->animation.isFacingRight * eEnt->animation.animationFrameWidth;

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