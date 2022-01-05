#pragma once

// DEBUG SETTINGS
#define CONSOLE_LOG_CALLBACKS 0
#define CONSOLE_LOG_EVENTS 0
#define CONSOLE_LOG_CHUNKS 1


// CHUNK SETTINGS
#define CHUNK_HEIGHT 256
#define CHUNK_WIDTH  8
#define CHUNK_BLOCKNUM (CHUNK_HEIGHT*CHUNK_WIDTH)

#define CHUNK_PATH "chunks/"
#define CHUNK_EXT ".chnk"

#define CHUNK_TTL_MS (10*1000)


// TERRAIN SETTINGS
#define WATER_LEVEL 128
#define  LAVA_LEVEL 10



// Z-OFFSETS
#define Z_VAL_BACKGROUND 1.0f  // far
#define Z_VAL_TERRAIN 0.8f  // far
#define Z_VAL_MENU  (-1.0f) // near
#define Z_VAL_ENTITY  0.0f


// FONTS
#define DEBUG_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DEBUG_FONT_H 24


// VIEWPORT SETTINGS
#define ZOOMOUT 300.0f
//#define ZOOMOUT 4.0f
#define DEFAULT_ASPECT 1.7777777777777777 // 16/9
#define REQUIRE_ENTIRE 0


// DRAWING OPTIONS
#define DRAW_FACES 1
#define DRAW_BORDERS 0
#define DOUBLE_BUFFERED 0


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