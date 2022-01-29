#pragma once
#include "defines.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <string>

#include "GL/glut.h"

#include "functions.h"

#include "Block.h"
#include "BlockColors.h"


constexpr auto generate_vertices()
{
	std::array<GLfloat, CHUNK_VERTNUM * 3> points{};

	size_t i = 0;
	for (BlkCrd y = 0; y <= CHUNK_HEIGHT; ++y)
	{
		for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		{
			points[i++] = x;
			points[i++] = y;
		}
		points[i++] = nextafter(float(CHUNK_WIDTH));
		points[i++] = y;
	}
	return points;
}

constexpr auto generate_faces()
{
	typedef GLushort DtTp;
	std::array<DtTp, CHUNK_BLOCKNUM * 4> points{}; // GLubyte, GLushort, GLuint

	size_t i = 0;
	for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
	{
		for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		{
			points[i++] = DtTp(y * (CHUNK_WIDTH + 1) + x);
			points[i++] = DtTp(y * (CHUNK_WIDTH + 1) + x + 1);
			points[i++] = DtTp((y + 1) * (CHUNK_WIDTH + 1) + x + 1);
			points[i++] = DtTp((y + 1) * (CHUNK_WIDTH + 1) + x);
		}
	}
	return points;
}


class Chunk
{
	ChkCrd Xpos = INT_MIN;
	std::array<Block, CHUNK_BLOCKNUM> blocks{};
	std::string getPath() const;

	static constexpr auto pointsArr = generate_vertices();
	static constexpr auto pointsPtr = pointsArr.data();
	static constexpr auto facesArr = generate_faces();
	static constexpr auto facesPtr = facesArr.data();

public:
	Chunk(ChkCrd);
	Chunk();
	~Chunk();
	bool save() const;
	void draw(BlkCrd, BlkCrd) const;

	void testFill();
	void flatFill();
	
	Block getBlockAt(BlkCrd, BlkCrd) const;
};


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
