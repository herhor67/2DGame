#pragma once

#include "typedefs.h"


// CHUNK FORMAT SETTINGS
static constexpr BlkCrd CHUNK_HEIGHT = 256;
static constexpr BlkCrd CHUNK_WIDTH = 16;
static constexpr BlkCrd CHUNK_BLOCKNUM = CHUNK_HEIGHT * CHUNK_WIDTH;
static constexpr BlkCrd CHUNK_VERTNUM = (CHUNK_HEIGHT + 1) * (CHUNK_WIDTH + 1);
static constexpr char CHUNK_PATH[] = "chunks/";
static constexpr char CHUNK_EXT[] = ".chnk";

static constexpr int CHUNK_TTL_MS = 10 * 1000;


// TERRAIN SETTINGS
static constexpr BlkCrd WATER_LEVEL = 64;
static constexpr BlkCrd  LAVA_LEVEL = 10;
static constexpr BlkCrd BIOME_ITPL_RDS = 16;
static constexpr BlkCrd BIOME_ITPL_RNG = 2 * BIOME_ITPL_RDS + 1;
static constexpr BlkCrd BIOME_WIDTH = CHUNK_WIDTH + 2 * BIOME_ITPL_RDS;


// FONTS
#define DEBUG_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DEBUG_FONT_H 24


// VIEWPORT SETTINGS
extern float ZOOMOUT;
static constexpr float MIN_ZOOMOUT = 16.0f;
static constexpr int ZOOMOUT_LVLS = 5;
static constexpr float DEFAULT_ASPECT = 1.77777777f; // 16/9


// RENDERING OPTIONS
#define REQUIRE_ENTIRE_CHUNK 0
#define REQUIRE_ENTIRE_BLOCK 0
#define DRAW_FACES 1
#define DRAW_BORDERS 0
#define DOUBLE_BUFFERED 1
#define CLR_NO_FLOAT 0

// Z-OFFSETS
static constexpr float Z_VAL_BACKGROUND = 1.0f; // far
static constexpr float Z_VAL_TERRAIN = 0.8f;
static constexpr float Z_VAL_MENU = -1.0f; // near
static constexpr float Z_VAL_ENTITY = 0.0f;


// COLLISIONS
#define OFFSET_CRDS 0

#define SIDE_NONE   0
#define SIDE_LEFT   1
#define SIDE_RIGHT  2
#define SIDE_BOTTOM 4
#define SIDE_TOP    8
#define SIDE_VERTICAL   (SIDE_LEFT | SIDE_RIGHT)
#define SIDE_HORIZONTAL (SIDE_BOTTOM | SIDE_TOP)


// PHYSICS
static constexpr float ENV_GRAVITY = -9.81f;


// GAME ENGINE
#define ENV_INTERACTION 0
#define ENV_COLLISIONS 0
#define DYN_ZOOMOUT 1




// DEBUG SETTINGS
#define CONSOLE_LOG_CALLBACKS 0
#define CONSOLE_LOG_EVENTS 0
#define CONSOLE_LOG_CHUNKS 1
#define CONSOLE_LOG_GENERATION 1
#define CONSOLE_LOG_MOVEMENT 0

#define DEBUG_COLOR_BIOMES 0
#define DEBUG_RENDER_ARRAYS 1