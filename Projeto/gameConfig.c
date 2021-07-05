#include <stdlib.h>     
#include <string.h>     
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "raylib.h"
#include "frameMapping.c"


// Enums
enum GAME_STATE{MENU, ACTIVE, PAUSE, GAMEOVER, SCORE};
enum CHARACTER_STATE{IDLE, WALKING, JUMPING, FALLING, ATTACKING, THROWING, DYING, DEAD};
enum CHARACTER_AIMING{FORWARD, UP45, DOWN45};
enum ENTITY_TYPES{PLAYER, ENEMY};
enum ENEMY_BEHAVIOR{NONE, ATTACK, MOVE};
enum BACKGROUND_TYPES{BACKGROUND, MIDDLEGROUND, FOREGROUND};
enum BACKGROUND_STYLE{SKYSCRAPER};
enum MIDDLEGROUND_STYLE{COMPLEX, RED_BUILDING};
enum FOREGROUND_STYLE{RESIDENTIAL};
enum BULLET_TYPE{MAGNUM, SNIPER, LASER};
enum ENEMY_CLASSES{SWORDSMAN, ASSASSIN, GUNNER, SNIPERSHOOTER, DRONE, TURRET, BOSS};
enum OBJECTS_TYPES {METAL_CRATE, AMMO_CRATE, HP_CRATE, CARD_CRATE1, CARD_CRATE2, CARD_CRATE3, TRASH_BIN, EXPLOSIVE_BARREL, METAL_BARREL, GARBAGE_BAG1, GARBAGE_BAG2, TRASH_CONTAINER, ROAD_BLOCK};
enum PARTICLE_TYPES {EXPLOSION, SMOKE, BLOOD_SPILL, MAGNUM_SHOOT};

// Consts
const float GRAVITY = 400; // 400 px / f²
const float bulletLifeTime = 4; // 4 s
const float grenadeExplosionTime = 4; // 4 s
const static int numBackgroundRendered = 7;
const static int maxNumBullets = 100;
const static int maxNumParticles = 500;
const static int maxNumGrenade = 50;
const static int maxNumEnemies = 60;
const static int maxNumGrounds = 300;
const static int maxNumEnvProps = 50;
const static int numEnemyClasses = 7;
const int screenWidth = 1920;
const int screenHeight = 1080;
const char gameName[30] = "Project N30-N";
bool isFullscreen = true;

// Structs
typedef struct circle {
    Vector2 center;
    float radius;
} Circle;

typedef struct animation {
    enum CHARACTER_STATE currentAnimationState;
    Rectangle currentAnimationFrameRect;
    int isFacingRight;
    float animationFrameSpeed;
    int animationFrameWidth;
    int animationFrameHeight;
    int currentAnimationFrame;
    float timeSinceLastFrame;

} Animation;

typedef struct entity {
    enum ENTITY_TYPES type;
    int maxHP;
    int currentHP;
    float characterWidthScale;
    float characterHeightScale;
    Rectangle drawableRect;
    Rectangle collisionBox;
    Circle collisionHead;
    Vector2 position;
    Vector2 velocity;
    Vector2 momentum;
    float maxXSpeed;
    float sprintSpeed;
    float jumpSpeed;
    bool isGrounded;
    Vector2 eyesOffset;
    Animation upperAnimation;
    Animation lowerAnimation;
    int width;
    int height;
    int grenadeAmmo;

    //////// Handlers para controlar tiros nas diagonais
    bool upPressed;
    bool downPressed;

} Entity;

typedef struct player
{
    Entity entity;

}  Player;

typedef struct enemy
{
    Entity entity;
    Vector2 target;
    int id;
    enum ENEMY_CLASSES class;
    enum ENEMY_BEHAVIOR behavior;
    int viewDistance;
    int attackRange;
    float timeSinceLastBehaviorChange;
    float behaviorChangeInterval;
    float noDetectionTime;
    float loseTargetInterval;
    Vector2 spawnLocation;
    float maxDistanceToSpawn;
    bool isAlive;
    float attackSpeed;
    float timeSinceLastAttack;

} Enemy;


/**********************************************************************************************
 ** Struct para objetos interativos do cenário                                               **
 ** @rect: Rectangle com informação da posição, largura e altura da parte interativa         **
 ** @canBeStepped:  Bool se a parte superior interage com a flag "isGrounded" das entidades  **
 ** @followCamera:  Bool se a posição em @rect se mantém em relação à câmera                 **
 ** @blockPlayer: Bool se o @rect impede que as entidades passem horizontalmente por "dentro"**
 ** @isInvisible: Bool se o Ground deve ser desenhado na tela                                **
 ** @isActive: Bool de controle se o Ground existe                                           **
 **********************************************************************************************/
