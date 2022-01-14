#pragma once

#if DEBUG_COLOR_BIOMES
static constexpr float blockColorsFloat[] = {
	0.0f, 0.0f, 0.0f, 0.1f, // air

	0.0f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 0.5f, 1.0f,
	0.0f, 0.5f, 0.0f, 1.0f,
	0.0f, 0.5f, 0.5f, 1.0f,
	0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.0f, 0.5f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f,

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.75f,0.75f,0.75f,1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};
#else
static constexpr float blockColorsFloat[] = {
	0.0f,      0.0f,      0.0f,       0.1f, // air
	0.5f,      0.5f,      0.5f,       1.0f, // stone
	0.545098f, 0.270588f, 0.0745098f, 1.0f, // dirt
	0.0f,      0.5f,      0.0f,       1.0f, // grass
	1.0f,      0.0f,      0.0f,       1.0f,
	0.0f,      1.0f,      0.0f,       1.0f,
	0.2f,      0.2f,      0.2f,       1.0f, // bedrock
	0.0f,      0.0f,      1.0f,       1.0f,
	0.0f,      0.02f,     1.0f,       0.75f, // water
	1.0f,      0.2f,      0.2f,       0.75f, // lava
	1.0f,      1.0f,      0.0f,       1.0f, // sand
	0.0f,      0.0f,      0.0f,       1.0f,
	0.0f,      0.0f,      0.0f,       1.0f,
	0.0f,      0.0f,      0.0f,       1.0f,
};
#endif
