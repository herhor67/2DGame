#pragma once
#include "defines.h"

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
	0.0f,	0.0f,	0.0f,	0.01f,	// air
	0.431f,	0.447f,	0.459f,	1.0f,	// stone
	0.529f,	0.38f,	0.29f,	1.0f,	// dirt
	0.322f,	0.596f,	0.0f,	1.0f,	// grass
	0.235f,	0.243f,	0.247f,	1.0f,	// bedrock
	0.173f,	0.478f,	1.659f,	0.9f,	// water
	1.0f,	0.2f,	0.04f,	0.9f,	// lava
	0.91f,	0.851f,	0.769f,	1.0f,	// sand
	0.715f,	0.583f,	0.395f,	1.0f,	// sandstone
	1.0f,	1.0f,	1.0f,	1.0f,	// snow
	0.667f,	0.788f,	1.0f,	0.8f,	// ice
	0.184f,	0.302f,	0.075f,	1.0f,	// cactoo
	0.894f,	0.326f,	0.071f,	1.0f,	// pumpkin
	0.161f,	0.07f,	0.0f,	1.0f,	// spruce wood
	0.035f,	0.075f,	0.039f,	0.9f,	// spruce leaves
	0.357f,	0.141f,	0.0f,	1.0f,	// oak wood
	0.097f,	0.304f,	0.0f,	0.9f,	// oak leaves
	0.706f,	0.467f,	0.286f,	1.0f,	// accacia wood
	0.475f,	0.416f,	0.188f,	0.9f,	// accacia leaves
	0.357f,	0.106f,	0.031f,	1.0f,	// redwood wood
	0.102f,	0.188f,	0.051f,	0.9f,	// redwood leaves
	0.945f,	0.118f,	0.208f,	1.0f,	// red coral
	0.239f,	0.733f,	0.173f,	1.0f,	// green coral
	0.812f,	0.694f,	0.012f,	1.0f,	// yellow coral
	0.011f,	0.051f,	0.384f,	1.0f,	// blue coral
	0.718f,	0.498f,	0.204f,	1.0f,	// dry grass
	0.0f,	0.0f,	0.0f,	1.0f,
	0.0f,	0.0f,	0.0f,	1.0f,
	0.0f,	0.0f,	0.0f,	1.0f,
	0.0f,	0.0f,	0.0f,	1.0f,
};
#endif
