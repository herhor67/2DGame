#pragma once
#include "defines.h"

#include <cmath>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_set>
#include <utility>

#include "functions.h"

#include "Chunk.h"
#include "GLhelpers.h"
#include "StaticGenerators.h"


enum class BiomeN : uint32_t { MIN = 0, Polar, Taiga, Mountains, Ocean, Hills, Plains, Forest, Redwoodforest, Savanna, Desert, MAX };
typedef std::underlying_type_t<BiomeN> Bm_t;

enum class StrctrN : uint32_t { MIN = 0, Cactoo, Pumpkin, Oak, Spruce, Acacia, Redwood, Igloo, Coral, MAX };
typedef std::underlying_type_t<StrctrN> St_t;




class ChunkGen
{
	ChkCrd Xpos = INT_MIN;
	std::array<BlockN, CHUNK_BLOCKNUM>& dataRef;
	std::array<BiomeN, BIOME_WIDTH> biomeArr{};
	std::unordered_set<BiomeN> biomesUnq;
	std::array<BlkCrd, TERRAIN_WIDTH> heightArr{};
	std::array<bool, CHUNK_BLOCKNUM> caves{};

//	const Generators& generators;

	static       BlockN  BlNullRefSet;
	static const BlockN  BlNullRefGet;
	static const BiomeN BmNullRefGet;
	static const BlkCrd HtNullRefGet;

#if HEIGHT_ITPL_GAUSS
	static constexpr auto itpl_coeffs = generate_biome_weights();
#endif

	inline void get_biomes();
	inline void get_height_for_biome(BiomeN, std::array<float, TERRAIN_WIDTH>&);
	inline void get_height();
	inline void generate_caves();
	inline void fill_with_stone() const;
	inline void fill_with_fluids() const;
	inline void surface_layers() const;
	inline void add_structures() const;
	inline void protect_bedrock() const;

	inline void debug_color_biomes() const;

	inline void generate_structure(StrctrN, BlkCrd) const;

	inline BlockN& blockAtSet(BlkCrd, BlkCrd) const;
	inline const BlockN& blockAtGet(BlkCrd, BlkCrd) const;
	inline const BiomeN& biomeAtGet(BlkCrd) const;
	inline const BlkCrd& heightAtGet(BlkCrd) const;

public:
	ChunkGen(ChkCrd, std::array<BlockN, CHUNK_BLOCKNUM>&);
	~ChunkGen();

	void generate_chunk();

	static constexpr std::string biome_to_name(BiomeN);
};


