#pragma once

#include <array>
#include <string>

#include "typedefs.h"
#include "defines.h"

#include "Block.h"



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
	void test();
	void flatFill();
	void fill();
	Block getBlockAt(BlkCrd, BlkCrd) const;
};