typedef struct ground {
    Rectangle rect;
    bool canBeStepped;
    bool followCamera;
    bool blockPlayer;
    bool isInvisible;
    bool isActive;

} Ground;


typedef struct background {
    enum BACKGROUND_TYPES bgType;
    RenderTexture2D canvas;
    Texture2D atlas;
    Vector2 position;
    int id;
    int originalX;
    float scale;
    int width;
    float height;
    float relativePosition;

} Background;


typedef struct particle {
    int id;
    Vector2 position;
    Vector2 velocity;
    Rectangle drawableRect;
    Rectangle frameRect;
    enum PARTICLE_TYPES type;
    float animationFrameSpeed;
    int numFrames;
    int frameRow; // Só será usada se for um atlas para as partículas. Em caso de arquivo individual, pode deletar a variável
    int currentAnimationFrame;
    float timeSinceLastFrame;
    float angle;
    float angularVelocity;
    Vector2 scaleRange;
    float scale;
    bool scaleUp;
    int loopAllowed;
    int isFacingRight;
    float timeToDestroy;
    float lifeTime;
    bool isActive;

} Particle;

typedef struct bullet {
    Vector2 position;
    int id;
    float angle;
    Animation animation;
    enum ENTITY_TYPES srcEntity;
    enum BULLET_TYPE bulletType;
    Vector2 direction;
    int width;
    int height;
    float power;
    float lifeTime;
    bool isActive;
    Rectangle drawableRect;
    Rectangle collisionBox;

} Bullet;

typedef struct grenade {
    Vector2 position;
    Vector2 velocity;
    int id;
    float angle;
    Animation animation;
    enum ENTITY_TYPES srcEntity;
    Vector2 direction;
    int width;
    int height;
    float power;
    float lifeTime;
    bool isActive;
    Rectangle drawableRect;
    Circle collisionCircle;

} Grenade;

typedef struct envProps {
    int id;
    int groundID;
    enum OBJECTS_TYPES type;
    Rectangle collisionRect;
    Rectangle frameRect;
    Rectangle drawableRect;
    bool isDestroyable;
    bool isCollectable;
    bool isActive;
} EnvProps;

// Headers
Texture2D CreateTexture(enum BACKGROUND_TYPES bgLayer, Image srcAtlas);
void CreateBullet(Entity *entity, Bullet *bulletsPool, enum BULLET_TYPE bulletType, enum ENTITY_TYPES srcEntity);
int CreateGround(Ground *groundPool, Vector2 position, int width, int height, bool canBeStepped, bool followCamera, bool blockPlayer, bool isInvisible);
void CreateEnvProp(EnvProps *envPropsPool, Ground *groundPool, enum OBJECTS_TYPES obType, Vector2 position, int width, int height);
Background CreateBackground(Player *player, Enemy *enemyPool, EnvProps *envPropsPool, Background *backgroundPool, Ground *groundPool, Texture2D srcAtlas, enum BACKGROUND_TYPES bgType, int *numBackground, int id);
Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom);
Player CreatePlayer(int maxHP, Vector2 position, int width, int height);
void CreateEnemy(Enemy *enemyPool, enum ENEMY_CLASSES class, Vector2 position, int width, int height);
void CreateGrenade(Entity *entity, Grenade *grenadePool, enum ENTITY_TYPES srcEntity);
void CreateParticle(Vector2 srcPosition, Vector2 velocity, Particle *particlePool, enum PARTICLE_TYPES type, float animTime, float angularVelocity, Vector2 scaleRange, bool isLoopable, int facingRight);

void DestroyEnvProp(EnvProps *envPropsPool, Ground *groundsPool, int envPropID);

void UpdateBackground(Player *player, Background *backgroundPool, int i, Texture2D srcAtlas, Enemy *enemyPool, EnvProps *envPropsPool, Ground *groundPool, float delta, int *numBackground, float minX, float *maxX);
void UpdateClampedCameraPlayer(Camera2D *camera, Player *player, float delta, int width, int height, float *minX, float maxX);
void UpdatePlayer(Player *player, Enemy *enemy, Bullet *bulletPool, Grenade *grenadePool, float delta, Ground *ground, EnvProps *envProps, float minX);
void UpdateBullets(Bullet *bullet, Enemy *enemy, Player *player, Ground *ground, EnvProps *envProp, float delta);
void UpdateEnemy(Enemy *enemy, Player *player, Bullet *bulletPool, float delta, Ground *ground, EnvProps *envProps);
void UpdateGrounds(Player *player, Ground *ground, float delta, float minX);
void UpdateEnvProps(Player *player, EnvProps *envPropsPool, Ground *groundsPool, float delta, float minX);
void UpdateGrenades(Grenade *grenade, Enemy *enemy, Player *player, Ground *ground, EnvProps *envProp, Particle *particlePool, float delta);
void UpdateParticles(Particle *curParticle, float delta, float minX);

