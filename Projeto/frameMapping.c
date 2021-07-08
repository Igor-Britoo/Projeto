////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Player frames ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int static PLAYER_GRID[2] = {122, 122};
// LEGS são as animações exclusivas a parte inferior do player
int static PLAYER_LEGS_IDLE_ROW = 0;
int static PLAYER_LEGS_IDLE_NUM_FRAMES = 6;
int static PLAYER_LEGS_JUMPING_ROW = 1;
int static PLAYER_LEGS_JUMPING_NUM_FRAMES = 5; // Não conta com o frame de "falling"
int static PLAYER_LEGS_WALKING_ROW = 2;
int static PLAYER_LEGS_WALKING_NUM_FRAMES = 6;
// UPPER são as animações exclusivas a parte superior do player
int static PLAYER_UPPER_IDLE_ROW = 3;
int static PLAYER_UPPER_IDLE_NUM_FRAMES = 6;
int static PLAYER_UPPER_JUMPING_ROW = 4;
int static PLAYER_UPPER_JUMPING_NUM_FRAMES = 5; // Não conta com o frame de "falling"
int static PLAYER_UPPER_ATTACKING_45U_ROW = 5;
int static PLAYER_UPPER_ATTACKING_45U_NUM_FRAMES = 4;
int static PLAYER_UPPER_ATTACKING_45D_ROW = 6;
int static PLAYER_UPPER_ATTACKING_45D_NUM_FRAMES = 4;
int static PLAYER_UPPER_ATTACKING_ROW = 7;
int static PLAYER_UPPER_ATTACKING_NUM_FRAMES = 4;
int static PLAYER_UPPER_WALKING_ROW = 8;
int static PLAYER_UPPER_WALKING_NUM_FRAMES = 8;
int static PLAYER_UPPER_THROWING_ROW = 9;
int static PLAYER_UPPER_THROWING_NUM_FRAMES = 5;
// BODY são as animações do player como um todo (controlados por lowerAnimation)
int static PLAYER_BODY_DYING_ROW = 10;
int static PLAYER_BODY_DYING_NUM_FRAMES = 7;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assassin frames /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int static ASSASSIN_GRID[2] = {135, 135};
// LEGS são as animações exclusivas a parte inferior do Assassin
int static ASSASSIN_LEGS_IDLE_ROW = 0;
int static ASSASSIN_LEGS_IDLE_NUM_FRAMES = 8;
int static ASSASSIN_LEGS_WALKING_ROW = 1;
int static ASSASSIN_LEGS_WALKING_NUM_FRAMES = 8;
// UPPER são as animações exclusivas a parte superior do Assassin
int static ASSASSIN_UPPER_IDLE_ROW = 2;
int static ASSASSIN_UPPER_IDLE_NUM_FRAMES = 8;
int static ASSASSIN_UPPER_WALKING_ROW = 3;
int static ASSASSIN_UPPER_WALKING_NUM_FRAMES = 8;
int static ASSASSIN_UPPER_ATTACKING_ROW = 4;
int static ASSASSIN_UPPER_ATTACKING_NUM_FRAMES = 6;
// BODY são as animações do Assassin como um todo (controlados por lowerAnimation)
int static ASSASSIN_BODY_DYING_ROW = 5;
int static ASSASSIN_BODY_DYING_NUM_FRAMES = 8;

