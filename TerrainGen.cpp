
#include <span>
#include "functions.h"
#include "TerrainGen.h"


std::once_flag TerrainGen::noiseInitFlag = std::once_flag();
FastNoise::SmartNode<FastNoise::CellularValue> TerrainGen::biomeGenerator   = FastNoise::New<FastNoise::CellularValue>();
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::perlinGenerator  = FastNoise::New<FastNoise::FractalFBm>();
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::simplexGenerator = FastNoise::New<FastNoise::FractalFBm>();
FastNoise::SmartNode<FastNoise::Remap>         TerrainGen::plantGenerator   = FastNoise::New<FastNoise::Remap>();

      Block  TerrainGen::BlNullRefSet = BlockN::MAX;
const Block  TerrainGen::BlNullRefGet = BlockN::MAX;
const BiomeN TerrainGen::BmNullRefGet = BiomeN::MIN;
const BlkCrd TerrainGen::HtNullRefGet = INT_MIN;


TerrainGen::TerrainGen(ChkCrd _Xpos, std::array<Block, CHUNK_BLOCKNUM>& _dataRef) : Xpos(_Xpos), dataRef(_dataRef)
{
	std::call_once(noiseInitFlag, []()
		{
			biomeGenerator->SetJitterModifier(0.7f);

			perlinGenerator->SetSource(FastNoise::New<FastNoise::Perlin>());
			perlinGenerator->SetOctaveCount(3);
			perlinGenerator->SetGain(0.5f);
			perlinGenerator->SetLacunarity(2.0f);

			simplexGenerator->SetSource(FastNoise::New<FastNoise::Simplex>());
			simplexGenerator->SetOctaveCount(3);
			simplexGenerator->SetGain(0.5f);
			simplexGenerator->SetLacunarity(2.0f);

			plantGenerator->SetSource(FastNoise::New<FastNoise::White>());
			plantGenerator->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);
		});

}

TerrainGen::~TerrainGen() { }

// get array of biomes per coordinate and make a list of uniques
void TerrainGen::get_biomes()
{
	std::array<float, BIOME_WIDTH> biomeNoise{};

	biomeGenerator->GenUniformGrid2D(biomeNoise.data(), Xpos * CHUNK_WIDTH - BIOME_OFFSET, 0, BIOME_WIDTH, 1, 0.003f, 123455);
	
	for (BlkCrd i = 0; i < BIOME_WIDTH; ++i)
	{
//		float value = std::fabs(biomeNoise[i]);
		float value = biomeNoise[i] * 0.5f + 0.5f;

		constexpr Bm_t biomeRange = Bm_t(BiomeN::MAX) - 1 - Bm_t(BiomeN::MIN);

		Bm_t choosen = value * biomeRange + Bm_t(BiomeN::MIN) + 1;
//		std::cout << choosen << ' ';
		choosen = std::clamp(choosen, Bm_t(BiomeN::MIN) + 1, Bm_t(BiomeN::MAX) - 1);

//		BiomeN biome = BiomeN(std::clamp(Bm_t(std::fabs(biomeNoise[i]) * Bm_t(BiomeN::MAX) - 1) + 1, 1, Bm_t(BiomeN::MAX) - 1));
		BiomeN biome = BiomeN(choosen);

		biome = BiomeN::Taiga;

		biomeArr[i] = biome;
		biomesUnq.insert(biome);
	}
#if CONSOLE_LOG_GENERATION
	std::cout << "Biomes found: ";
	for (const auto& bm : biomesUnq)
		std::cout << biome_to_name(bm) << ' ';
	std::cout << std::endl;
#endif
}

