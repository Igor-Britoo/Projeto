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
    Texture2D characterTexDiv = LoadTexture("resources/Atlas/hero_atlas_div.png");    
    Texture2D miscAtlas = LoadTexture("resources/Atlas/misc_atlas.png");        
    Texture2D backgroundAtlas = LoadTexture("resources/Atlas/background_atlas.png");        
    Texture2D midgroundAtlas = LoadTexture("resources/Atlas/midground_atlas.png");        
    //Texture2D foregroundAtlas = LoadTexture("resources/Background/cyberpunk_street_foreground.png");     
    Texture2D foregroundAtlas = LoadTexture("resources/Atlas/foreground_atlas.png");
    Texture2D *enemyTex = (Texture2D *)malloc(numEnemyClasses*sizeof(Texture2D));
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
    Bullet *bulletsPool = (Bullet *)malloc(maxNumBullets*sizeof(Bullet));

    for (int i = 0; i < maxNumBullets; i++) {
        bulletsPool[i].isActive = false;
    }

    // Pool de backgrounds. Sempre que um "sai" da tela, é destruído e um novo é gerado no "final" do mapa, mantendo sempre apenas 3 chunks
    Background *nearBackgroundPool = (Background *)malloc(numBackgroundRendered*sizeof(Background));
    Background *middleBackgroundPool = (Background *)malloc(numBackgroundRendered*sizeof(Background));
    Background *farBackgroundPool = (Background *)malloc(numBackgroundRendered*sizeof(Background));
    int numNearBackground = 0, numMiddleBackground = 0, numFarBackground = 0; // Usado para posicionamento correto das novas imagens geradas
    for (int i = 0; i < numBackgroundRendered; i++) {
        farBackgroundPool[i] = CreateBackground(&player, farBackgroundPool, backgroundAtlas, BACKGROUND, &numFarBackground, 2.0f);
        middleBackgroundPool[i] = CreateBackground(&player, middleBackgroundPool, midgroundAtlas, MIDDLEGROUND, &numMiddleBackground, 2.0f);
        nearBackgroundPool[i] = CreateBackground(&player, nearBackgroundPool, foregroundAtlas, FOREGROUND, &numNearBackground, 2.0f);
    }

    Props *propsPool = (Props *)malloc(maxNumProps*sizeof(Props));
    for (int i = 0; i < maxNumProps; i++) {
        propsPool[i].isActive = false;
    } 
    propsPool[0] = (Props){(Rectangle){0,screenHeight-150,screenWidth*3,5}, 1, 0, 1, true};

    // Enemy Init
    Enemy *enemyPool = (Enemy *)malloc(maxNumEnemies*sizeof(Enemy));
    for (int i = 0; i < maxNumEnemies; i++) {
        enemyPool[i].isAlive = false;
    }
    //enemyPool[0] = CreateEnemy(BOSS, 500, (Vector2) {500, 300}, 122, 122);

    // Main game loop
    while (!WindowShouldClose()) {

        // Usado para debbug
        //Vector2 mousePosition = GetMousePosition();

        //if (IsMouseButtonPressed(0)) mouseClick = mousePosition;
        

        // Updates
        float deltaTime = GetFrameTime();

        // Atualizar player
        UpdatePlayer(&player, enemyPool, bulletsPool, deltaTime, propsPool, camMinX);

        // Atualizar balas
        for (int i = 0; i < maxNumBullets; i++) {
            if (bulletsPool[i].isActive)
                UpdateBullets(&bulletsPool[i], enemyPool, &player, propsPool, deltaTime);
        }

        // Atualizar limites de câmera e posição
        camMinX = (camMinX < camera.target.x - camera.offset.x ? camera.target.x - camera.offset.x : camMinX);
        UpdateClampedCameraPlayer(&camera, &player, propsPool, deltaTime, screenWidth, screenHeight, &camMinX, camMaxX);

        // Atualizar props
        for (int i = 0; i < maxNumProps; i++) {
            if (propsPool[i].isActive)
                UpdateProps(&player, propsPool, deltaTime, camMinX);
        }

        // Loop dentro da função TODO
        UpdateBackground(&player, nearBackgroundPool, foregroundAtlas, deltaTime, &numNearBackground, camMinX, &camMaxX);
        UpdateBackground(&player, middleBackgroundPool, midgroundAtlas, deltaTime, &numMiddleBackground, camMinX, &camMaxX);
        UpdateBackground(&player, farBackgroundPool, backgroundAtlas, deltaTime, &numFarBackground, camMinX, &camMaxX);


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

                // Draw player
                DrawPlayer(&player, characterTexDiv, false);

                // Draw Enemy
                for (int i = 0; i < maxNumEnemies; i++) {
                    if (enemyPool[i].isAlive) 
                        DrawEnemy(&enemyPool[i], enemyTex, false, false, false); //enemypool, enemytex, detecção, vida, colisão
                }

                // Draw bullets
                for (int i = 0; i < maxNumBullets; i++) {
                    if (bulletsPool[i].isActive) {
                        DrawBullet(&bulletsPool[i], miscAtlas, false); //bulletspool, miscAtlas, colisão                        
                    }
                }
                
                // Draw props
                for (int i = 0; i < maxNumProps; i++) {
                    if (propsPool[i].isActive)
                        if (!propsPool[i].isInvisible)
                            DrawRectangleRec(propsPool[i].rect, WHITE);
                }

        EndMode2D();

        //Chão
        DrawRectangle(0, screenHeight-150, screenWidth, 150, LIGHTGRAY);
        // TODO o que está nessa região fica "parado" em relação à câmera
        
        //HUD deve vir aqui
            char state[50];
            //sprintf(state, "%d", player.entity.upperAnimation.currentAnimationFrame);
            //DrawText(state, 0, 675, 20, GREEN);
            switch (player.entity.upperAnimation.currentAnimationState) {
            case IDLE:
                strcpy(state, "UPPER ANIMATION STATE: IDLE");
                break;
            case WALKING:
                strcpy(state, "UPPER ANIMATION STATE: WALKING");
                break;
            case JUMPING:
                strcpy(state, "UPPER ANIMATION STATE: JUMPING");
                break;
            case FALLING:
                strcpy(state, "UPPER ANIMATION STATE: FALLING");
                break;
            case ATTACKING:
                strcpy(state, "UPPER ANIMATION STATE: ATTACKING");
                break;
            default:
                break;
            }
            DrawText(state, 0, 0, 20, RED);
            switch (player.entity.lowerAnimation.currentAnimationState) {
            case IDLE:
                strcpy(state, "LOWER ANIMATION STATE: IDLE");
                break;
            case WALKING:
                strcpy(state, "LOWER ANIMATION STATE: WALKING");
                break;
            case JUMPING:
                strcpy(state, "LOWER ANIMATION STATE: JUMPING");
                break;
            case FALLING:
                strcpy(state, "LOWER ANIMATION STATE: FALLING");
                break;
            case ATTACKING:
                strcpy(state, "LOWER ANIMATION STATE: ATTACKING");
                break;
            default:
                break;
            }
            DrawText(state, 0, 25, 20, RED);

        EndDrawing();


    }

    // Unload
    UnloadTexture(backgroundTex);
    UnloadTexture(midgroundTex);
    UnloadTexture(foregroundTex);
    UnloadTexture(characterTex);
    UnloadTexture(characterTexDiv);
    UnloadTexture(miscAtlas);
    for (int i = 0; i < numBackgroundRendered; i++) {
        UnloadRenderTexture(farBackgroundPool[i].canvas);
        UnloadRenderTexture(nearBackgroundPool[i].canvas);
        UnloadRenderTexture(middleBackgroundPool[i].canvas);
    }
    for (int i = 0; i < numEnemyClasses; i++)
        UnloadTexture(enemyTex[i]);

    CloseWindow();
    return 0;
}

