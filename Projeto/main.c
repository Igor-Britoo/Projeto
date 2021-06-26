#include "gameConfig.c"

int main(void) {
    if (isFullscreen) SetConfigFlags(FLAG_FULLSCREEN_MODE); // Fullscreen
    InitWindow(screenWidth, screenHeight, gameName);
    SetTargetFPS(60);

    // Load assets
    Texture2D backgroundTex = LoadTexture("resources/Background/cyberpunk_street_background.png");
    Texture2D midgroundTex = LoadTexture("resources/Background/cyberpunk_street_midground.png");
    Texture2D foregroundTex = LoadTexture("resources/Background/cyberpunk_street_foreground.png");
    Texture2D characterTex = LoadTexture("resources/Atlas/hero_atlas.png");    
    Texture2D miscAtlas = LoadTexture("resources/Atlas/misc_atlas.png");        
    Texture2D enemyTex[6]; // 6 = numEnemyClasses
    enemyTex[SWORDSMAN] = LoadTexture("resources/Atlas/hero_atlas.png");
    enemyTex[GUNNER] = LoadTexture("resources/Atlas/hero_atlas.png");
    enemyTex[SNIPERSHOOTER] = LoadTexture("resources/Atlas/hero_atlas.png");
    enemyTex[DRONE] = LoadTexture("resources/Atlas/hero_atlas.png");
    enemyTex[TURRET] = LoadTexture("resources/Atlas/hero_atlas.png");
    enemyTex[BOSS] = LoadTexture("resources/Atlas/hero_atlas.png");

    // Usado para Debbug
    Vector2 mouseClick = (Vector2){0, 0};

    // Player Init
    Player player = CreatePlayer(100, (Vector2){122, 200},122, 122);

    // Camera init
    float camMinX = 0; // Usado no avanço da câmera e na limitação de movimentação para trás do player
    float camMaxX = 0; // Usado no avanço da câmera
    Camera2D camera = CreateCamera(player.entity.position, (Vector2) {screenWidth/2.0f, screenHeight/2.0f}, 0.0f, 1.00f);

    // General Init
    Bullet bulletsPool[100]; // 100 -> maxNumBullets
    for (int i = 0; i < maxNumBullets; i++) {
        bulletsPool[i].isActive = false;
    }

    Background nearBackgroundPool[3], middleBackgroundPool[3], farBackgroundPool[3]; // Pool de backgrounds. Sempre que um "sai" da tela, é destruído e um novo é gerado no "final" do mapa, mantendo sempre apenas 3 chunks
    int numNearBackground = 0, numMiddleBackground = 0, numFarBackground = 0; // Usado para posicionamento correto das novas imagens geradas
    for (int i = 0; i < numBackgroundRendered; i++) {
        farBackgroundPool[i] = CreateBackground(&player, farBackgroundPool, BACKGROUND, &numFarBackground, 2.0f);
        middleBackgroundPool[i] = CreateBackground(&player, middleBackgroundPool, MIDDLEGROUND, &numMiddleBackground, 2.0f);
        nearBackgroundPool[i] = CreateBackground(&player, nearBackgroundPool, FOREGROUND, &numNearBackground, 2.0f);
    }
    Props propsPool[20];
    for (int i = 0; i < maxNumProps; i++) {
        propsPool[i].isActive = false;
    } 
    propsPool[0] = (Props){(Rectangle){0,430,screenWidth*3,5}, 1, 0, 1};

    // Enemy Init
    Enemy enemyPool[20]; // maxNumEnemies
    for (int i = 0; i < maxNumEnemies; i++) {
        enemyPool[i].isAlive = false;
    }
    enemyPool[0] = CreateEnemy(BOSS,500, (Vector2){750, 150},122, 122);

    // Main game loop
    while (!WindowShouldClose()) {
        // Usado para debbug
        /*
        Vector2 mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(0)) mouseClick = mousePosition;
        */

        // Updates
        float deltaTime = GetFrameTime();

        // Atualizar balas
        for (int i = 0; i < maxNumBullets; i++) {
            if (bulletsPool[i].isActive)
                UpdateBullets(&bulletsPool[i], enemyPool, &player, propsPool, deltaTime);
        }

        // Atualizar player
        UpdatePlayer(&player, enemyPool, bulletsPool, deltaTime, propsPool, camMinX);

        // Atualizar limites de câmera e posição
        camMinX = (camMinX < camera.target.x - camera.offset.x ? camera.target.x - camera.offset.x : camMinX);
        UpdateClampedCameraPlayer(&camera, &player, propsPool, deltaTime, screenWidth, screenHeight, &camMinX, camMaxX);

        // Atualizar props
        for (int i = 0; i < maxNumProps; i++) {
            if (propsPool[i].isActive)
                UpdateProps(&player, propsPool, deltaTime, camMinX);
        }

        // Loop dentro da função TODO
        UpdateBackground(&player, nearBackgroundPool, deltaTime, &numNearBackground, camMinX, &camMaxX);
        UpdateBackground(&player, middleBackgroundPool, deltaTime, &numMiddleBackground, camMinX, &camMaxX);
        UpdateBackground(&player, farBackgroundPool,deltaTime, &numFarBackground, camMinX, &camMaxX);


        // Atualizar inimigos
        for (int i = 0; i < maxNumEnemies; i++) {
            if (enemyPool[i].isAlive)
                UpdateEnemy(enemyPool, &player, bulletsPool, deltaTime, propsPool);
        }

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

                // Draw bullets
                for (int i = 0; i < maxNumBullets; i++) {
                    if (bulletsPool[i].isActive) {
                        DrawBullet(&bulletsPool[i], miscAtlas, false); //bulletspool, miscAtlas, colisão                        
                    }
                }

                // Draw player
                float charWidth = player.entity.animation.animationFrameWidth*player.entity.characterWidthScale;
                float charHeight= player.entity.animation.animationFrameHeight*player.entity.characterHeightScale;
                Rectangle dstRect = {player.entity.position.x - charWidth/2, player.entity.position.y - charHeight/2, charWidth, charHeight};
                DrawTexturePro(characterTex, player.entity.animation.currentAnimationFrameRect, dstRect, (Vector2) {0, 0}, 0, WHITE);

                // Draw Enemy
                for (int i = 0; i < maxNumEnemies; i++) {
                    if (enemyPool[i].isAlive) 
                        DrawEnemy(&enemyPool[i], enemyTex, false, false, false); //enemypool, enemytex, detecção, vida, colisão
                }
                

                // Draw props
                for (int i = 0; i < maxNumProps; i++) {
                    if (propsPool[i].isActive)
                        if (!propsPool[i].isInvisible)
                            DrawRectangleRec(propsPool[i].rect, WHITE);
                }


            EndMode2D();

            // TODO o que está nessa região fica "parado" em relação à câmera
            char state[30];
            sprintf(state, "%d", player.entity.isGrounded);
            DrawText(state, 0, 600, 20, RED);
            sprintf(state, "%f", player.entity.position.y);
            DrawText(state, 0, 625, 20, RED);
    

    ////////////////////////////////////////////////// INÍCIO DE DEBBUG //////////////////////////////////////////////////
    /*
    char state[30];
    switch (player.entity.animation.currentAnimationState) {
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
    //DrawText(state, 0, 625, 20, RED);

    sprintf(state, "%f", enemies[0].entity.position.x);
    //DrawText("ePx:", 0, 650, 20, RED);
    //DrawText(state, 100, 650, 20, RED);
    sprintf(state, "%f", enemies[0].target.x);
    //DrawText("tPx", 0, 675, 20, RED);
    //DrawText(state, 100, 675, 20, RED);
    sprintf(state, "%f", enemies[0].noDetectionTime);
    //DrawText("nDT:", 0, 700, 20, RED);
    //DrawText(state, 100, 700, 20, RED);
*/
    ////////////////////////////////////////////////// FIM DE DEBBUG //////////////////////////////////////////////////

        EndDrawing();
    }

    // Unload
    UnloadTexture(backgroundTex);
    UnloadTexture(midgroundTex);
    UnloadTexture(foregroundTex);
    UnloadTexture(characterTex);
    for (int i = 0; i < numEnemyClasses; i++)
        UnloadTexture(enemyTex[i]);

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
        dstBackground.relativePosition = -0.05f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    case MIDDLEGROUND:
        dstBackground.width = 512*scale;
        dstBackground.height = 192*scale;
        dstBackground.position.y = 20;
        dstBackground.relativePosition = -0.025f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    case FOREGROUND:
        dstBackground.width = 704*scale;
        dstBackground.height = 192*scale;
        dstBackground.position.y = 70;
        dstBackground.relativePosition = 0.0f; // Velocidade do parallax (quanto menor, mais lento)
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
    newPlayer.currentWeaponID = -1;
    newPlayer.currentAmmo = 0;
    newPlayer.entity.maxXSpeed = 400;
    newPlayer.entity.sprintSpeed = 800;
    newPlayer.entity.jumpSpeed = 250;
    newPlayer.entity.isGrounded = false;
    newPlayer.entity.eyesOffset = (Vector2) {55, 40};

    newPlayer.entity.width = width;
    newPlayer.entity.height = height;
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

    newPlayer.entity.drawableRect = (Rectangle) {position.x - width/2, position.y - height/2, width * newPlayer.entity.characterWidthScale, height * newPlayer.entity.characterHeightScale};
    newPlayer.entity.collisionBox = (Rectangle) {position.x - width/2, position.y - height/2, width * 0.8f, height};
    newPlayer.entity.collisionHead = (Circle) {(Vector2){position.x - width/2, position.y - height/2}, width * 0.8f};

    return newPlayer;
}