void DrawEnemy(Enemy *enemy, Texture2D *texture, bool drawDetectionCollision, bool drawLife, bool drawCollisionBox);
void DrawBullet(Bullet *bullet, Texture2D texture, bool drawCollisionBox);
void DrawPlayer(Player *player, Texture2D texture, bool drawCollisionBox);
void DrawGrenade(Grenade *grenade, Texture2D texture, bool drawCollisionCircle);
void DrawParticle(Particle *particle, Texture2D texture);

RenderTexture2D PaintCanvas(Texture2D atlas, enum BACKGROUND_TYPES bgLayer, Ground *groundPool, int relativeXPos);

void GenerateBackground(RenderTexture2D canvas, Texture atlas, enum BACKGROUND_STYLE bgStyle);
void GenerateMidground(RenderTexture2D canvas, Texture atlas, enum MIDDLEGROUND_STYLE mgStyle);
void GenerateForeground(RenderTexture2D canvas, Ground *groundPool, Texture atlas, enum FOREGROUND_STYLE fgStyle, int relativeXPos);

void TurnAround(Entity *ent) {
    ent->lowerAnimation.isFacingRight *= -1;
}

void SetTarget(Vector2 target, Enemy *enemy) {
    enemy->target = target;
    enemy->entity.velocity.x = 0;
}

void LookAtTarget(Enemy *enemy) {
    if (enemy->entity.position.x >= enemy->target.x) { //
        if (enemy->entity.lowerAnimation.isFacingRight == 1) TurnAround(&(enemy->entity));
    } else {
        if (enemy->entity.lowerAnimation.isFacingRight == -1) TurnAround(&(enemy->entity));
    }
}

void MoveToTarget(Enemy *enemy) {
    LookAtTarget(enemy);
    enemy->entity.momentum.x += 200;
}

void AttackTarget(Enemy *enemy, Bullet *bulletPool) {
    // Atualizar estado
    LookAtTarget(enemy);
    CreateBullet(&(enemy->entity), bulletPool, MAGNUM, ENEMY);
    enemy->entity.upperAnimation.currentAnimationState = ATTACKING;
    enemy->entity.momentum.x = 0;
    enemy->entity.velocity.x = 0;
}

