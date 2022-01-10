#pragma once
#define NOMINMAX 1
#define _USE_MATH_DEFINES 1

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>

#include <FastNoise/FastNoise.h>
#include "GL/glut.h"

#include "typedefs.h"
#include "defines.h"

#include "Block.h"
#include "BlockColors.h"


class Chunk
{
	std::array<Block, CHUNK_BLOCKNUM> blocks{};
	int Xpos = INT_MIN;
	std::string getPath() const;
public:
	Chunk(ChkCrd);
	Chunk();
	~Chunk();
	bool save() const;
	void draw() const;

	void fill();

	void testFill();
	void flatFill();
	void perlinFill();
	
	Block getBlockAt(BlkCrd, BlkCrd) const;
};

template<size_t H = CHUNK_HEIGHT, size_t W = CHUNK_WIDTH>
constexpr auto generate_vertices()
{
	std::array<GLfloat, (H + 1)* (W + 1) * 3> points{ };

	size_t i = 0;
	for (BlkCrd y = 0; y <= H; ++y)
	{
		for (BlkCrd x = 0; x <= W; ++x)
		{
			points[i++] = x;
			points[i++] = y;
		}
	}
	return points;
}

template<size_t H = CHUNK_HEIGHT, size_t W = CHUNK_WIDTH>
constexpr auto generate_faces()
{
	typedef GLushort DtTp;
	std::array<DtTp, H* W * 4> points{ }; // GLubyte, GLushort, GLuint

	size_t i = 0;
	for (DtTp y = 0; y < H; ++y)
	{
		for (DtTp x = 0; x < W; ++x)
		{
			points[i++] = y * (W + 1) + x;
			points[i++] = y * (W + 1) + x + 1;
			points[i++] = (y + 1) * (W + 1) + x + 1;
			points[i++] = (y + 1) * (W + 1) + x;
		}
	}
	return points;
}

template<typename T>
constexpr auto generate_colors()
{
	std::array<T, sizeof(blockColorsFloat) / sizeof(GLfloat)> colors{ };

	for (size_t i = 0; i < colors.size(); ++i)
	{
//		colors[i] = std::clamp(blockColorsFloat[i / 4][i % 4] * (std::numeric_limits<T>::max() + 1), 0.0f, (float)std::numeric_limits<T>::max());
		colors[i] = std::clamp(blockColorsFloat[i] * (std::numeric_limits<T>::max() + 1), 0.0f, (float)std::numeric_limits<T>::max());
	}
	return colors;
}


enum class Biomes { Polar, Taiga, Ocean, Plains, Forest, Rainforest, Savanna, Desert, MAX };
typedef std::underlying_type_t<Biomes> BmT;

std::string biome_to_name(Biomes);

std::array<Biomes, BIOME_WIDTH> get_biomes(ChkCrd);
std::array<BlkCrd, BIOME_WIDTH> get_height_for_biome(ChkCrd, Biomes);
std::map<Biomes, std::array<BlkCrd, BIOME_WIDTH>> get_height_for_biomes(ChkCrd, std::unordered_set<Biomes>);