// GUNNER frames /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int static GUNNER_GRID[2] = {135, 135};
// LEGS são as animações exclusivas a parte inferior do GUNNER
int static GUNNER_LEGS_IDLE_ROW = 0;
int static GUNNER_LEGS_IDLE_NUM_FRAMES = 7;
int static GUNNER_LEGS_WALKING_ROW = 1;
int static GUNNER_LEGS_WALKING_NUM_FRAMES = 8;
// UPPER são as animações exclusivas a parte superior do GUNNER
int static GUNNER_UPPER_IDLE_ROW = 2;
int static GUNNER_UPPER_IDLE_NUM_FRAMES = 7;
int static GUNNER_UPPER_WALKING_ROW = 3;
int static GUNNER_UPPER_WALKING_NUM_FRAMES = 8;
int static GUNNER_UPPER_ATTACKING_ROW = 4;
int static GUNNER_UPPER_ATTACKING_NUM_FRAMES = 4;
// BODY são as animações do GUNNER como um todo (controlados por lowerAnimation)
int static GUNNER_BODY_DYING_ROW = 5;
int static GUNNER_BODY_DYING_NUM_FRAMES = 6;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Objects frames //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int static OBJECTS_GRID[2] = {200, 200};
int static OBJECTS_METAL_CRATE[2] = {0, 0};
int static OBJECTS_AMMO_CRATE[2] = {0, 1};
int static OBJECTS_HP_CRATE[2] = {0, 2};
int static OBJECTS_CARD_CRATE1[2] = {0, 3};
int static OBJECTS_CARD_CRATE2[2] = {0, 4};
int static OBJECTS_CARD_CRATE3[2] = {0, 5};
int static OBJECTS_TRASH_BIN[2] = {1, 0};
int static OBJECTS_EXPLOSIVE_BARREL[2] = {1, 1};
int static OBJECTS_METAL_BARREL[2] = {1, 2};
int static OBJECTS_GARBAGE_BAG1[2] = {1, 3};
int static OBJECTS_GARBAGE_BAG2[2] = {1, 4};
int static OBJECTS_TRASH_CONTAINER[2] = {2, 0};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Background frames ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int static BACKGROUND_GRID[2] = {120, 450};
int static BACKGROUND_SKYSCRAPER_ROW = 0;
int static BACKGROUND_SKYSCRAPER_NUM_TYPES = 4;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Midground frames ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int static MIDGROUND_GRID[2] = {178, 36};
int static MIDGROUND_SKYSCRAPER_COL = 0;
int static MIDGROUND_SKYSCRAPER_NUM_TYPES = 6;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Foreground frames ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int static FOREGROUND_GRID[2] = {200, 200};
int static FOREGROUND_NUM_TYPES = 4; // Número de colunas
int static FOREGROUND_EDGE_ROW = 0;
int static FOREGROUND_DOOR_ROW = 1;
int static FOREGROUND_WALL1_ROW = 2;
int static FOREGROUND_WALL2_ROW = 3;
int static FOREGROUND_ROOF_ROW = 4;
int static FOREGROUND_STREET_ROW = 5;
int static FOREGROUND_CHIP_IMPLANT_RECT[4] = {0, 6, 6, 4};
int static FOREGROUND_SUSHI_BAR_RECT[4] = {6, 6, 4, 4};
int static FOREGROUND_DECALS[4] = {5, 0, 2, 4};
int static FOREGROUND_BUS_STOP[2] = {5, 3};
int static FOREGROUND_LAMP_POST[2] = {6,3};
int static FOREGROUND_FENCE[2] = {7,0};
int static FOREGROUND_STREET_WALL[2] = {8,0};
int static FOREGROUND_TREE1_COL = 7;
int static FOREGROUND_TREE2_COL = 8;
int static FOREGROUND_TREE3_COL = 9;
int static FOREGROUND_TREE_ROW = 1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Misc frames /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int static MISC_GRID[2] = {122, 122};
int static MISC_BULLET_ROW = 0;
int static MISC_BULLET_NUM_FRAMES = 2;
int static MISC_BULLET_BLAZE_ROW = 1;
int static MISC_BULLET_BLAZE_NUM_FRAMES = 3;
int static MISC_SMOKE_EXPLOSION_ROW = 2;
int static MISC_SMOKE_EXPLOSION_NUM_FRAMES = 7;
int static MISC_LASER_BEAM_ROW = 3;
int static MISC_LASER_BEAM_NUM_FRAMES = 2;
int static MISC_BLOOD_SPILL_ROW = 4;
int static MISC_BLOOD_SPILL_NUM_FRAMES = 9;
int static MISC_GRANADE_ROW = 5;
int static MISC_GRANADE_NUM_FRAMES = 1;
int static MISC_EXPLOSION_ROW = 6;
int static MISC_EXPLOSION_NUM_FRAMES = 9;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Points frames /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int static PTS_KILL_ASSASSIN = 300;
int static PTS_KILL_GUNNER = 450;
int static PTS_DESTROY_CARD_CRATE = 20;
int static PTS_DESTROY_GARBAGE_BAG = 15;
int static PTS_DESTROY_EXPLOSIVE_BARREL = 150;