#pragma once

#include "Block.h"

#if DEBUG_COLOR_BIOMES
static constexpr float blockColorsFloat[] = {
	0.0f,	0.0f,	0.0f,	0.1f,	//	air

	0.0f,	0.0f,	0.0f,	1.0f,
	0.0f,	0.0f,	0.5f,	1.0f,
	0.0f,	0.5f,	0.0f,	1.0f,
	0.0f,	0.5f,	0.5f,	1.0f,
	0.5f,	0.0f,	0.0f,	1.0f,
	0.5f,	0.0f,	0.5f,	1.0f,
	0.5f,	0.5f,	0.0f,	1.0f,
	0.5f,	0.5f,	0.5f,	1.0f,

	0.0f,	0.0f,	1.0f,	1.0f,
	0.0f,	1.0f,	0.0f,	1.0f,
	0.0f,	1.0f,	1.0f,	1.0f,
	1.0f,	0.0f,	0.0f,	1.0f,
	1.0f,	0.0f,	1.0f,	1.0f,
	1.0f,	1.0f,	0.0f,	1.0f,
	0.75f,	0.75f,	0.75f,	1.0f,
	1.0f,	1.0f,	1.0f,	1.0f,
};
#else
static constexpr float blockColorsFloat[] = {
	0.0f,		0.0f,		0.0f,		0.1f,	// air
	0.5f,		0.5f,		0.5f,		1.0f,	// stone
	0.5451f,	0.2706f,	0.0745f,	1.0f,	// dirt
	0.0f,		0.5f,		0.0f,		1.0f,	// grass
	0.2f,		0.2f,		0.2f,		1.0f,	// bedrock
	0.0f,		0.02f,		1.0f,		0.75f,	// water
	1.0f,		0.2f,		0.04f,		0.9f,	// lava
	1.0f,		1.0f,		0.0f,		1.0f,	// sand
	0.8f,		0.8f,		0.0f,		1.0f,	// sandstone
	1.0f,		1.0f,		1.0f,		1.0f,	// snow
	0.6667f,	0.7882f,	1.0f,		0.75f,	// ice
	0.1843f,	0.302f,		0.0745f,	1.0f,	// cactoo
	0.8941f,	0.3255f,	0.0706f,	1.0f,	// pumpkin
	0.1608f,	0.0706f,	0.0f,		1.0f,	// spruce wood
	0.0353f,	0.0745f,	0.0392f,	1.0f,	// spruce leaves
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
	0.0f,		0.0f,		0.0f,		1.0f,
};
#endif
