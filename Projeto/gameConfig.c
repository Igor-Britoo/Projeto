#include <stdlib.h>     
#include <string.h>     
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "raylib.h"

#define WP_GUN_ID 1;
#define WP_MACHINEGUN_ID 2;
#define WP_SWORD_ID 3;

// Enums
enum CHARACTER_STATE{IDLE, WALKING, HURT, JUMPING, FALLING, ATTACKING, DYING};
enum ENTITY_TYPES{PLAYER, ENEMY};
enum ENEMY_BEHAVIOR{NONE, ATTACK, MOVE};
enum BACKGROUND_TYPES{BACKGROUND, MIDDLEGROUND, FOREGROUND};
enum BULLET_TYPE{MAGNUM, SNIPER, LASER};
enum ENEMY_CLASSES{SWORDSMAN, GUNNER, SNIPERSHOOTER, DRONE, TURRET, BOSS};

// Consts
const float GRAVITY = 400; // 400 px / f²
const float bulletLifeTime = 4; // 4 s
const static int numBackgroundRendered = 3;
const static int maxNumBullets = 100;
const static int maxNumEnemies = 20;
const static int maxNumProps = 20;
const static int numEnemyClass = 6;
const int screenWidth = 1600;
const int screenHeight = 900;
const char gameName[30] = "Project N30-N";
bool isFullscreen = 0;

// Structs
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
    int maxHP;
    int currentHP;
    Vector2 position;
    Vector2 velocity;
    Vector2 momentum;
    float maxXSpeed;
    float sprintSpeed;
    float jumpSpeed;
    bool isGrounded;
    Vector2 eyesOffset;
    float characterWidthScale;
    float characterHeightScale;
    Animation animation;

} Entity;

typedef struct player
{
    Entity entity;
    int currentWeaponID;
    int currentAmmo;

}  Player;

typedef struct enemy
{
    Entity entity;
    Vector2 target;
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

} Enemy;

typedef struct props {
    Rectangle rect;
    int canBeStepped;
    int blockPlayer;
    int isInvisible;
    bool isActive;

} Props;


typedef struct background {
    enum BACKGROUND_TYPES bgType;
    Vector2 position;
    int originalX;
    float scale;
    int width;
    float height;
    float relativePosition;

} Background;


typedef struct particle {
    Texture2D animTex;
    Vector2 position;
    int numFrames;
    int currentFrame;
    int frameRow; // Só será usada se for um atlas para as partículas. Em caso de arquivo individual, pode deletar a variável
    int loopAllowed;
    float animationFrameSpeed;
    Vector2 scaleRange;
    float angularVelocity;
    float lifeTime;

} Particle;

typedef struct bullet {
    Vector2 position;
    Animation animation;
    enum ENTITY_TYPES srcEntity;
    enum BULLET_TYPE bulletType;
    int direction;
    int width;
    int height;
    float power;
    float lifeTime;
    bool isActive;

} Bullet;

// Headers
void CreateBullet(Entity *entity, Bullet *bulletsPool, enum BULLET_TYPE bulletType, enum ENTITY_TYPES srcEntity);
Background CreateBackground(Player *player, Background *backgroundPool, enum BACKGROUND_TYPES bgType, int *numBackground, float scale);
Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom);
Player CreatePlayer(int maxHP, Vector2 position, float imageWidth, float imageHeight);
Enemy CreateEnemy(enum ENEMY_CLASSES class, int maxHP, Vector2 position, float imageWidth, float imageHeight);

void UpdateBackground(Player *player, Background *backgroundPool, float delta, int *numBackground, float minX, float *maxX);
void UpdateClampedCameraPlayer(Camera2D *camera, Player *player, Props *props, float delta, int width, int height, float *minX, float maxX);
void UpdatePlayer(Player *player, Enemy *enemy, Bullet *bulletPool, float delta, Props *props, float minX);
void UpdateBullets(Bullet *bullet, Enemy *enemy, Player *player, Props *props, float delta);
void UpdateEnemy(Enemy *enemy, Player *player, float delta, Props *props);
void UpdateProps(Player *player, Props *props, float delta, float minX);

void TurnAround(Entity *ent) {
    ent->animation.isFacingRight *= -1;
}

void SetTarget(Vector2 target, Enemy *enemy) {
    enemy->target = target;
    enemy->entity.velocity.x = 0;
}