Enemy CreateEnemy(enum ENEMY_CLASSES class, int maxHP, Vector2 position, int width, int height) {
    Enemy newEnemy;

    newEnemy.target = (Vector2){-1, -1};
    newEnemy.class = class;
    newEnemy.behavior = NONE;
    newEnemy.behaviorChangeInterval = 3.5f; // Tempo em segundos para tentar alterar comportamento
    newEnemy.timeSinceLastBehaviorChange = 0;
    newEnemy.noDetectionTime = 0;
    newEnemy.loseTargetInterval = 5;
    newEnemy.spawnLocation = (Vector2){position.x, position.y};
    newEnemy.maxDistanceToSpawn = 1000;
    newEnemy.isAlive = true;

    newEnemy.entity.position = newEnemy.spawnLocation;
    newEnemy.entity.velocity.x = 0.0f;
    newEnemy.entity.velocity.y = 0.0f;
    newEnemy.entity.momentum.x = 0.0f;
    newEnemy.entity.momentum.y = 0.0f;
    newEnemy.entity.maxXSpeed = 200;
    newEnemy.entity.sprintSpeed = 800;
    newEnemy.entity.jumpSpeed = 250;
    newEnemy.entity.isGrounded = false;
    newEnemy.entity.eyesOffset = (Vector2) {55, 40};

    newEnemy.entity.width = width;
    newEnemy.entity.height = height;
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

    newEnemy.entity.drawableRect = (Rectangle) {position.x - width/2, position.y - height/2, width * newEnemy.entity.characterWidthScale, height * newEnemy.entity.characterHeightScale};
    newEnemy.entity.collisionBox = (Rectangle) {position.x - width/2, position.y - height/2, width * 0.8f, height};
    newEnemy.entity.collisionHead = (Circle) {(Vector2){position.x - width/2, position.y - height/2}, width * 0.8f};

    //Valores para range de ataque e de visão selecionados de forma arbitraria, atualizar posteriormente
    switch (class){
        case SWORDSMAN:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 0;
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case GUNNER:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case SNIPERSHOOTER:
            newEnemy.viewDistance = 1000;
            newEnemy.attackRange = 1000;
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case DRONE:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case TURRET:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case BOSS:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.entity.maxHP = maxHP*2;
            newEnemy.entity.currentHP = maxHP*2;
            break;
        default:
            break;
}

    return newEnemy;
}