void SteeringBehavior(Enemy *enemy, Player *player, Bullet *bulletPool, float delta) {
    Entity *eEnt = &(enemy->entity); // Pointer direto para a Entity do inimigo
    Entity *pEnt = &(player->entity); // Pointer direto para a Entity do player
    
    float eyesX = eEnt->position.x + eEnt->eyesOffset.x;
    float eyesY = eEnt->position.y + eEnt->eyesOffset.y;

    Rectangle detectionBox;

    if (eEnt->lowerAnimation.isFacingRight == 1) {
        detectionBox = (Rectangle){eyesX, eyesY, enemy->viewDistance, 5};
    } else {
        detectionBox = (Rectangle){eyesX-enemy->viewDistance, eyesY, enemy->viewDistance, 5};
    }

    Rectangle playerBox = (Rectangle) {pEnt->position.x, pEnt->position.y, pEnt->lowerAnimation.animationFrameWidth * pEnt->characterWidthScale, pEnt->lowerAnimation.animationFrameHeight * pEnt->characterHeightScale};
    
    if ((enemy->entity.lowerAnimation.currentAnimationState != DYING)) {// && (enemy->entity.lowerAnimation.currentAnimationState != HURT)) {
        if (CheckCollisionRecs(detectionBox, playerBox)) { // Se houver detecção, setar target
            enemy->noDetectionTime = 0;
            if (enemy->behavior == NONE) { // Se não tiver target
                enemy->behavior = MOVE;
                SetTarget(pEnt->position, enemy);
            } else { // Se já tiver target
                SetTarget(pEnt->position, enemy); // Atualiza target
                // MOVER OU ATACAR
                float attackX;
                if (eEnt->lowerAnimation.isFacingRight == 1) { // Direita
                    attackX = eEnt->position.x + eEnt->lowerAnimation.animationFrameWidth * eEnt->characterWidthScale + enemy->attackRange;
                    if (attackX < enemy->target.x) { // Ainda não chegou
                        enemy->behavior = MOVE;
                        MoveToTarget(enemy);
                    } else {
                        if (enemy->timeSinceLastAttack >= 1/enemy->attackSpeed && enemy->entity.lowerAnimation.currentAnimationFrame == 0) {
                            enemy->timeSinceLastAttack = 0;
                            enemy->behavior = ATTACK;
                            AttackTarget(enemy, bulletPool);
                        } else {
                            //enemy->behavior = NONE;
                        }
                    }
                } else {
                    attackX = eEnt->position.x - enemy->attackRange;
                    if (attackX > enemy->target.x + pEnt->lowerAnimation.animationFrameWidth * pEnt->characterWidthScale) { // Ainda não chegou
                        enemy->behavior = MOVE;
                        MoveToTarget(enemy);
                    } else {
                        if (enemy->timeSinceLastAttack >= 1/enemy->attackSpeed && enemy->entity.lowerAnimation.currentAnimationFrame == 0) {
                            enemy->timeSinceLastAttack = 0;
                            enemy->behavior = ATTACK;
                            AttackTarget(enemy, bulletPool);
                        } else {
                            //enemy->behavior = NONE;
                        }
                    }
                }
            }
        } else { // Se não tem colisão
            if (enemy->behavior == NONE) { // Se não tiver target
                if (enemy->timeSinceLastBehaviorChange >= enemy->behaviorChangeInterval) { // Controle de tempo para alterar comportamento
                    enemy->timeSinceLastBehaviorChange = 0;
                    int random = GetRandomValue(1, 5); // 5 possibilidades. Precisar tunar para que o inimigo não se afaste tanto do spawn próprio
                    if (random <= 1) { // 10%
                        // Mudar direção
                        TurnAround(eEnt);
                        eEnt->momentum.x = 0; // Parar
                        eEnt->velocity.x = 0; // Parar
                    } else { // 80%
                        random = GetRandomValue(1,5);
                        // Alguma outra opção?
                        if (random <= 2) {// 20%
                            eEnt->momentum.x = 0; // Parar
                            eEnt->velocity.x = 0; // Parar
                        } else {
                            eEnt->momentum.x = 200; // Tem que tunar
                        }
                    }
                }
            } else { // Se tem target
                enemy->noDetectionTime += delta;
                SetTarget(pEnt->position, enemy); // Atualiza target
                if (enemy->noDetectionTime >= enemy->loseTargetInterval) { // Perder target
                    enemy->behavior = NONE;
                    SetTarget((Vector2){-1, -1}, enemy);
                } else { // Se ainda não deu tempo de perder target, ir atrás do player
                    enemy->behavior = MOVE;
                    MoveToTarget(enemy);
                }
            }
        }
    }
}

void PlayEntityAnimation(Entity *entity, float delta, Animation *animation, float numOfFrames, bool isLoopable, bool isFixed, int fixedFrame, bool transitToAnotherState, enum CHARACTER_STATE nextState) {
// ENTITY E DELTA ESTÃO SENDO USADOS TEMPORARIAMENTE PARA USO DAS ANIMACOES DYING E HURT. DEVERÃO SAIR EM ALGUM MOMENTO
    if (animation->timeSinceLastFrame >= animation->animationFrameSpeed) {
        animation->timeSinceLastFrame = 0.0f;
        animation->currentAnimationFrame++;
        if (animation->currentAnimationFrame > numOfFrames-1) { 
            if (isLoopable) {
                animation->currentAnimationFrame = 0; 
            } else {
                if (isFixed) {
                    animation->currentAnimationFrame = fixedFrame; 
                } else {
                    if (transitToAnotherState) {
                        animation->currentAnimationFrame = 0;
                        animation->currentAnimationState = nextState;
                    } else {
                        animation->currentAnimationFrame = numOfFrames-1; 
                    }
                }
            }
        } else {
            /////////////// A PARTIR DESTE PONTO, O CÓDIGO É TEMPORÁRIO
            if (animation->currentAnimationState == DYING) {
                entity->position.x -= animation->isFacingRight*1000*delta;
            }
        }
        if (animation->currentAnimationState == DYING)
            if (!entity->isGrounded) entity->position.x -= animation->isFacingRight*1000*delta;

            /////////////// FIM DO CÓDIGO TEMPORÁRIO
    }
}

