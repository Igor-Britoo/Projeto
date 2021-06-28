#include <stdlib.h>     
#include <string.h>     
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "raylib.h"

// Enums
enum CHARACTER_STATE{IDLE, WALKING, HURT, JUMPING, FALLING, ATTACKING, DYING, DEAD};
enum CHARACTER_AIMING{FORWARD, UP45, DOWN45};
enum ENTITY_TYPES{PLAYER, ENEMY};
enum ENEMY_BEHAVIOR{NONE, ATTACK, MOVE};
enum BACKGROUND_TYPES{BACKGROUND, MIDDLEGROUND, FOREGROUND};
enum MIDDLEGROUND_STYLE{COMPLEX, RED_BUILDING};
enum BULLET_TYPE{MAGNUM, SNIPER, LASER};
enum ENEMY_CLASSES{SWORDSMAN, GUNNER, SNIPERSHOOTER, DRONE, TURRET, BOSS};

// Consts
const float GRAVITY = 400; // 400 px / f²
const float bulletLifeTime = 4; // 4 s
const static int numBackgroundRendered = 5;
const static int maxNumBullets = 100;
const static int maxNumEnemies = 20;
const static int maxNumProps = 20;
const static int numEnemyClasses = 6;
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
    Animation upperAnimation;
    Animation lowerAnimation;
    int width;
    int height;
    Rectangle drawableRect;
    Rectangle collisionBox;
    Circle collisionHead;

    ////////

    bool upPressed;
    bool downPressed;

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
    float attackSpeed;
    float timeSinceLastAttack;

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
    RenderTexture2D canvas;
    Texture2D atlas;
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
    Rectangle drawableRect;
    Rectangle collisionBox;

} Bullet;

// Headers
Texture2D CreateTexture(enum BACKGROUND_TYPES bgLayer, Image srcAtlas);
void CreateBullet(Entity *entity, Bullet *bulletsPool, enum BULLET_TYPE bulletType, enum ENTITY_TYPES srcEntity);
Background CreateBackground(Player *player, Background *backgroundPool, Texture2D srcAtlas, enum BACKGROUND_TYPES bgType, int *numBackground, float scale);
Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom);
Player CreatePlayer(int maxHP, Vector2 position, int width, int height);
Enemy CreateEnemy(enum ENEMY_CLASSES class, int maxHP, Vector2 position, int width, int height);

void UpdateBackground(Player *player, Background *backgroundPool, Texture2D srcAtlas, float delta, int *numBackground, float minX, float *maxX);
void UpdateClampedCameraPlayer(Camera2D *camera, Player *player, Props *props, float delta, int width, int height, float *minX, float maxX);
void UpdatePlayer(Player *player, Enemy *enemy, Bullet *bulletPool, float delta, Props *props, float minX);
void UpdateBullets(Bullet *bullet, Enemy *enemy, Player *player, Props *props, float delta);
void UpdateEnemy(Enemy *enemy, Player *player, Bullet *bulletPool, float delta, Props *props);
void UpdateProps(Player *player, Props *props, float delta, float minX);

void DrawEnemy(Enemy *enemy, Texture2D *texture, bool drawDetectionCollision, bool drawLife, bool drawCollisionBox);
void DrawBullet(Bullet *bullet, Texture2D texture, bool drawCollisionBox);
void DrawPlayer(Player *player, Texture2D texture, bool drawCollisionBox);