void LookAtTarget(Enemy *enemy) {
    if (enemy->entity.position.x >= enemy->target.x) { //
        if (enemy->entity.animation.isFacingRight == 1) TurnAround(&(enemy->entity));
    } else {
        if (enemy->entity.animation.isFacingRight == -1) TurnAround(&(enemy->entity));
    }
}

void MoveToTarget(Enemy *enemy) {
    LookAtTarget(enemy);
    enemy->entity.momentum.x += 200;
}

void AttackTarget(Enemy *enemy) {
    // Atualizar estado
    LookAtTarget(enemy);
    enemy->entity.animation.currentAnimationState = ATTACKING;
    enemy->entity.momentum.x = 0;
    enemy->entity.velocity.x = 0;
}

void SteeringBehavior(Enemy *enemy, Player *player, float delta) {
    Entity *eEnt = &(enemy->entity); // Pointer direto para a Entity do inimigo
    Entity *pEnt = &(player->entity); // Pointer direto para a Entity do player
    
    float eyesX = eEnt->position.x + eEnt->eyesOffset.x;
    float eyesY = eEnt->position.y + eEnt->eyesOffset.y;

    Rectangle detectionBox;

    if (eEnt->animation.isFacingRight == 1) {
        detectionBox = (Rectangle){eyesX, eyesY, enemy->viewDistance, 5};
    } else {
        detectionBox = (Rectangle){eyesX-enemy->viewDistance, eyesY, enemy->viewDistance, 5};
    }

    Rectangle playerBox = (Rectangle) {pEnt->position.x, pEnt->position.y, pEnt->animation.animationFrameWidth * pEnt->characterWidthScale, pEnt->animation.animationFrameHeight * pEnt->characterHeightScale};
    
    if ((enemy->entity.animation.currentAnimationState != DYING) && (enemy->entity.animation.currentAnimationState != HURT)) {
        if (CheckCollisionRecs(detectionBox, playerBox)) { // Se houver detecção, setar target
            enemy->noDetectionTime = 0;
            if (enemy->behavior == NONE) { // Se não tiver target
                enemy->behavior = MOVE;
                SetTarget(pEnt->position, enemy);
            } else { // Se já tiver target
                SetTarget(pEnt->position, enemy); // Atualiza target
                // MOVER OU ATACAR
                float attackX;
                if (eEnt->animation.isFacingRight == 1) { // Direita
                    attackX = eEnt->position.x + eEnt->animation.animationFrameWidth * eEnt->characterWidthScale + enemy->attackRange;
                    if (attackX < enemy->target.x) { // Ainda não chegou
                        enemy->behavior = MOVE;
                        MoveToTarget(enemy);
                    } else {
                        enemy->behavior = ATTACK;
                        AttackTarget(enemy);
                    }
                } else {
                    attackX = eEnt->position.x - enemy->attackRange;
                    if (attackX > enemy->target.x + pEnt->animation.animationFrameWidth * pEnt->characterWidthScale) { // Ainda não chegou
                        enemy->behavior = MOVE;
                        MoveToTarget(enemy);
                    } else {
                        enemy->behavior = ATTACK;
                        AttackTarget(enemy);
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
            } else if (animation->currentAnimationState == HURT) {
               if (animation->currentAnimationFrame < 2) {
                entity->position.x -= animation->isFacingRight*600*delta;
                }
            }
        }
        if (animation->currentAnimationState == DYING)
            if (!entity->isGrounded) entity->position.x -= animation->isFacingRight*1000*delta;

            /////////////// FIM DO CÓDIGO TEMPORÁRIO
    }
}

void PhysicsAndGraphicsHandlers (Entity *entity, float delta, enum CHARACTER_STATE currentState) {
    Animation *animation = &(entity->animation);

    // Atualização da física
    entity->velocity.x += entity->animation.isFacingRight*entity->momentum.x * delta;
    if (entity->velocity.x > entity->maxXSpeed) {
        entity->velocity.x = entity->maxXSpeed;
    } else if (entity->velocity.x < -entity->maxXSpeed) {
        entity->velocity.x = -entity->maxXSpeed;
    }
    if (entity->velocity.x > 0) {
        entity->animation.isFacingRight = 1;
    } else if (entity->velocity.x < 0) {
        entity->animation.isFacingRight = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Atualização de estado para controle da animação da entity        /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Estados relacionados a movimentação
    if ((entity->animation.currentAnimationState != DYING) && (entity->animation.currentAnimationState != HURT)) {
        entity->position.x += entity->velocity.x * delta;
        if (entity->animation.currentAnimationState != ATTACKING) {
            if (entity->isGrounded) {
                if (entity->velocity.x != 0) {
                    entity->animation.currentAnimationState = WALKING;
                } else {
                    entity->animation.currentAnimationState = IDLE;
                }
            } else if (!entity->isGrounded) {
                if (entity->velocity.y < 0) {
                    entity->animation.currentAnimationState = JUMPING;
                } else if(entity->velocity.y > 0) {
                    entity->animation.currentAnimationState = FALLING;
                }
            }
        } else {

        }
    } else {
        
    }
    
    // Atualização de estado quando o inimigo morre
    if (entity->currentHP <= 0 && !(entity->animation.currentAnimationState == DYING)) {
        entity->animation.currentAnimationState = DYING;
    }

    int animRow = 0;
    if (currentState != animation->currentAnimationState) {
        animation->timeSinceLastFrame = 0.0f;
        animation->currentAnimationFrame = 0;
    }
    switch (animation->currentAnimationState)
    {
    case IDLE:
        animRow = 0;
        PlayEntityAnimation(entity, delta, animation, 6, true, false, -1, false, IDLE);
        break;
    case WALKING:
        animRow = 1;
        PlayEntityAnimation(entity, delta, animation, 8, true, false, -1, false, WALKING);
        break;
    case HURT:
        animRow = 2;
        PlayEntityAnimation(entity, delta, animation, 6, false, false, -1, true, IDLE);
        break;
    case JUMPING:
        animRow = 3;
        PlayEntityAnimation(entity, delta, animation, 5, false, false, -1, false, JUMPING);
        break;
    case FALLING:
        animRow = 3;
        PlayEntityAnimation(entity, delta, animation, 5, false, true, 5, false, FALLING);
        break;
    case DYING:
        animRow = 4;
        PlayEntityAnimation(entity, delta, animation, 7, false, false, -1, false, DYING);
        break;
    case ATTACKING:
        animRow = 6;
        PlayEntityAnimation(entity, delta, animation, 6, false, false, -1, true, IDLE);
    default:
        break;
    }

    entity->animation.currentAnimationFrameRect.x = (float)entity->animation.currentAnimationFrame * entity->animation.animationFrameWidth;
    entity->animation.currentAnimationFrameRect.y = animRow * entity->animation.animationFrameHeight;
    entity->animation.currentAnimationFrameRect.width = entity->animation.isFacingRight * entity->animation.animationFrameWidth;
}

void EntityCollisionHandler(Entity *entity, Props *props, float delta) {
    // Colisão com props                                            ///////////////////////////////////////////////////////////////////////
    int hitObstacle = 0;
    int hasFloorBelow = 0;
    Rectangle prect = {entity->position.x, entity->position.y, entity->animation.animationFrameWidth, entity->animation.animationFrameHeight};
    Rectangle prectGrav = {entity->position.x, entity->position.y+1, entity->animation.animationFrameWidth, entity->animation.animationFrameHeight};
    for (int i = 0; i < maxNumProps; i++)
    {
        Props *eprop = props + i;
        Vector2 *p = &(entity->position);
        if (eprop->canBeStepped) {
            if (CheckCollisionRecs(eprop->rect, prect)) {
                hitObstacle = 1;
                entity->velocity.y = 0.0f;
                p->y = eprop->rect.y - entity->animation.animationFrameHeight;
            }
            if (CheckCollisionRecs(eprop->rect, prectGrav)) {
                hasFloorBelow = 1;
            }
        }
    }
    
    // Verifica se tem props abaixo para controle da gravidade      ///////////////////////////////////////////////////////////////////////
    if (!hitObstacle) 
    {
        entity->position.y += entity->velocity.y * delta;
        entity->velocity.y += GRAVITY * delta;
        entity->isGrounded = false;
    } 
    else entity->isGrounded = true;

    if (hasFloorBelow) {
        entity->velocity.y = 0;
        entity->isGrounded = true;
    }
}

void HurtEntity(Entity *dstEntity, Bullet srcBullet, float damage) {
    dstEntity->animation.currentAnimationState = HURT;
    dstEntity->animation.timeSinceLastFrame = 0;
    dstEntity->animation.currentAnimationFrame = 0;
    dstEntity->animation.isFacingRight = -srcBullet.direction;
    dstEntity->currentHP -= damage;
}