void PhysicsAndGraphicsHandlers (Entity *entity, float delta, enum CHARACTER_STATE currentLowerState, enum CHARACTER_STATE currentUpperState, enum ENTITY_TYPES entityType, enum ENEMY_CLASSES enemyClass) {
    Animation *upperAnimation = &(entity->upperAnimation);
    Animation *lowerAnimation = &(entity->lowerAnimation);

    // Atualização da física
    entity->velocity.x += entity->lowerAnimation.isFacingRight*entity->momentum.x * delta;
    if (entity->velocity.x > entity->maxXSpeed) {
        entity->velocity.x = entity->maxXSpeed;
    } else if (entity->velocity.x < -entity->maxXSpeed) {
        entity->velocity.x = -entity->maxXSpeed;
    }
    if (entity->velocity.x > 0) {
        entity->lowerAnimation.isFacingRight = 1;
    } else if (entity->velocity.x < 0) {
        entity->lowerAnimation.isFacingRight = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Atualização de estado para controle da animação da entity        /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Estados relacionados a movimentação
    if ((entity->lowerAnimation.currentAnimationState != DYING)){//} && (entity->animation.currentAnimationState != HURT)) {
        entity->position.x += entity->velocity.x * delta;
        //if (entity->animation.currentAnimationState != ATTACKING) {
            if (entity->isGrounded) {
                if (entity->velocity.x != 0) {
                    entity->lowerAnimation.currentAnimationState = WALKING;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING && entity->upperAnimation.currentAnimationState != THROWING)  
                        entity->upperAnimation.currentAnimationState = WALKING;
                } else {
                    entity->lowerAnimation.currentAnimationState = IDLE;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING && entity->upperAnimation.currentAnimationState != THROWING)  
                        entity->upperAnimation.currentAnimationState = IDLE;
                }
            } else if (!entity->isGrounded) {
                if (entity->velocity.y < 0) {
                    entity->lowerAnimation.currentAnimationState = JUMPING;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING && entity->upperAnimation.currentAnimationState != THROWING)  
                        entity->upperAnimation.currentAnimationState = JUMPING;
                } else if(entity->velocity.y > 0) {
                    entity->lowerAnimation.currentAnimationState = FALLING;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING && entity->upperAnimation.currentAnimationState != THROWING)  
                        entity->upperAnimation.currentAnimationState = FALLING;
                }
            }
        //} else {

        //}
    } else {
        
    }
    
    // Atualização de estado quando o inimigo morre
    if (entity->currentHP <= 0 && !(entity->lowerAnimation.currentAnimationState == DYING)) {
        entity->lowerAnimation.currentAnimationState = DYING;
        entity->upperAnimation.currentAnimationState = DYING;
    }

    int lAnimRow = 0;
    int uAnimRow = 0;

    if (currentLowerState != lowerAnimation->currentAnimationState) {
        lowerAnimation->timeSinceLastFrame = 0.0f;
        lowerAnimation->currentAnimationFrame = 0;
    }
    if (currentUpperState != upperAnimation->currentAnimationState) {
        upperAnimation->timeSinceLastFrame = 0.0f;
        upperAnimation->currentAnimationFrame = 0;
    }
    switch (lowerAnimation->currentAnimationState)
    {
    case IDLE:
        if (entityType == PLAYER) {
            lAnimRow = PLAYER_LEGS_IDLE_ROW;
            PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_IDLE_NUM_FRAMES, true, false, -1, false, IDLE);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                lAnimRow = ASSASSIN_LEGS_IDLE_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, ASSASSIN_LEGS_IDLE_NUM_FRAMES, true, false, -1, false, IDLE);
                break;
            default:
                lAnimRow = PLAYER_LEGS_IDLE_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_IDLE_NUM_FRAMES, true, false, -1, false, IDLE);
                break;
            }
        }
        break;
    case WALKING:
        if (entityType == PLAYER) {
            lAnimRow = PLAYER_LEGS_WALKING_ROW;
            PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_WALKING_NUM_FRAMES, true, false, -1, false, WALKING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                lAnimRow = ASSASSIN_LEGS_WALKING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, ASSASSIN_LEGS_WALKING_NUM_FRAMES, true, false, -1, false, WALKING);
                break;
            default:
                lAnimRow = PLAYER_LEGS_WALKING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_WALKING_NUM_FRAMES, true, false, -1, false, WALKING);
                break;
            }
        }
        break;
    case JUMPING:
        if (entityType == PLAYER) {
            lAnimRow = PLAYER_LEGS_JUMPING_ROW;
            PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_JUMPING_NUM_FRAMES, false, false, -1, false, JUMPING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN: // Não tem animação.. deixando como IDLE
                lAnimRow = ASSASSIN_LEGS_IDLE_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, ASSASSIN_LEGS_IDLE_NUM_FRAMES, false, false, -1, false, JUMPING);
                break;
            default:
                lAnimRow = PLAYER_LEGS_JUMPING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_JUMPING_NUM_FRAMES, false, false, -1, false, JUMPING);
                break;
            }
        }
        break;
    case FALLING:
        if (entityType == PLAYER) {
            lAnimRow = PLAYER_LEGS_JUMPING_ROW;
            PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_JUMPING_NUM_FRAMES, false, true, PLAYER_LEGS_JUMPING_NUM_FRAMES, false, FALLING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN: // Não tem animação.. deixando como IDLE
                lAnimRow = ASSASSIN_LEGS_IDLE_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, ASSASSIN_LEGS_IDLE_NUM_FRAMES, false, true, ASSASSIN_LEGS_IDLE_NUM_FRAMES, false, FALLING);
                break;
            default:
                lAnimRow = PLAYER_LEGS_JUMPING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_LEGS_JUMPING_NUM_FRAMES, false, true, PLAYER_LEGS_JUMPING_NUM_FRAMES, false, FALLING);
                break;
            }
        }
        break;
    case DYING:
        if (entityType == PLAYER) {
            lAnimRow = PLAYER_BODY_DYING_ROW;
            PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_BODY_DYING_NUM_FRAMES, false, false, -1, false, DYING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                lAnimRow = ASSASSIN_BODY_DYING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, ASSASSIN_BODY_DYING_NUM_FRAMES, false, false, -1, false, DYING);
                break;
            default:
                lAnimRow = PLAYER_BODY_DYING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, PLAYER_BODY_DYING_NUM_FRAMES, false, false, -1, false, DYING);
                break;
            }
        }
        break;
    default:
        break;
    }
    
    int numOfFrames = 0;
    switch (upperAnimation->currentAnimationState) {
    case IDLE:
        if (entityType == PLAYER) {
            uAnimRow = PLAYER_UPPER_IDLE_ROW;
            PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_IDLE_NUM_FRAMES, true, false, -1, false, IDLE);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                uAnimRow = ASSASSIN_UPPER_IDLE_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, ASSASSIN_UPPER_IDLE_NUM_FRAMES, true, false, -1, false, IDLE);
                break;
            default:
                uAnimRow = PLAYER_UPPER_IDLE_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_IDLE_NUM_FRAMES, true, false, -1, false, IDLE);
                break;
            }
        }
        break;
    case WALKING:
        if (entityType == PLAYER) {
            uAnimRow = PLAYER_UPPER_WALKING_ROW;
            PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_WALKING_NUM_FRAMES, true, false, -1, false, WALKING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                uAnimRow = ASSASSIN_UPPER_WALKING_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, ASSASSIN_UPPER_WALKING_NUM_FRAMES, true, false, -1, false, WALKING);
                break;
            default:
                uAnimRow = PLAYER_UPPER_WALKING_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_WALKING_NUM_FRAMES, true, false, -1, false, WALKING);
                break;
            }
        }
        break;
    case JUMPING:
        if (entityType == PLAYER) {
            uAnimRow = PLAYER_UPPER_JUMPING_ROW;
            PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_JUMPING_NUM_FRAMES, false, false, -1, false, JUMPING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                uAnimRow = ASSASSIN_UPPER_IDLE_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, ASSASSIN_UPPER_IDLE_NUM_FRAMES, false, false, -1, false, JUMPING);
                break;
            default:
                uAnimRow = PLAYER_UPPER_JUMPING_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_JUMPING_NUM_FRAMES, false, false, -1, false, JUMPING);
                break;
            }
        }
        break;
    case FALLING:
        if (entityType == PLAYER) {
            uAnimRow = PLAYER_UPPER_JUMPING_ROW;
            PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_JUMPING_NUM_FRAMES, false, true, PLAYER_UPPER_JUMPING_NUM_FRAMES, false, FALLING);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                uAnimRow = ASSASSIN_UPPER_IDLE_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, ASSASSIN_UPPER_IDLE_NUM_FRAMES, false, true, ASSASSIN_UPPER_IDLE_NUM_FRAMES, false, FALLING);
                break;
            default:
                uAnimRow = PLAYER_UPPER_JUMPING_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_JUMPING_NUM_FRAMES, false, true, PLAYER_UPPER_JUMPING_NUM_FRAMES, false, FALLING);
                break;
            }
        }
        break;
    case ATTACKING:
        if (entityType == PLAYER) {
            if (entity->upPressed) {
                uAnimRow = PLAYER_UPPER_SHOOTING_45U_ROW;
                numOfFrames = PLAYER_UPPER_SHOOTING_45U_NUM_FRAMES;
            } else if (entity->downPressed) {
                uAnimRow = PLAYER_UPPER_SHOOTING_45D_ROW;
                numOfFrames = PLAYER_UPPER_SHOOTING_45D_NUM_FRAMES;
            } else {
                uAnimRow = PLAYER_UPPER_SHOOTING_ROW;
                numOfFrames = PLAYER_UPPER_SHOOTING_NUM_FRAMES;
            }
            PlayEntityAnimation(entity, delta, upperAnimation, numOfFrames, false, false, -1, true, IDLE);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                uAnimRow = ASSASSIN_UPPER_ATTACKING_ROW;
                PlayEntityAnimation(entity, delta, lowerAnimation, ASSASSIN_UPPER_ATTACKING_NUM_FRAMES, false, false, -1, true, IDLE);
                break;
            default:
                if (entity->upPressed) {
                    uAnimRow = PLAYER_UPPER_SHOOTING_45U_ROW;
                    numOfFrames = PLAYER_UPPER_SHOOTING_45U_NUM_FRAMES;
                } else if (entity->downPressed) {
                    uAnimRow = PLAYER_UPPER_SHOOTING_45D_ROW;
                    numOfFrames = PLAYER_UPPER_SHOOTING_45D_NUM_FRAMES;
                } else {
                    uAnimRow = PLAYER_UPPER_SHOOTING_ROW;
                    numOfFrames = PLAYER_UPPER_SHOOTING_NUM_FRAMES;
                }
                PlayEntityAnimation(entity, delta, upperAnimation, numOfFrames, false, false, -1, true, IDLE);
                break;
            }
        }
        break;
    case DYING:
        uAnimRow = -1; // A animação é gerida apenas por "lowerAnimation"
        break;
    case THROWING:
        if (entityType == PLAYER) {
            uAnimRow = PLAYER_UPPER_THROWING_ROW;
            PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_THROWING_NUM_FRAMES, false, false, -1, true, IDLE);
        } else {
            switch (enemyClass)
            {
            case ASSASSIN:
                uAnimRow = ASSASSIN_UPPER_IDLE_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, ASSASSIN_UPPER_IDLE_NUM_FRAMES, false, false, -1, true, IDLE);
                break;
            default:
                uAnimRow = PLAYER_UPPER_THROWING_ROW;
                PlayEntityAnimation(entity, delta, upperAnimation, PLAYER_UPPER_THROWING_NUM_FRAMES, false, false, -1, true, IDLE);
                break;
            }
        }
        break;
    default:
        break;
    }

    entity->lowerAnimation.currentAnimationFrameRect.x = (float)entity->lowerAnimation.currentAnimationFrame * entity->lowerAnimation.animationFrameWidth;
    entity->lowerAnimation.currentAnimationFrameRect.y = lAnimRow * entity->lowerAnimation.animationFrameHeight;
    entity->lowerAnimation.currentAnimationFrameRect.width = entity->lowerAnimation.isFacingRight * entity->lowerAnimation.animationFrameWidth;

    entity->upperAnimation.currentAnimationFrameRect.x = (float)entity->upperAnimation.currentAnimationFrame * entity->upperAnimation.animationFrameWidth;
    entity->upperAnimation.currentAnimationFrameRect.y = uAnimRow * entity->upperAnimation.animationFrameHeight;
    entity->upperAnimation.currentAnimationFrameRect.width = entity->lowerAnimation.isFacingRight * entity->upperAnimation.animationFrameWidth;
}

