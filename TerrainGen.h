#pragma once
#define _USE_MATH_DEFINES 1

#include <cmath>
#include <unordered_set>
#include <utility>

#include "typedefs.h"
#include "defines.h"

#include "Chunk.h"


enum class Biomes { MIN, Polar, Taiga, Mountains, Ocean, Hills, Plains, Forest, Rainforest, Savanna, Desert, MAX };
typedef std::underlying_type_t<Biomes> BmT;



class TerrainGen
{
	ChkCrd Xpos = INT_MIN;
	std::array<Block, CHUNK_BLOCKNUM>& dataRef;
	std::array<Biomes, BIOME_WIDTH> biomeArr{};
	std::unordered_set<Biomes> biomesUnq;
	std::map<Biomes, std::array<float, CHUNK_WIDTH>> heightsArr;
	std::array<BlkCrd, CHUNK_WIDTH> heightArr{};

	void get_biomes();
	void get_height_for_biome(Biomes);
	void get_height_for_biomes();
public:
	static std::string biome_to_name(Biomes);

	void get_height();

	void fill_chunk();

	TerrainGen(ChkCrd, std::array<Block, CHUNK_BLOCKNUM>&);
	~TerrainGen();
};