void CreateBullet(Entity *entity, Bullet *bulletsPool, enum BULLET_TYPE bulletType, enum ENTITY_TYPES srcEntity) {
    // Procurar lugar vago na pool
    for (int i = 0; i < maxNumBullets; i++) {
        Bullet *bullet_i = bulletsPool + i;
        if (!bullet_i->isActive) {
            bullet_i->position = entity->position;
            bullet_i->srcEntity = srcEntity;
            bullet_i->bulletType = bulletType;
            bullet_i->direction = entity->animation.isFacingRight;
            bullet_i->width = 20; // Tem que tunar
            bullet_i->height = 7; // Tem que tunar
            bullet_i->power = 40; // Tem que tunar
            bullet_i->lifeTime = 0;
            bullet_i->isActive = true;

            bullet_i->animation.animationFrameSpeed = 0.08f;
            bullet_i->animation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
            bullet_i->animation.animationFrameHeight = 122;//imageHeight;
            bullet_i->animation.currentAnimationFrame = 0;
            bullet_i->animation.isFacingRight = bullet_i->direction;
            bullet_i->animation.timeSinceLastFrame = 0;
            bullet_i->animation.currentAnimationFrameRect.x = 0.0f;
            bullet_i->animation.currentAnimationFrameRect.y = 0.0f;
            bullet_i->animation.currentAnimationFrameRect.width = bullet_i->animation.animationFrameWidth;
            bullet_i->animation.currentAnimationFrameRect.height = bullet_i->animation.animationFrameHeight;

            bullet_i->drawableRect = (Rectangle) {bullet_i->position.x - bullet_i->animation.currentAnimationFrameRect.width/2, bullet_i->position.y - bullet_i->animation.currentAnimationFrameRect.height/2, bullet_i->animation.currentAnimationFrameRect.width, bullet_i->animation.currentAnimationFrameRect.height};
            bullet_i->collisionBox = (Rectangle) {bullet_i->position.x - bullet_i->width/2, bullet_i->position.y - bullet_i->height/2, bullet_i->width, bullet_i->height};
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

void UpdatePlayer(Player *player, Enemy *enemy, Bullet *bulletPool, float delta, Props *props, float minX) {
    enum CHARACTER_STATE currentState = player->entity.animation.currentAnimationState;
    player->entity.animation.timeSinceLastFrame += delta;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de input do player                                     ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (player->entity.animation.currentAnimationState != DYING && player->entity.animation.currentAnimationState != HURT && player->entity.animation.currentAnimationState != ATTACKING) {
        if (IsKeyDown(KEY_LEFT)) {
            player->entity.velocity.x -= player->entity.maxXSpeed;
            
        } else if (IsKeyDown(KEY_RIGHT)) {
            player->entity.velocity.x += player->entity.maxXSpeed;
        } else {
            player->entity.velocity.x = 0;
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

        if (IsKeyPressed(KEY_R)) {
            CreateBullet(&(player->entity), bulletPool, MAGNUM, PLAYER);
            player->entity.animation.currentAnimationState = ATTACKING;
            player->entity.animation.currentAnimationFrame = 0;
            player->entity.animation.timeSinceLastFrame = 0;
            if (player->entity.isGrounded)
                player->entity.velocity.x = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de colisão do player                                   ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EntityCollisionHandler(&(player->entity), props, delta);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de física e gráfico do player                          ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsAndGraphicsHandlers(&(player->entity), delta, currentState);

    // Limitar posição do player de acordo com o avanço da câmera
    if (player->entity.position.x < minX + player->entity.width/2) {
        player->entity.position.x = minX + player->entity.width/2;
    } 

    // Atualizar caixa de colisão e retângulo de desenho
    player->entity.drawableRect = (Rectangle) {player->entity.position.x - player->entity.width/2, player->entity.position.y - player->entity.height/2, (int)(player->entity.width * player->entity.characterWidthScale), (int)(player->entity.height * player->entity.characterHeightScale)};
    player->entity.collisionBox = (Rectangle) {player->entity.position.x  - player->entity.width/2 + (player->entity.animation.isFacingRight == -1 ? 0.3f : 0.15f) * player->entity.width, player->entity.position.y - player->entity.height/2, player->entity.width * 0.5f, player->entity.height};
    player->entity.collisionHead = (Circle) {(Vector2){player->entity.position.x - player->entity.animation.isFacingRight * 0.1f * player->entity.width, player->entity.position.y - 0.15f * player->entity.height}, player->entity.width * 0.2f};
}

void UpdateEnemy(Enemy *enemy, Player *player, Bullet *bulletPool, float delta, Props *props) {
    Entity *eEnt = &(enemy->entity);
    enum CHARACTER_STATE currentState = eEnt->animation.currentAnimationState;
    eEnt->animation.timeSinceLastFrame += delta;
    enemy->timeSinceLastBehaviorChange += delta;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de comportamento do enemy                              ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SteeringBehavior(enemy, player, bulletPool, delta);
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de colisão do enemy                                    ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EntityCollisionHandler(&(enemy->entity), props, delta);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de física e gráfico do enemy                           ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PhysicsAndGraphicsHandlers(&(enemy->entity), delta, currentState);

    //eEnt->drawableRect = (Rectangle) {eEnt->position.x - eEnt->width/2, eEnt->position.y - eEnt->height/2, 122,122};
    eEnt->drawableRect = (Rectangle) {eEnt->position.x - eEnt->width/2, eEnt->position.y - eEnt->height/2, eEnt->width * eEnt->characterWidthScale,eEnt->height * eEnt->characterHeightScale};
    eEnt->collisionBox = (Rectangle) {eEnt->position.x  - eEnt->width/2 + (eEnt->animation.isFacingRight == -1 ? 0.3f : 0.15f) * eEnt->width, eEnt->position.y - eEnt->height/2, eEnt->width * 0.5f, eEnt->height};
    eEnt->collisionHead = (Circle) {(Vector2){eEnt->position.x - eEnt->animation.isFacingRight * 0.1f * eEnt->width, eEnt->position.y - 0.15f * eEnt->height}, eEnt->width * 0.2f};
}
     
void UpdateBullets(Bullet *bullet, Enemy *enemy, Player *player, Props *props, float delta) {
    bullet->lifeTime += delta;
    bullet->animation.timeSinceLastFrame += delta;
    // Checar colisão
    // Props
    for (int i = 0; i < maxNumProps; i++)
    {
        Props *eprop = props + i;
        if (eprop->isActive) {
            if (CheckCollisionRecs(eprop->rect, bullet->collisionBox)) {
                bullet->isActive = false;
            }
        }
    }
    // Colisão com inimigos
    if (bullet->srcEntity == PLAYER) {
        for (int i = 0; i < maxNumEnemies; i++)
        {
            Enemy *currentEnemy = enemy + i;
            if (currentEnemy->isAlive) {
                if (currentEnemy->entity.animation.currentAnimationState != DYING) {
                    if (CheckCollisionRecs(currentEnemy->entity.collisionBox, bullet->collisionBox) || CheckCollisionCircleRec(currentEnemy->entity.collisionHead.center, currentEnemy->entity.collisionHead.radius, bullet->collisionBox)) {
                        bullet->isActive = false;
                        HurtEntity(&(currentEnemy->entity), *bullet, 40); // TODO damage
                    }
                }
            }
        }
    }

    // Colisão com Player
    if (bullet->srcEntity == ENEMY) {
        Rectangle playerRect = (Rectangle) {player->entity.position.x, player->entity.position.y, player->entity.animation.animationFrameWidth, player->entity.animation.animationFrameHeight};
        if (CheckCollisionRecs(player->entity.collisionBox, bullet->collisionBox) || CheckCollisionCircleRec(player->entity.collisionHead.center, player->entity.collisionHead.radius, bullet->collisionBox)) {
            bullet->isActive = false;
            // TODO Causa dano ao player
            // TODO Criar animação de sangue
        }
    }

    // Se não tiver colisão, checar tempo de vida e atualizar posição
    if (bullet->lifeTime >= bulletLifeTime) {
        bullet->isActive = false;
    } else {
        bullet->position.x += 2500 * delta * bullet->direction;
        // TODO fazer partícula
        // Animação
        if (bullet->animation.timeSinceLastFrame >= bullet->animation.animationFrameSpeed) {
            bullet->animation.timeSinceLastFrame = 0.0f;
            bullet->animation.currentAnimationFrame++;
            if (bullet->animation.currentAnimationFrame > 1) {  // 1 porque são 2 frames
                bullet->animation.currentAnimationFrame = 0;
            }
        }
        bullet->animation.currentAnimationFrameRect.x = (float)bullet->animation.currentAnimationFrame * bullet->animation.animationFrameWidth;
        bullet->animation.currentAnimationFrameRect.y = 0;
        bullet->animation.currentAnimationFrameRect.width = bullet->animation.isFacingRight * bullet->animation.animationFrameWidth;

        bullet->drawableRect = (Rectangle) {bullet->position.x - bullet->animation.currentAnimationFrameRect.width/2, bullet->position.y - bullet->animation.currentAnimationFrameRect.height/2, 122, 122};
        bullet->collisionBox = (Rectangle) {bullet->position.x + 40, bullet->position.y - 12, bullet->width, bullet->height};
    }

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

void UpdateBackground(Player *player, Background *backgroundPool, float delta, int *numBackground, float minX, float *maxX) {

    for (int i = 0; i < numBackgroundRendered; i++) {
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

void DrawEnemy(Enemy *enemy, Texture2D *texture, bool drawDetectionCollision, bool drawLife, bool drawCollisionBox) {
    // Draw inimigos

    // Draw campo de visão
    if (drawDetectionCollision) {
        float eyesX = enemy->entity.position.x + enemy->entity.eyesOffset.x;
        float eyesY = enemy->entity.position.y + enemy->entity.eyesOffset.y;
        Rectangle detectionBox;
        if (enemy->entity.animation.isFacingRight == 1) {
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
        DrawRectangle(enemy->entity.drawableRect.x, enemy->entity.drawableRect.y, enemy->entity.drawableRect.width, enemy->entity.drawableRect.height, RED);
        DrawRectangle(enemy->entity.collisionBox.x, enemy->entity.collisionBox.y, enemy->entity.collisionBox.width, enemy->entity.collisionBox.height, WHITE);
        DrawCircle(enemy->entity.collisionHead.center.x, enemy->entity.collisionHead.center.y, enemy->entity.collisionHead.radius, WHITE);
    }
    DrawTexturePro(texture[enemy->class], enemy->entity.animation.currentAnimationFrameRect, enemy->entity.drawableRect, (Vector2) {0, 0}, 0, WHITE);

    char state[30];
    sprintf(state, "%f", enemy->entity.drawableRect.x);
    DrawText(state, 0, 650, 20, BLUE);
    sprintf(state, "%f", enemy->entity.drawableRect.x);
    DrawText(state, 0, 675, 20, BLUE);
    sprintf(state, "%f", enemy->entity.drawableRect.width);
    DrawText(state, 0, 700, 20, BLUE);
    sprintf(state, "%f", enemy->entity.drawableRect.height);
    DrawText(state, 0, 725, 20, BLUE);
}

void DrawBullet(Bullet *bullet, Texture2D texture, bool drawCollisionBox) {
    DrawTexturePro(texture, bullet->animation.currentAnimationFrameRect, bullet->drawableRect, (Vector2) {0, 0}, 0, WHITE);
    // Draw das caixas de colisão
    if (drawCollisionBox) {
        DrawRectangle(bullet->collisionBox.x, bullet->collisionBox.y, bullet->collisionBox.width, bullet->collisionBox.height, RED);
    }

    char state[30];
    sprintf(state, "%f", bullet->position.x);
    DrawText(state, 0, 425, 20, RED);
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