void EntityCollisionHandler(Entity *entity, Ground *ground, EnvProps *envProp, float delta) {
    // Colisão com grounds                                            ///////////////////////////////////////////////////////////////////////
    int hitObstacle = 0;
    for (int i = 0; i < maxNumGrounds; i++)
    {
        Ground *curGround = ground + i;
        Vector2 *p = &(entity->position);
        if (curGround->isActive) {
            Rectangle *eCol = &(entity->collisionBox);
            Vector2 *eVel = &(entity->velocity);
            Rectangle futureBox = (Rectangle) {eCol->x + eVel->x * delta, eCol->y + eVel->y * delta, eCol->width, eCol->height};
            if (curGround->blockPlayer) {
                if (CheckCollisionRecs(curGround->rect, futureBox)) {
                    bool checkHorizontal = true;
                    if (futureBox.y + futureBox.height < curGround->rect.y + 5 && eVel->y >= 0) {
                        hitObstacle = 1;
                        eVel->y = 0;
                        p->y = curGround->rect.y - entity->height / 2+1;
                        checkHorizontal = false;
                    } else
                    if (futureBox.y > curGround->rect.y + curGround->rect.height - 5 && eVel->y < 0) {
                        checkHorizontal = false;
                        eVel->y = 0;
                    }
                    if (checkHorizontal) {
                        if (futureBox.x > curGround->rect.x) {
                            if (entity->lowerAnimation.isFacingRight < 0) {
                                p->x = curGround->rect.x + curGround->rect.width + 9;
                            } else {
                                p->x = curGround->rect.x + curGround->rect.width + 39;
                            }
                            eVel->x = 0;
                        } else
                        if (futureBox.x < curGround->rect.x) {
                            if (entity->lowerAnimation.isFacingRight < 0) {
                                p->x = curGround->rect.x - 39;
                            } else {
                                p->x = curGround->rect.x - 9;
                            }
                            eVel->x = 0;
                        }
                    }
                }
            } else if (curGround->canBeStepped) {
                if (CheckCollisionRecs(curGround->rect, futureBox)) {
                    if (entity->velocity.y >= 0) {
                        if (p->y <= curGround->rect.y - entity->height / 2+1) {
                            hitObstacle = 1;
                            entity->velocity.y = 0;
                            p->y = curGround->rect.y - entity->height / 2+1;
                        }
                    }
                }
            }
        }
    }
    
    // Verifica se tem props abaixo para controle da gravidade      ///////////////////////////////////////////////////////////////////////
    if (!hitObstacle) 
    {
        entity->position.y += entity->velocity.y*delta;
        entity->velocity.y += GRAVITY*delta;
        entity->isGrounded = false;
    } else {
        entity->isGrounded = true;
    }

    // Colisão com Props coletáveis                                  ///////////////////////////////////////////////////////////////////////
    if (entity->type == PLAYER) {
        for (int i = 0; i < maxNumEnvProps; i++)
        {
            EnvProps *curProp = envProp + i;
            if (curProp->isActive) {
                Rectangle *eCol = &(entity->collisionBox);
                Vector2 *eVel = &(entity->velocity);
                Rectangle futureBox = (Rectangle) {eCol->x + eVel->x * delta, eCol->y + eVel->y * delta, eCol->width, eCol->height};
                if (curProp->isCollectable) {
                    if (CheckCollisionRecs(curProp->collisionRect, futureBox)) {
                        // TODO dar o benefício
                        switch (curProp->type) {
                        case AMMO_CRATE:
                            entity->grenadeAmmo += 10;
                            entity->grenadeAmmo = fmin(entity->grenadeAmmo, 999);
                            break;
                        case HP_CRATE:
                            entity->currentHP += 50;
                            entity->currentHP = fmin(entity->currentHP, entity->maxHP);
                            break;
                        default:
                            break;
                        }
                        DestroyEnvProp(envProp, ground, i);
                        curProp->isActive = false;
                    }
                }
            }
        }
    }

}

