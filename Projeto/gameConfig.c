#include <stdlib.h>     
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "raylib.h"

#define WP_GUN_ID 1;
#define WP_MACHINEGUN_ID 2;
#define WP_SWORD_ID 3;

// Enums
enum CHARACTER_STATE{IDLE, WALKING, HURT, JUMPING, FALLING, SHOOTING, DYING};
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

    // Animation
    enum CHARACTER_STATE currentAnimationState;
    Rectangle currentAnimationFrameRect;
    int isFacingRight;
    float animationFrameSpeed;
    int animationFrameWidth;
    int animationFrameHeight;
    int currentAnimationFrame;
    float timeSinceLastFrame;
    float characterWidthScale;
    float characterHeightScale;
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
    int viewDistance;

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
void UpdatePlayer(Player *player, float delta, Props *props, float minX);
void UpdateEnemy(Enemy *enemy, Player *player, float delta, Props *props);

void UpdateProps(Player *player, Props *props, float delta, float minX);