Background CreateBackground(Player *player, Background *backgroundPool, Texture2D srcAtlas, enum BACKGROUND_TYPES bgType, int *numBackground, float scale) {
    Background dstBackground;
    int numBg = *numBackground;

    switch (bgType)
    {
    case BACKGROUND:
        dstBackground.relativePosition = -0.05f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    case MIDDLEGROUND:
        dstBackground.relativePosition = -0.025f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    case FOREGROUND:
        dstBackground.relativePosition = 0.0f; // Velocidade do parallax (quanto menor, mais lento)
        break;
    }


    dstBackground.position.y = 0;
    dstBackground.canvas = PaintCanvas(srcAtlas, bgType);
    dstBackground.width = dstBackground.canvas.texture.width;
    dstBackground.height = dstBackground.canvas.texture.height;
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
    newPlayer.entity.upperAnimation.animationFrameSpeed = 0.08f;
    newPlayer.entity.upperAnimation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newPlayer.entity.upperAnimation.animationFrameHeight = 122;//imageHeight;
    newPlayer.entity.upperAnimation.currentAnimationFrame = 0;
    newPlayer.entity.upperAnimation.currentAnimationState = IDLE;
    newPlayer.entity.upperAnimation.isFacingRight = 1;
    newPlayer.entity.upperAnimation.timeSinceLastFrame = 0;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.x = 0.0f;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.y = 0.0f;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.width = newPlayer.entity.upperAnimation.animationFrameWidth;
    newPlayer.entity.upperAnimation.currentAnimationFrameRect.height = newPlayer.entity.upperAnimation.animationFrameHeight;
    newPlayer.entity.lowerAnimation.animationFrameSpeed = 0.08f;
    newPlayer.entity.lowerAnimation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newPlayer.entity.lowerAnimation.animationFrameHeight = 122;//imageHeight;
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
    newEnemy.timeSinceLastAttack = 0;

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
    newEnemy.entity.lowerAnimation.animationFrameSpeed = 0.10f;
    newEnemy.entity.lowerAnimation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newEnemy.entity.lowerAnimation.animationFrameHeight = 122;//imageHeight;
    newEnemy.entity.lowerAnimation.currentAnimationFrame = 0;
    newEnemy.entity.lowerAnimation.currentAnimationState = IDLE;
    newEnemy.entity.lowerAnimation.isFacingRight = 1;
    newEnemy.entity.lowerAnimation.timeSinceLastFrame = 0;
    newEnemy.entity.lowerAnimation.currentAnimationFrameRect.x = 0.0f;
    newEnemy.entity.lowerAnimation.currentAnimationFrameRect.y = 0.0f;
    newEnemy.entity.lowerAnimation.currentAnimationFrameRect.width = newEnemy.entity.lowerAnimation.animationFrameWidth;
    newEnemy.entity.lowerAnimation.currentAnimationFrameRect.height = newEnemy.entity.lowerAnimation.animationFrameHeight;
    newEnemy.entity.upperAnimation.animationFrameSpeed = 0.10f;
    newEnemy.entity.upperAnimation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
    newEnemy.entity.upperAnimation.animationFrameHeight = 122;//imageHeight;
    newEnemy.entity.upperAnimation.currentAnimationFrame = 0;
    newEnemy.entity.upperAnimation.currentAnimationState = IDLE;
    newEnemy.entity.upperAnimation.isFacingRight = 1;
    newEnemy.entity.upperAnimation.timeSinceLastFrame = 0;
    newEnemy.entity.upperAnimation.currentAnimationFrameRect.x = 0.0f;
    newEnemy.entity.upperAnimation.currentAnimationFrameRect.y = 0.0f;
    newEnemy.entity.upperAnimation.currentAnimationFrameRect.width = newEnemy.entity.upperAnimation.animationFrameWidth;
    newEnemy.entity.upperAnimation.currentAnimationFrameRect.height = newEnemy.entity.upperAnimation.animationFrameHeight;
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
            newEnemy.attackSpeed = 0.8f; // Ataques por segundo
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case GUNNER:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.attackSpeed = 0.8f; // Ataques por segundo
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case SNIPERSHOOTER:
            newEnemy.viewDistance = 1000;
            newEnemy.attackRange = 1000;
            newEnemy.attackSpeed = 0.8f; // Ataques por segundo
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case DRONE:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.attackSpeed = 0.8f; // Ataques por segundo
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case TURRET:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.attackSpeed = 0.8f; // Ataques por segundo
            newEnemy.entity.maxHP = maxHP;
            newEnemy.entity.currentHP = maxHP;
            break;
        case BOSS:
            newEnemy.viewDistance = 600;
            newEnemy.attackRange = 200;
            newEnemy.attackSpeed = 0.8f; // Ataques por segundo
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
            bullet_i->srcEntity = srcEntity;
            bullet_i->bulletType = bulletType;
            bullet_i->direction.x = entity->lowerAnimation.isFacingRight;
            bullet_i->direction.y = (entity->upPressed ? -1 : entity->downPressed ? 1 : 0);
            bullet_i->angle = (bullet_i->direction.y/bullet_i->direction.x == -1 ? -45 : bullet_i->direction.y/bullet_i->direction.x == 1 ? 45 : 0);
            
            float offset = 0; //0.52f;
            //if (bullet_i->direction.x == -1)
            //    offset = 0.65f;
            bullet_i->position.x = entity->position.x + bullet_i->direction.x * offset * entity->width;
            bullet_i->position.y = entity->position.y; // Ajustar TODO
            
            bullet_i->width = 20; // Tem que tunar
            bullet_i->height = 7; // Tem que tunar
            bullet_i->power = 40; // Tem que tunar
            bullet_i->lifeTime = 0;
            bullet_i->isActive = true;

            bullet_i->animation.animationFrameSpeed = 0.08f;
            bullet_i->animation.animationFrameWidth = 122;//(float)imageWidth/imageFramesCount;
            bullet_i->animation.animationFrameHeight = 122;//imageHeight;
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

Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom) {
    Camera2D newCam = {0};
    newCam.target = target;
    newCam.offset = offset;
    newCam.rotation = rotation;
    newCam.zoom = zoom;

    return newCam;
}