void HurtEntity(Entity *dstEntity, Bullet srcBullet, float damage) {
    dstEntity->lowerAnimation.isFacingRight = -srcBullet.direction.x;
    dstEntity->currentHP -= damage;
}

void ExplosionAOE(EnvProps *envPropPool, Enemy *enemyPool, Ground *groundPool, int explosionRadius, float energy, Vector2 centerOfExplosion, enum ENTITY_TYPES srcEntity) {
    int maxCount = 0;
    maxCount = fmax(maxNumGrounds, maxNumGrenade);
    maxCount = fmax(maxCount, maxNumEnvProps);
    maxCount = fmax(maxCount, maxNumEnemies);
    maxCount = fmax(maxCount, maxNumBullets);
    for (int i = 0; i < maxCount; i++) {
        if (i < maxNumEnvProps) {
            EnvProps *curEnvProp = envPropPool + i;
            // Props
            if (curEnvProp->isActive && curEnvProp->isDestroyable) {
                if (CheckCollisionCircleRec(centerOfExplosion, explosionRadius, curEnvProp->collisionRect)) {
                    DestroyEnvProp(envPropPool, groundPool, i);
                }
            }
        }

        if (i < maxNumEnemies) {
            Enemy *curEnemy = enemyPool + i;
            // Enemy
            if (curEnemy->isAlive) {
                if (CheckCollisionCircleRec(centerOfExplosion, explosionRadius, curEnemy->entity.collisionBox)) {
                    curEnemy->entity.currentHP = 0;
                }
            }
        }

    }
}

void PopulateChunk(int chunkId, EnvProps *envPropsPool, Ground *groundPool, Enemy *enemyPool) {
    // chunkId -> posição do chunk para correto posicionamento
    int objAdditions = 0;
    int enemyAdditions = 0;
    // Popular com objetos
    int objProb = 60; // 5% de chance de ter um objeto
    int enemyProb = 70; // 15% de chance de ter um inimigo
    if (GetRandomValue(1,100) <= objProb) {
        objAdditions++;
        int obType = GetRandomValue(METAL_CRATE, GARBAGE_BAG2);
        //CreateEnvProp(envPropsPool, groundPool, obType, (Vector2) {chunkId*screenWidth + GetRandomValue(5, 10) + objAdditions*200, screenHeight-150-200}, 200, 200);
    }
    if (GetRandomValue(1,100) <= enemyProb) {
        enemyAdditions++;
        int enClass = GetRandomValue(SWORDSMAN, ASSASSIN);
        CreateEnemy(enemyPool, enClass, (Vector2) {chunkId*screenWidth + GetRandomValue(50, 100) + enemyAdditions*3, screenHeight-250}, 122, 122);
    }
}
