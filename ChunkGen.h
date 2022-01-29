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
#include "StaticGenerators.h"


enum class BiomeN : uint32_t { MIN = 0, Polar, Taiga, Mountains, Ocean, Hills, Plains, Forest, Redwoodforest, Savanna, Desert, MAX };
typedef std::underlying_type_t<BiomeN> Bm_t;

enum class StrctrN : uint32_t { MIN = 0, Cactoo, Pumpkin, Oak, Spruce, Acacia, Redwood, Igloo, Coral, MAX };
typedef std::underlying_type_t<StrctrN> St_t;



#if HEIGHT_ITPL_GAUSS
static constexpr std::array<float, BIOME_ITPL_RDS + 1> generate_biome_weights()
{
	constexpr float sigmas = 2.5f;
	constexpr float std = (BIOME_ITPL_RDS ? BIOME_ITPL_RDS : 1) / sigmas;
	constexpr float DCoffset = gauss_pdf_dscrt(-2, BIOME_ITPL_RDS, std); // -1?

	std::array<float, BIOME_ITPL_RDS + 1> multipliers{ };

	size_t i = 0;
	float sum = 0.0f;
	for (size_t i = 0; i < BIOME_ITPL_RDS; ++i)
	{
		multipliers[i] = gauss_pdf_dscrt(i, BIOME_ITPL_RDS, std) - DCoffset;
		sum += 2 * multipliers[i];
	}
	multipliers[BIOME_ITPL_RDS] = gauss_pdf_dscrt(BIOME_ITPL_RDS, BIOME_ITPL_RDS, std) - DCoffset;
	sum += multipliers[BIOME_ITPL_RDS];

	for (size_t i = 0; i <= BIOME_ITPL_RDS; ++i)
		multipliers[i] /= sum;

	return multipliers;
}
#endif



class ChunkGen
{
	ChkCrd Xpos = INT_MIN;
	std::array<Block, CHUNK_BLOCKNUM>& dataRef;
	std::array<BiomeN, BIOME_WIDTH> biomeArr{};
	std::unordered_set<BiomeN> biomesUnq;
	std::array<BlkCrd, TERRAIN_WIDTH> heightArr{};
	std::array<bool, CHUNK_BLOCKNUM> caves{};

//	const Generators& generators;

	static       Block  BlNullRefSet;
	static const Block  BlNullRefGet;
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

	inline Block& blockAtSet(BlkCrd, BlkCrd) const;
	inline const Block& blockAtGet(BlkCrd, BlkCrd) const;
	inline const BiomeN& biomeAtGet(BlkCrd) const;
	inline const BlkCrd& heightAtGet(BlkCrd) const;

public:
	ChunkGen(ChkCrd, std::array<Block, CHUNK_BLOCKNUM>&);
	~ChunkGen();

	void generate_chunk();

	static constexpr std::string biome_to_name(BiomeN);
};