void UpdatePlayer(Player *player, Enemy *enemy, Bullet *bulletPool, float delta, Props *props, float minX) {
    enum CHARACTER_STATE currentState = player->entity.lowerAnimation.currentAnimationState;
    player->entity.lowerAnimation.timeSinceLastFrame += delta;
    player->entity.upperAnimation.timeSinceLastFrame += delta;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Handler de input do player                                     ///////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (player->entity.lowerAnimation.currentAnimationState != DYING) {//} && player->entity.animation.currentAnimationState != HURT && player->entity.animation.currentAnimationState != ATTACKING) {
        
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
            player->entity.velocity.y = -player->entity.jumpSpeed;
            player->entity.isGrounded = false;
        }

        if (IsKeyPressed(KEY_H))
            player->entity.lowerAnimation.currentAnimationState = HURT;

        if (IsKeyPressed(KEY_P))
            player->entity.currentHP = 0;

        if (IsKeyPressed(KEY_R)) {
            if (player->entity.upperAnimation.currentAnimationState != ATTACKING || (player->entity.upperAnimation.currentAnimationState == ATTACKING && player->entity.upperAnimation.currentAnimationFrame > 1)) {
                CreateBullet(&(player->entity), bulletPool, MAGNUM, PLAYER);
                player->entity.upperAnimation.currentAnimationState = ATTACKING;
                player->entity.upperAnimation.currentAnimationFrame = 0;
                player->entity.upperAnimation.timeSinceLastFrame = 0;
            }
            //player->entity.animation.currentAnimationState = ATTACKING;
            //player->entity.animation.currentAnimationFrame = 0;
            //player->entity.animation.timeSinceLastFrame = 0;
            //if (player->entity.isGrounded)
            //    player->entity.velocity.x = 0;
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
    player->entity.drawableRect = (Rectangle) {player->entity.position.x, player->entity.position.y, (int)(player->entity.width * player->entity.characterWidthScale), (int)(player->entity.height * player->entity.characterHeightScale)};
    player->entity.collisionBox = (Rectangle) {player->entity.position.x  - player->entity.width/2 + (player->entity.lowerAnimation.isFacingRight == -1 ? 0.3f : 0.15f) * player->entity.width, player->entity.position.y - player->entity.height/2, player->entity.width * 0.5f, player->entity.height};
    player->entity.collisionHead = (Circle) {(Vector2){player->entity.position.x - player->entity.lowerAnimation.isFacingRight * 0.1f * player->entity.width, player->entity.position.y - 0.15f * player->entity.height}, player->entity.width * 0.2f};
}