RenderTexture2D PaintCanvas(Texture2D atlas, enum BACKGROUND_TYPES bgLayer);

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
    
    if ((enemy->entity.lowerAnimation.currentAnimationState != DYING) && (enemy->entity.lowerAnimation.currentAnimationState != HURT)) {
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
                    if (entity->upperAnimation.currentAnimationState != ATTACKING) 
                        entity->upperAnimation.currentAnimationState = WALKING;
                } else {
                    entity->lowerAnimation.currentAnimationState = IDLE;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING)
                        entity->upperAnimation.currentAnimationState = IDLE;
                }
            } else if (!entity->isGrounded) {
                if (entity->velocity.y < 0) {
                    entity->lowerAnimation.currentAnimationState = JUMPING;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING)
                        entity->upperAnimation.currentAnimationState = JUMPING;
                } else if(entity->velocity.y > 0) {
                    entity->lowerAnimation.currentAnimationState = FALLING;
                    if (entity->upperAnimation.currentAnimationState != ATTACKING)
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
    }

    int lAnimRow = 0;
    int uAnimRow = 0;
    int mvForward = 0;

    if (currentState != lowerAnimation->currentAnimationState) {
        lowerAnimation->timeSinceLastFrame = 0.0f;
        lowerAnimation->currentAnimationFrame = 0;
        upperAnimation->timeSinceLastFrame = 0.0f;
        upperAnimation->currentAnimationFrame = 0;
    }
    switch (lowerAnimation->currentAnimationState)
    {
    case IDLE:
        lAnimRow = 0;
        PlayEntityAnimation(entity, delta, lowerAnimation, 6, true, false, -1, false, IDLE);
        break;
    case WALKING:
        lAnimRow = 2;
        PlayEntityAnimation(entity, delta, lowerAnimation, 6, true, false, -1, false, WALKING);
        break;
    case HURT:
        lAnimRow = -1;
        //PlayEntityAnimation(entity, delta, animation, 6, false, false, -1, true, IDLE);
        break;
    case JUMPING:
        lAnimRow = 1;
        PlayEntityAnimation(entity, delta, lowerAnimation, 5, false, false, -1, false, JUMPING);
        break;
    case FALLING:
        lAnimRow = 1;
        PlayEntityAnimation(entity, delta, lowerAnimation, 5, false, true, 5, false, FALLING);
        break;
    case DYING:
        //PlayEntityAnimation(entity, delta, animation, 7, false, false, -1, false, DYING);
        break;
    case ATTACKING:
        //PlayEntityAnimation(entity, delta, animation, 6, false, false, -1, true, IDLE);
    default:
        break;
    }

    switch (upperAnimation->currentAnimationState) {
    case IDLE:
        uAnimRow = 3;
        PlayEntityAnimation(entity, delta, upperAnimation, 6, true, false, -1, false, IDLE);
        break;
    case WALKING:
        uAnimRow = 8;
        PlayEntityAnimation(entity, delta, upperAnimation, 8, true, false, -1, false, WALKING);
        break;
    case HURT:
        uAnimRow = 9; //TODO teremos essa animação?
        //PlayEntityAnimation(entity, delta, animation, 6, false, false, -1, true, IDLE);
        break;
    case JUMPING:
        uAnimRow = 4;
        PlayEntityAnimation(entity, delta, upperAnimation, 5, false, false, -1, false, JUMPING);
        break;
    case FALLING:
        uAnimRow = 4;
        PlayEntityAnimation(entity, delta, upperAnimation, 5, false, true, 5, false, FALLING);
        break;
    case DYING:
        //PlayEntityAnimation(entity, delta, animation, 7, false, false, -1, false, DYING);
        break;
    case ATTACKING:
        mvForward = 0;
        if (entity->velocity.x != 0) mvForward = 1;
        if (entity->upPressed) {
            if (mvForward == 1) {
                uAnimRow = 5;
            } else {
                uAnimRow = 7;
            }
        } else if (entity->downPressed) {
            if (mvForward == 1) {
                uAnimRow = 6;
            } else {
                uAnimRow = 7;
            }
        } else {
            uAnimRow = 7;
        }
        PlayEntityAnimation(entity, delta, upperAnimation, 4, false, false, -1, true, IDLE);
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

void EntityCollisionHandler(Entity *entity, Props *props, float delta) {
    // Colisão com props                                            ///////////////////////////////////////////////////////////////////////
    int hitObstacle = 0;
    int hasFloorBelow = 0;
    Rectangle prectGrav = entity->collisionBox;
    prectGrav.y += 1;
    for (int i = 0; i < maxNumProps; i++)
    {
        Props *eprop = props + i;
        Vector2 *p = &(entity->position);
        if (eprop->canBeStepped) {
            if (CheckCollisionRecs(eprop->rect, entity->collisionBox)) {
                hitObstacle = 1;
                entity->velocity.y = 0.0f;
                p->y = eprop->rect.y - entity->height / 2;
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
    dstEntity->lowerAnimation.currentAnimationState = HURT;
    dstEntity->lowerAnimation.timeSinceLastFrame = 0;
    dstEntity->lowerAnimation.currentAnimationFrame = 0;
    dstEntity->lowerAnimation.isFacingRight = -srcBullet.direction;
    dstEntity->currentHP -= damage;
}