#pragma once

// DEBUG SETTINGS
#define CONSOLE_LOG_CALLBACKS 0
#define CONSOLE_LOG_EVENTS 0
#define CONSOLE_LOG_CHUNKS 1
#define CONSOLE_LOG_MOVEMENT 0


// CHUNK FORMAT SETTINGS
#define CHUNK_HEIGHT 256
#define CHUNK_WIDTH  16
#define CHUNK_BLOCKNUM (CHUNK_HEIGHT*CHUNK_WIDTH)
#define CHUNK_VERTNUM ((CHUNK_HEIGHT+1)*(CHUNK_WIDTH+1))
#define CHUNK_PATH "chunks/"
#define CHUNK_EXT ".chnk"

#define CHUNK_TTL_MS (10*1000)


// TERRAIN SETTINGS
#define WATER_LEVEL 128
#define  LAVA_LEVEL 10
#define BIOME_ITPL_R 16
#define BIOME_WIDTH (CHUNK_WIDTH + 2 * BIOME_ITPL_R)


// FONTS
#define DEBUG_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DEBUG_FONT_H 24


// VIEWPORT SETTINGS
#define ZOOMOUT 400.0f
#define DEFAULT_ASPECT 1.7777777777777777 // 16/9
#define REQUIRE_ENTIRE 0


// RENDERING OPTIONS
#define DRAW_FACES 1
#define DRAW_BORDERS 0
#define DOUBLE_BUFFERED 1
#define CLR_NO_FLOAT 0
// Z-OFFSETS
#define Z_VAL_BACKGROUND 1.0f  // far
#define Z_VAL_TERRAIN 0.8f
#define Z_VAL_MENU  (-1.0f) // near
#define Z_VAL_ENTITY  0.0f


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
#define ENV_GRAVITY (-9.81f)


// GAME ENGINE
#define ENV_INTERACTION 0
#define ENV_COLLISIONS 0