void UpdateEnemy(Enemy *enemy, Player *player, Bullet *bulletPool, float delta, Props *props) {
    Entity *eEnt = &(enemy->entity);
    enum CHARACTER_STATE currentState = eEnt->lowerAnimation.currentAnimationState;
    eEnt->lowerAnimation.timeSinceLastFrame += delta;
    enemy->timeSinceLastBehaviorChange += delta;
    enemy->timeSinceLastAttack += delta;

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

    eEnt->drawableRect = (Rectangle) {eEnt->position.x, eEnt->position.y, eEnt->width * eEnt->characterWidthScale,eEnt->height * eEnt->characterHeightScale};
    eEnt->collisionBox = (Rectangle) {eEnt->position.x  - eEnt->width/2 + (eEnt->lowerAnimation.isFacingRight == -1 ? 0.3f : 0.15f) * eEnt->width, eEnt->position.y - eEnt->height/2, eEnt->width * 0.5f, eEnt->height};
    eEnt->collisionHead = (Circle) {(Vector2){eEnt->position.x - eEnt->lowerAnimation.isFacingRight * 0.1f * eEnt->width, eEnt->position.y - 0.15f * eEnt->height}, eEnt->width * 0.2f};
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
                if (currentEnemy->entity.lowerAnimation.currentAnimationState != DYING) {
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
        if (CheckCollisionRecs(player->entity.collisionBox, bullet->collisionBox) || CheckCollisionCircleRec(player->entity.collisionHead.center, player->entity.collisionHead.radius, bullet->collisionBox)) {
            bullet->isActive = false;
            // TODO Causa dano ao player
            // TODO Criar animação de sangue
        }
    }

    // Se não tiver colisão, checar tempo de vida e atualizar posição
    int vel = 2200;
    if (bullet->lifeTime >= bulletLifeTime) {
        bullet->isActive = false;
    } else {
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
            if (bullet->animation.currentAnimationFrame > 1) {  // 1 porque são 2 frames
                bullet->animation.currentAnimationFrame = 0;
            }
        }
        
        bullet->animation.currentAnimationFrameRect.x = (float)bullet->animation.currentAnimationFrame * bullet->animation.animationFrameWidth;
        bullet->animation.currentAnimationFrameRect.y = 0;
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

void UpdateBackground(Player *player, Background *backgroundPool, Texture2D srcAtlas, float delta, int *numBackground, float minX, float *maxX) {

    for (int i = 0; i < numBackgroundRendered; i++) {
        Background *bgP = backgroundPool + i;
        bgP->position.x = (bgP->originalX - minX*bgP->relativePosition);
        if (bgP->position.x+bgP->width < minX) {
            //"Deletar" bg e criar um novo
            UnloadRenderTexture(bgP->canvas);
            *bgP = CreateBackground(player, backgroundPool, srcAtlas, bgP->bgType, numBackground, 2.0f);
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
    DrawTexturePro(texture[enemy->class], enemy->entity.lowerAnimation.currentAnimationFrameRect, enemy->entity.drawableRect, (Vector2) {enemy->entity.width/2, enemy->entity.height/2}, 0, WHITE);
}

void DrawBullet(Bullet *bullet, Texture2D texture, bool drawCollisionBox) {
    Vector2 origin = (Vector2) {122/2, 122/2};
    DrawTexturePro(texture, bullet->animation.currentAnimationFrameRect, bullet->drawableRect, origin, bullet->angle, WHITE);
    // Draw das caixas de colisão
    if (drawCollisionBox) {
        DrawRectangle(bullet->collisionBox.x, bullet->collisionBox.y, bullet->collisionBox.width, bullet->collisionBox.height, BLUE);
    }
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

RenderTexture2D PaintCanvas(Texture2D atlas, enum BACKGROUND_TYPES bgLayer) {
    RenderTexture2D canvas;
    int frameWidth;
    int frameHeight;
    int offset;
    int width;
    int height;
    int buildingRow;
    int buildingCol;
    switch(bgLayer) {
        case BACKGROUND:
            frameWidth = 120;
            frameHeight = 450;
            offset = 5;
            width = 5*(frameWidth+offset);
            height = screenHeight;
            buildingRow = 0;
            buildingCol = 0;
            canvas = LoadRenderTexture(width, height);
            BeginTextureMode(canvas);
            //ClearBackground(RAYWHITE);
            for (int i = 0; i < 5; i++) { // totalProps max
                buildingRow = 0;
                buildingCol = GetRandomValue(0, 3); // 4 Tipos
                if (GetRandomValue(1,5) >= 2) 
                    DrawTexturePro(atlas, (Rectangle){buildingCol*frameWidth, buildingRow*frameHeight, frameWidth, frameHeight},
                        (Rectangle){offset + (i*(offset+frameWidth)), (height - 2*frameHeight - GetRandomValue(20, 100)), frameWidth*1.2f, 2*GetRandomValue(frameHeight-10, frameHeight+10)}, (Vector2) {0, 0}, 0, WHITE);
            }
            EndTextureMode();
        break;
        case MIDDLEGROUND:
            frameWidth = 178;
            frameHeight = 36;
            offset = 30;
            width = (2*frameWidth+offset) * 2;
            height = screenHeight;
            buildingRow = 0;
            buildingCol = 0;
            canvas = LoadRenderTexture(width, height);
            BeginTextureMode(canvas);
            for (int j = 0; j < 2; j++) { //2 Unidades por chunk
                int numFloor = GetRandomValue(10,15);
                int heightScale = GetRandomValue(-3,3);
                int widthScale = GetRandomValue(-10,-5);
                int doubled = GetRandomValue(1,5);
                for (int i = 0; i < numFloor; i++) {
                    buildingRow = GetRandomValue(0,5); // 6 tipos
                    DrawTexturePro(atlas, (Rectangle){0, buildingRow*frameHeight, frameWidth, frameHeight},
                        (Rectangle){offset + (j*(offset+2*frameWidth+widthScale)), (height - 150) - i*(frameHeight+heightScale), frameWidth + widthScale, frameHeight + heightScale},
                        (Vector2) {0, 0}, 0, WHITE);
                    if (doubled < 2) {
                        buildingRow = GetRandomValue(0,5); // 6 tipos
                        DrawTexturePro(atlas, (Rectangle){0, buildingRow*frameHeight, -frameWidth, frameHeight},
                            (Rectangle){offset + frameWidth +widthScale+ (j*(offset+2*frameWidth+widthScale)), (height - 150) - i*(frameHeight+heightScale), frameWidth + widthScale, frameHeight + heightScale},
                            (Vector2) {0, 0}, 0, WHITE);
                    }
                }
            }
            EndTextureMode();
        break;
        case FOREGROUND:
            frameWidth = 200;
            frameHeight = 200;
            int overhang = 0;
            int buildingRow = 0;
            int numFloor = GetRandomValue(2,4);
            int tilesWidth = GetRandomValue(3,5);
            bool hasDoor = false;
            int isFlipped = 1;
            int style = GetRandomValue(0,1); // 2 estilos
            width = screenWidth;
            height = screenHeight;
            canvas = LoadRenderTexture(width, height);
            BeginTextureMode(canvas);
            for (int i = 0; i < numFloor; i++) {
                for (int j = 0; j < tilesWidth; j++) {
                    overhang = 0;
                    if (j == 0) {
                        buildingRow = 0;
                        isFlipped = 1;
                    } else if (j == tilesWidth - 1) {
                        buildingRow = 0;
                        isFlipped = -1;
                    } else {
                        if (i == 0) {
                            if (j == tilesWidth - 2 && !hasDoor) {
                                buildingRow = 1;
                            } else {
                                if (!hasDoor) {
                                    buildingRow = (GetRandomValue(0,3) == 0 ? 1 : 2);
                                    if (buildingRow == 1) {
                                        hasDoor = true;
                                    }
                                }
                            }
                        } else {
                            buildingRow = (GetRandomValue(0,3) == 0 ? 3 : 2);
                        }
                    }
                    if (i == numFloor - 1) {
                        overhang = 7;
                        buildingRow = 4;
                    }
                    DrawTexturePro(atlas, (Rectangle){style*frameWidth, buildingRow*frameHeight, isFlipped*frameWidth, frameHeight},
                        (Rectangle){100+j*frameHeight-overhang, (height - 150) - (i+1)*(frameHeight), frameWidth+2*overhang, frameHeight}, // deslocado 150 pixels acima do fundo da tela
                        (Vector2) {0, 0}, 0, WHITE);
                }
            }
            EndTextureMode();
        break;
    }

    return canvas;
}