#pragma once
#include "defines.h"
#include "functions.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <string>

#include "GL/glut.h"

#include "GLhelpers.h"
#include "Block.h"



class Chunk
{
	ChkCrd Xpos = INT_MIN;
	std::array<BlockN, CHUNK_BLOCKNUM> blocks{};
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
	
	BlockN getBlockAt(BlkCrd, BlkCrd) const;
};

