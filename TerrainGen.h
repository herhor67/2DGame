#pragma once
#define _USE_MATH_DEFINES 1

#include <cmath>
#include <mutex>
#include <unordered_set>
#include <utility>

#include "typedefs.h"
#include "defines.h"
#include "functions.h"

#include "Chunk.h"


enum class Biomes { MIN, Polar, Taiga, Mountains, Ocean, Hills, Plains, Forest, Rainforest, Savanna, Desert, MAX };
typedef std::underlying_type_t<Biomes> BmT;


template<size_t L = BIOME_ITPL_RNG>
constexpr auto generate_gauss_interpolation()
{
	constexpr float sigmas = 2.5f;
	constexpr float std = (BIOME_ITPL_RDS ? BIOME_ITPL_RDS : 1) / sigmas;

	std::array<float, L> multipliers{ };

	constexpr float DCoffset = gauss_pdf_dscrt<BIOME_ITPL_RDS, std>(-2); // -1?

	size_t i = 0;
	float sum = 0.0f;
	for (size_t i = 0; i < L; ++i)
	{
		multipliers[i] = gauss_pdf_dscrt<BIOME_ITPL_RDS, std>(i) - DCoffset;
		sum += multipliers[i];
	}
	for (size_t i = 0; i < L; ++i)
		multipliers[i] /= sum;

	return multipliers;
}


class TerrainGen
{
	ChkCrd Xpos = INT_MIN;
	std::array<Block, CHUNK_BLOCKNUM>& dataRef;
	std::array<Biomes, BIOME_WIDTH> biomeArr{};
	std::unordered_set<Biomes> biomesUnq;
	std::map<Biomes, std::array<float, CHUNK_WIDTH>> heightsArr;
	std::array<BlkCrd, CHUNK_WIDTH> heightArr{};

	static std::once_flag noiseInitFlag;
	static FastNoise::SmartNode<FastNoise::CellularValue> biomeGenerator;
	static FastNoise::SmartNode<FastNoise::FractalFBm> perlinGenerator;
	static FastNoise::SmartNode<FastNoise::FractalFBm> simplexGenerator;

	static constexpr std::array<float, BIOME_ITPL_RNG> itpl_coeffs = generate_gauss_interpolation();


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
