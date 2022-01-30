#pragma once
#include "defines.h"

#include <map>

#include "Chunk.h"
#include "Environment.h"
#include "Entity.h"


std::vector<BlkCrd> possibleBlocksCollisions(EntCrd p1, EntCrd p2);


class ChunkManager
{
	typedef std::pair<Chunk, int> ChT;

	// coordinate -> <Chunk, TTL>
	std::map<ChkCrd, ChT> chunks;
	int Tnow = 0;
public:
	void updateTnow(int);
	void removeOld();
	void updateTTL(ChkCrd);
	ChT& readChunk(ChkCrd);
	void loadChunk(ChkCrd);
	Chunk& getChunk(ChkCrd);
	size_t count();

	BlockN getBlockAt(BlkCrd, BlkCrd);

	// Pos, BlockX, BlockY, side, ratio
	std::tuple<Pos, BlockN, BlockN, int, EntCrd> checkCollision(const Movement&, const Entity&);
};