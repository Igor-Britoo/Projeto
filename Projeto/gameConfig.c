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
enum ENEMY_BEHAVIOR{NONE, ATTACK, MOVE};
enum BACKGROUND_TYPES{BACKGROUND, MIDDLEGROUND, FOREGROUND};
enum ENEMY_CLASSES{SWORDSMAN, GUNNER, SNIPER, DRONE, TURRET, BOSS};

// Consts
const float GRAVITY = 400;
const static int numBackgroundRendered = 5;
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
    float walkSpeed;
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

} Enemy;

typedef struct props {
    Rectangle rect;
    int canBeStepped;
    int blockPlayer;
    int isInvisible;

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

} Particle;

typedef struct weapon 
{
    int id;
    int maxAmmo;
    int baseDamage;

} Weapon;

// Headers
Background CreateBackground(Player *player, Background *backgroundPool, enum BACKGROUND_TYPES bgType, int *numBackground, float scale);
Camera2D CreateCamera (Vector2 target, Vector2 offset, float rotation, float zoom);
Player CreatePlayer(int maxHP, Vector2 position, float imageWidth, float imageHeight);
Enemy CreateEnemy(enum ENEMY_CLASSES class, int maxHP, Vector2 position, float imageWidth, float imageHeight);

void UpdateBackground(Player *player, Background *backgroundPool, int pointer, float delta, int *numBackground, float minX, float *maxX);
void UpdateClampedCameraPlayer(Camera2D *camera, Player *player, Props *props, float delta, int width, int height, float *minX, float maxX);
void UpdatePlayer(Player *player, Enemy *enemy, float delta, Props *props, float minX);
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

void RangedSteeringBehavior(Enemy *enemy, Player *player, float delta) {
    Entity *eEnt = &(enemy->entity); // Pointer direto para a Entity do inimigo

    float eyesX = eEnt->position.x + eEnt->eyesOffset.x;
    float eyesY = eEnt->position.y + eEnt->eyesOffset.y;
    Rectangle detectionBox;
    if (eEnt->animation.isFacingRight == 1) {
        detectionBox = (Rectangle){eyesX, eyesY, enemy->viewDistance, 5};
    } else {
        detectionBox = (Rectangle){eyesX-enemy->viewDistance, eyesY, enemy->viewDistance, 5};
    }
    Rectangle playerBox = (Rectangle) {player->entity.position.x, player->entity.position.y, player->entity.animation.animationFrameWidth*player->entity.characterWidthScale, player->entity.animation.animationFrameHeight*player->entity.characterHeightScale};
    
    if (CheckCollisionRecs(detectionBox, playerBox)) { // Se houver detecção, setar target
        enemy->noDetectionTime = 0;
        if (enemy->behavior == NONE) { // Se não tiver target
            enemy->behavior = MOVE;
            SetTarget(player->entity.position, enemy);
        } else { // Se já tiver target
            SetTarget(player->entity.position, enemy); // Atualiza target
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
                if (attackX > enemy->target.x + player->entity.animation.animationFrameWidth * player->entity.characterWidthScale) { // Ainda não chegou
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
            SetTarget(player->entity.position, enemy); // Atualiza target
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

void UpdateAnimation(Animation *animation, float numOfFrames, bool isLoopable, bool isFixed, int fixedFrame, bool transitToAnotherState, enum CHARACTER_STATE nextState) {
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
        }
    }
}

int GetAnimRow (Entity *entity, float delta, enum CHARACTER_STATE currentState) {
    Animation *animation = &(entity->animation);
    int animRow = 0;
    if (currentState != animation->currentAnimationState) {
        animation->timeSinceLastFrame = 0.0f;
        animation->currentAnimationFrame = 0;
    }
    switch (animation->currentAnimationState)
    {
    case IDLE:
        animRow = 0;
        UpdateAnimation(animation, 6, true, false, -1, false, IDLE);
        break;
    case WALKING:
        animRow = 1;
        UpdateAnimation(animation, 8, true, false, -1, false, WALKING);
        break;
    case HURT:
        animRow = 2;
        UpdateAnimation(animation, 6, false, false, -1, true, IDLE); // Deslocamento não está implementado
        if (animation->timeSinceLastFrame >= animation->animationFrameSpeed) {
            if (animation->currentAnimationFrame > 5) { // 5 porque são 6 frames para essa animação, depois muda o estado
            } else if (animation->currentAnimationFrame < 2) {
                entity->position.x -= animation->isFacingRight*600*delta;
            }
        }
        break;
    case JUMPING:
        animRow = 3;
        UpdateAnimation(animation, 5, false, false, -1, false, JUMPING);
        break;
    case FALLING:
        animRow = 3;
        UpdateAnimation(animation, 5, false, true, 5, false, FALLING);
        break;
    case DYING:
        animRow = 4;
        UpdateAnimation(animation, 7, false, false, -1, false, DYING); // Deslocamento não está implementado
        if (animation->timeSinceLastFrame >= animation->animationFrameSpeed) {
            if (animation->currentAnimationFrame > 6) { // 6 porque são 7 frames para essa animação, além disso, mantém o frame em "6"
            } else {
                entity->position.x -= animation->isFacingRight*1000*delta;
            }
            if (!entity->isGrounded) entity->position.x -= animation->isFacingRight*1000*delta;
        }
        break;
    case ATTACKING:
        animRow = 6;
        UpdateAnimation(animation, 6, false, false, -1, true, IDLE); // Deslocamento não está implementado
    default:
        break;
    }

    return animRow;
}