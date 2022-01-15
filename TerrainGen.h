#pragma once
#define _USE_MATH_DEFINES 1

#include <cmath>
#include <functional>
#include <mutex>
#include <optional>
#include <unordered_set>
#include <utility>

#include "typedefs.h"
#include "defines.h"
#include "functions.h"

#include "Chunk.h"


enum class BiomeN { MIN=-3, Polar, Taiga, Mountains, Ocean, Hills, Plains, Forest, Rainforest, Savanna, Desert, MAX };
typedef std::underlying_type_t<BiomeN> Bm_t;

enum class PlantN { MIN = -1, Cactoo, Pumpkin, Oak, Spruce, Accacia, JungleTree, MAX };
typedef std::underlying_type_t<PlantN> Pl_t;

template<size_t L = BIOME_ITPL_RDS>
constexpr auto generate_gauss_interpolation()
{
	constexpr float sigmas = 2.5f;
	constexpr float std = (L ? L : 1) / sigmas;
	constexpr float DCoffset = gauss_pdf_dscrt<L, std>(-2); // -1?

	std::array<float, L+1> multipliers{ };

	size_t i = 0;
	float sum = 0.0f;
	for (size_t i = 0; i < L; ++i)
	{
		multipliers[i] = gauss_pdf_dscrt<L, std>(i) - DCoffset;
		sum += 2 * multipliers[i];
	}
	multipliers[L] = gauss_pdf_dscrt<L, std>(L) - DCoffset;
	sum += multipliers[i];

	for (size_t i = 0; i <= L; ++i)
		multipliers[i] /= sum;

	return multipliers;
}


class TerrainGen
{
	ChkCrd Xpos = INT_MIN;
	std::array<Block, CHUNK_BLOCKNUM>& dataRef;
	std::array<BiomeN, BIOME_WIDTH> biomeArr{};
	std::unordered_set<BiomeN> biomesUnq;
	std::array<BlkCrd, TERRAIN_WIDTH> heightArr{};
	std::array<bool, CHUNK_BLOCKNUM> caves{};

	static std::once_flag noiseInitFlag;
	static FastNoise::SmartNode<FastNoise::CellularValue> biomeGenerator;
	static FastNoise::SmartNode<FastNoise::FractalFBm> perlinGenerator;
	static FastNoise::SmartNode<FastNoise::FractalFBm> simplexGenerator;
	static FastNoise::SmartNode<FastNoise::Remap> plantGenerator;

	static       Block  BlNullRefSet;
	static const Block  BlNullRefGet;
	static const BiomeN BmNullRefGet;
	static const BlkCrd HtNullRefGet;

#if HEIGHT_ITPL_GAUSS
	static constexpr std::array<float, BIOME_ITPL_RDS + 1> itpl_coeffs = generate_gauss_interpolation();
#endif

	inline void get_biomes();
	inline void get_height_for_biome(BiomeN, std::array<float, TERRAIN_WIDTH>&);
	inline void get_height();
	inline void generate_caves();
	inline void fill_with_stone() const;
	inline void fill_with_fluids() const;
	inline void surface_layers() const;
	inline void add_vegetation() const;
	inline void protect_bedrock() const;

	inline void debug_color_biomes() const;

	inline void place_plant(PlantN, BlkCrd) const;

	
	inline constexpr Block& blockAtSet(BlkCrd, BlkCrd) const;
	inline constexpr const Block& blockAtGet(BlkCrd, BlkCrd) const;
	inline constexpr const BiomeN& biomeAtGet(BlkCrd) const;
	inline constexpr const BlkCrd& heightAtGet(BlkCrd) const;



public:
	TerrainGen(ChkCrd, std::array<Block, CHUNK_BLOCKNUM>&);
	~TerrainGen();

	void generate_chunk();

	static constexpr std::string biome_to_name(BiomeN);
};