// calculate heights per coordinate for given biome
void TerrainGen::get_height_for_biome(BiomeN biome, std::array<float, TERRAIN_WIDTH>& ptr)
{
//	for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
//		ptr[i] = WATER_LEVEL + (Bm_t(biome) - 3) * 5;
//	return;
	
	BlkCrd Xmin = Xpos * CHUNK_WIDTH - TERRAIN_OFFSET;
	std::array<float, TERRAIN_WIDTH> heightNoise{};

	switch (biome)
	{
	case BiomeN::Polar:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->SetLacunarity(2.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.005f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.75f + WATER_LEVEL;
		break;


	case BiomeN::Taiga:
		perlinGenerator->SetOctaveCount(2);
		perlinGenerator->SetGain(2.0f);
		perlinGenerator->SetLacunarity(2.5f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.15f + WATER_LEVEL * 1.02f;
		break;


	case BiomeN::Mountains:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.6f);
		perlinGenerator->SetLacunarity(3.14159f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.003f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * CHUNK_HEIGHT * 0.8f + CHUNK_HEIGHT * 0.5f;
		break;


	case BiomeN::Ocean:
		perlinGenerator->SetOctaveCount(5);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->SetLacunarity(3.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.001f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = std::fabs(heightNoise[i]) * WATER_LEVEL * 2.0f + WATER_LEVEL * 0.2f;
		break;


	case BiomeN::Hills:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->SetLacunarity(2.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.9f + WATER_LEVEL * 1.3f;
		break;


	case BiomeN::Plains:
		perlinGenerator->SetOctaveCount(2);
		perlinGenerator->SetGain(2.0f);
		perlinGenerator->SetLacunarity(2.5f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.25f + WATER_LEVEL * 1.07f;
		break;


	case BiomeN::Forest:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.6f);
		perlinGenerator->SetLacunarity(2.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.4f + WATER_LEVEL * 1.2f;
		break;


	case BiomeN::Rainforest:
		perlinGenerator->SetOctaveCount(4);
		perlinGenerator->SetGain(0.7f);
		perlinGenerator->SetLacunarity(1.8f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.008f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.8f + WATER_LEVEL * 1.15f;
		break;


	case BiomeN::Savanna:
		perlinGenerator->SetOctaveCount(2);
		perlinGenerator->SetGain(2.0f);
		perlinGenerator->SetLacunarity(2.3f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.15f + WATER_LEVEL * 1.1f;
		break;


	case BiomeN::Desert:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(2.3f);
		perlinGenerator->SetLacunarity(3.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xmin, 0, TERRAIN_WIDTH, 1, 0.001f, 123456);

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.6f + WATER_LEVEL * 1.25f;
		break;


	default:
		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			ptr[i] = WATER_LEVEL + (Bm_t(biome) - 3) * 5;
		break;
	}
}


#if HEIGHT_ITPL_GAUSS
// get final interpolated height per coordinate
void TerrainGen::get_height()
{
	// calculate heights per coordinate for a list of unique biomes
	std::map<BiomeN, std::array<float, TERRAIN_WIDTH>> heightsArr;
	for (const BiomeN& biome : biomesUnq)
		get_height_for_biome(biome, (*heightsArr.emplace(biome, std::array<float, TERRAIN_WIDTH>()).first).second);
	
	if (biomesUnq.size() == 1)
		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
			heightArr[i] = std::clamp((BlkCrd)std::round(heightsArr[*biomesUnq.begin()][i]), 0, CHUNK_HEIGHT - 1);
	else
	{
		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
		{
			float height = 0;

			for (size_t j = 0; j < BIOME_ITPL_RDS; ++j)
				height += itpl_coeffs[j] * (heightsArr[biomeArr[i + j]][i] + heightsArr[biomeArr[i + 2 * BIOME_ITPL_RDS - j]][i]);
			height += itpl_coeffs[BIOME_ITPL_RDS] * heightsArr[biomeArr[i + BIOME_ITPL_RDS]][i];

			heightArr[i] = std::clamp((BlkCrd)std::round(height), 0, CHUNK_HEIGHT - 1);
		}
	}
}

#else
// get final interpolated height per coordinate
void TerrainGen::get_height()
{
	// calculate heights per coordinate for a list of unique biomes
	std::map<BiomeN, std::array<float, TERRAIN_WIDTH>> heightsArr;
	for (const BiomeN& biome : biomesUnq)
		get_height_for_biome(biome, (*heightsArr.emplace(biome, std::array<float, TERRAIN_WIDTH>()).first).second);

	if (biomesUnq.size() == 1)
		for (size_t i = 0; i < TERRAIN_WIDTH; ++i)
			heightArr[i] = std::clamp((BlkCrd)std::round(heightsArr[*biomesUnq.begin()][i]), 0, CHUNK_HEIGHT - 1);
	else
	{
		std::map<BiomeN, size_t> biomeItpl;

		for (BlkCrd i = 0; i < 2 * BIOME_ITPL_RDS; ++i)
			++biomeItpl[biomeArr[i]];

		for (BlkCrd i = 0; i < TERRAIN_WIDTH; ++i)
		{
			++biomeItpl[biomeArr[i + 2 * BIOME_ITPL_RDS]];
			--biomeItpl[biomeArr[i]];

			float height = 0;
			for (const auto& [biome, count] : biomeItpl)
				height += heightsArr[biome][i] * count;

			height /= 2 * BIOME_ITPL_RDS + 1;

			heightArr[i] = std::clamp((BlkCrd)std::round(height), 0, CHUNK_HEIGHT - 1);
		}
	}
}
#endif

// generate terrain caves
void TerrainGen::generate_caves()
{
	const float scale = 0.005f;
	const float max = 0.2f;
	std::array<float, CHUNK_BLOCKNUM> cavesNoise{};

	BlkCrd maxHeight = *std::max_element(heightArr.begin(), heightArr.end()) + 2;

	simplexGenerator->SetOctaveCount(3);
	simplexGenerator->SetGain(0.7f);
	simplexGenerator->SetLacunarity(2.0f);
	simplexGenerator->GenUniformGrid2D(cavesNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, maxHeight, scale, 123456);

	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (BlkCrd y = 0; y <= heightAtGet(x); ++y)
		{
			float val = std::fabs(cavesNoise[y * CHUNK_WIDTH + x]) - std::clamp(std::min(y * 0.05f, (heightAtGet(x) - y) * 0.05f + 0.1f), 0.0f, max);
			caves[y * CHUNK_WIDTH + x] = val < scale * 4 - max;
		}
	}
}

// fill world with stone, from Y0 to Ymax, except caves
void TerrainGen::fill_with_stone() const
{
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y <= heightAtGet(x); ++y)
			if (!caves[y * CHUNK_WIDTH + x])
				blockAtSet(y, x) = BlockN::stone;
}

// fill terrain holes with water &&  fill cave holes with lava
void TerrainGen::fill_with_fluids() const
{
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		if (heightAtGet(x) <= WATER_LEVEL)
			for (BlkCrd y = WATER_LEVEL; y > 0; --y)
				if (blockAtGet(y, x).ID == BlockN::air)
					blockAtSet(y, x) = BlockN::water;
				else
					break;
	
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y <= LAVA_LEVEL; ++y)
			if (blockAtGet(y, x).ID == BlockN::air)
				blockAtSet(y, x) = BlockN::lava;
}

// build the surface according to biome
void TerrainGen::surface_layers() const
{
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	{
		BlkCrd Yt = heightAtGet(x);
		BlkCrd Ytw = std::max(heightAtGet(x), WATER_LEVEL);

		switch (biomeAtGet(x))
		{
		case BiomeN::Polar:
			if (Yt > WATER_LEVEL)
			{
				blockAtSet(Yt, x) = BlockN::snow;
				blockAtSet(++Yt, x) = BlockN::snow;
				blockAtSet(++Yt, x) = BlockN::snow;
			}
			else
			{
				blockAtSet(Ytw, x) = BlockN::ice;
				blockAtSet(++Ytw, x) = BlockN::snow;
				blockAtSet(++Ytw, x) = BlockN::snow;
			}
			break;


		case BiomeN::Taiga:
			if (Yt >= WATER_LEVEL)
			{
					blockAtSet(Yt, x) = BlockN::dirt;
					blockAtSet(++Yt, x) = BlockN::snow;
			}
			else
			{
					blockAtSet(Yt, x) = BlockN::dirt;
			}
			break;


		case BiomeN::Mountains:
			if (Yt >= WATER_LEVEL)
			{
				if (Yt > CHUNK_HEIGHT * 1 / 2)
				{
					blockAtSet(Yt, x) = BlockN::snow;
					if (++Yt > CHUNK_HEIGHT * 2 / 3)
					{
						blockAtSet(Yt, x) = BlockN::snow;
						if (++Yt > CHUNK_HEIGHT * 3 / 4)
						{
							blockAtSet(Yt, x) = BlockN::snow;
							if (++Yt > CHUNK_HEIGHT * 4 / 5)
								blockAtSet(Yt, x) = BlockN::snow;
						}
					}
				}
			}
			break;


		case BiomeN::Ocean:
			if (Yt >= WATER_LEVEL - 3)
			{
				blockAtSet(Yt, x) = BlockN::sand;
				blockAtSet(Yt, x) = BlockN::sand;
			}
			if (Yt >= WATER_LEVEL - 4)
			{
				blockAtSet(Yt, x) = BlockN::sand;
			}
			break;


		case BiomeN::Hills:
			if (Yt >= WATER_LEVEL)
			{
				if (Yt >= WATER_LEVEL)
					blockAtSet(Yt, x) = BlockN::grass;
				else
					blockAtSet(Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			if (Yt >= WATER_LEVEL - 2)
			{
				blockAtSet(Yt, x) = BlockN::dirt;
			}
			break;


		case BiomeN::Plains:
			for (BlkCrd y = Yt; y >= (Yt + WATER_LEVEL) / 2; --y)
				blockAtSet(y, x) = BlockN::dirt;
			if (Yt >= WATER_LEVEL)
			{
				blockAtSet(Yt, x) = BlockN::grass;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			if (Yt >= WATER_LEVEL - 3)
			{
				blockAtSet(Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			break;


		case BiomeN::Forest:
			if (Yt >= WATER_LEVEL)
			{
				blockAtSet(Yt, x) = BlockN::grass;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			if (Yt >= WATER_LEVEL - 2)
			{
				blockAtSet(Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			break;


		case BiomeN::Rainforest:
			if (Yt >= WATER_LEVEL)
			{
				blockAtSet(Yt, x) = BlockN::grass;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			if (Yt >= WATER_LEVEL - 3)
			{
				blockAtSet(Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			break;


		case BiomeN::Savanna:
			if (Yt >= WATER_LEVEL)
			{
				blockAtSet(Yt, x) = BlockN::dirt;
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			if (Yt >= WATER_LEVEL - 2)
			{
				blockAtSet(--Yt, x) = BlockN::dirt;
			}
			break;


		case BiomeN::Desert:
			for (BlkCrd y = Yt; y >= (Yt + WATER_LEVEL) / 2; --y)
				blockAtSet(y, x) = BlockN::sandstone;
			if (Yt >= WATER_LEVEL)
			{
				blockAtSet(Yt, x) = BlockN::sand;
				blockAtSet(--Yt, x) = BlockN::sand;
			}
			if (Yt >= WATER_LEVEL - 2)
			{
				blockAtSet(--Yt, x) = BlockN::sand;
			}
			break;


		default:
			break;
		}
	}
}

// generate vegetation
void TerrainGen::add_vegetation() const
{
	std::array<float, TERRAIN_WIDTH> plantChanceNoise{};
	plantGenerator->GenUniformGrid2D(plantChanceNoise.data(), Xpos * CHUNK_WIDTH - PLANT_OUTGEN_RDS, 0, TERRAIN_WIDTH, 1, 1.0f, 123456);

	//std::cout << "Plants: \t";
	//for (auto pl : plantPlaceNoise)
	//	std::cout << pl << ' ';
	//std::cout << std::endl;

	for (BlkCrd x = -PLANT_OUTGEN_RDS; x < CHUNK_WIDTH + PLANT_OUTGEN_RDS; ++x)
	{
		BlkCrd i = x + PLANT_OUTGEN_RDS;
		switch (biomeAtGet(x))
		{
		case BiomeN::Desert:
			if (plantChanceNoise[i] > 0.97f)
				place_plant(PlantN::Cactoo, x);
			break;
		case BiomeN::Plains:
			if (plantChanceNoise[i] > 0.98f)
				place_plant(PlantN::Pumpkin, x);
			break;
		case BiomeN::Taiga:
			if (plantChanceNoise[i] > 0.9f)
				place_plant(PlantN::Spruce, x);
			break;
		}
	}
	std::cout << std::endl;
}

// generate plants
inline void TerrainGen::place_plant(PlantN plant, BlkCrd x) const
{
	BlkCrd Xttl = Xpos * CHUNK_WIDTH + x;
	std::cout << Xttl << ' ';
	BlkCrd Ymax = heightAtGet(x);

	switch (plant)
	{
	case PlantN::Cactoo:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);
			int height = rnd * 3 + 1;
			for (int i = 1; i <= height; ++i)
				blockAtSet(Ymax + i, x) = BlockN::cactoo;
		}
		break;


	case PlantN::Pumpkin:
		if (Ymax >= WATER_LEVEL)
			blockAtSet(Ymax + 1, x) = BlockN::pumpkin;
		break;


	case PlantN::Spruce:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);
			int height = rnd * 10 + 5;
			int radius = rnd * 5 + 2;

			for (int i = 1; i <= height + 1; ++i)
				if (i > height/5)
					for (int j = 0; j <= remap(3, height, radius, 0, i); ++j)
					{
						if (blockAtGet(Ymax + i, x + j).ID == BlockN::air)
							blockAtSet(Ymax + i, x + j) = BlockN::spruceleaves;
						if (blockAtGet(Ymax + i, x - j).ID == BlockN::air)
							blockAtSet(Ymax + i, x - j) = BlockN::spruceleaves;
					}
			
			for (int i = 1; i <= height - 2; ++i)
				blockAtSet(Ymax + i, x) = BlockN::sprucewood;

		}
		break;


	case PlantN::Accacia:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);
			int height = rnd * 5 + 4;
			int radius = rnd * 3;

			for (int i = 0; i < height - 1; ++i)
			{
				blockAtSet(Ymax + i + 1, x) = BlockN::sprucewood;

				if (i >= 2)
					for (int j = 0; j < radius * i; ++j)
					{
						blockAtSet(Ymax + i + 1, x + j + 1) = BlockN::spruceleaves;
						blockAtSet(Ymax + i + 1, x - j - 1) = BlockN::spruceleaves;
					}
			}
		}
		break;


	}
}


// overwrite anything with bedrock
void TerrainGen::protect_bedrock() const
{
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y <= 3; ++y)
			if (y == 3 && x % 2 || y == 2 && x / 2 % 2 || y == 1 && (x + 3) / 4 % 2 || y == 0)
				dataRef[y * CHUNK_WIDTH + x] = BlockN::bedrock;
}

// replace all non-air block with single type, according to the biome at coordinate
void TerrainGen::debug_color_biomes() const
{
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
			if (dataRef[y * CHUNK_WIDTH + x].ID == BlockN::water || dataRef[y * CHUNK_WIDTH + x].ID == BlockN::lava)
				dataRef[y * CHUNK_WIDTH + x] = Block(16);
			else if (dataRef[y * CHUNK_WIDTH + x].ID != BlockN::air)
				dataRef[y * CHUNK_WIDTH + x] = Block(Bm_t(biomeArr[x + BIOME_ITPL_RDS]));
}



inline constexpr       Block& TerrainGen::blockAtSet(BlkCrd y, BlkCrd x) const
{
	if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT)
		return dataRef[y * CHUNK_WIDTH + x];
	return BlNullRefSet;
}
inline constexpr const Block& TerrainGen::blockAtGet(BlkCrd y, BlkCrd x) const
{
	if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT)
		return dataRef[y * CHUNK_WIDTH + x];
	return BlNullRefGet;
}

inline constexpr const BiomeN& TerrainGen::biomeAtGet(BlkCrd x) const
{
	BlkCrd i = x + BIOME_OFFSET;
	if (i >= 0 && i < BIOME_WIDTH)
		return biomeArr[i];
	return BmNullRefGet;
}

inline constexpr const BlkCrd& TerrainGen::heightAtGet(BlkCrd x) const
{
	BlkCrd i = x + TERRAIN_OFFSET;
	if (i >= 0 && i < TERRAIN_WIDTH)
		return heightArr[i];
	return HtNullRefGet;
}

// public method to generate the chunk
void TerrainGen::generate_chunk()
{
	get_biomes();
	get_height();
	generate_caves();
	fill_with_stone();
	fill_with_fluids();
	surface_layers();
	add_vegetation();
	protect_bedrock();


#if DEBUG_COLOR_BIOMES
	debug_color_biomes();
#endif
}

constexpr std::string TerrainGen::biome_to_name(BiomeN biome)
{
	switch (biome)
	{
	case BiomeN::Polar:
		return "Polar";
	case BiomeN::Taiga:
		return "Taiga";
	case BiomeN::Mountains:
		return "Mountains";
	case BiomeN::Ocean:
		return "Ocean";
	case BiomeN::Hills:
		return "Hills";
	case BiomeN::Plains:
		return "Plains";
	case BiomeN::Forest:
		return "Forest";
	case BiomeN::Rainforest:
		return "Rainforest";
	case BiomeN::Savanna:
		return "Savanna";
	case BiomeN::Desert:
		return "Desert";
	default:
		return "Nullbiome";
	}
}