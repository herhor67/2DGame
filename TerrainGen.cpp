#include "TerrainGen.h"

#include <span>

#include "functions.h"




std::once_flag TerrainGen::noiseInitFlag = std::once_flag();
//FastNoise::SmartNode<FastNoise::CellularValue> TerrainGen::biomeGenerator   = FastNoise::New<FastNoise::CellularValue>();
//FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::perlinGenerator  = FastNoise::New<FastNoise::FractalFBm>();
//FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::simplexGenerator = FastNoise::New<FastNoise::FractalFBm>();
//FastNoise::SmartNode<FastNoise::Remap>         TerrainGen::plantGenerator   = FastNoise::New<FastNoise::Remap>();

FastNoise::SmartNode<FastNoise::CellularValue> TerrainGen::biomeGenerator;
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::perlinGenerator;
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::simplexGenerator;
FastNoise::SmartNode<FastNoise::Remap>         TerrainGen::plantGenerator;

      Block  TerrainGen::BlNullRefSet = BlockN::MAX;
const Block  TerrainGen::BlNullRefGet = BlockN::MAX;
const BiomeN TerrainGen::BmNullRefGet = BiomeN::MIN;
const BlkCrd TerrainGen::HtNullRefGet = INT_MIN;


TerrainGen::TerrainGen(ChkCrd _Xpos, std::array<Block, CHUNK_BLOCKNUM>& _dataRef) : Xpos(_Xpos), dataRef(_dataRef)
{
	std::call_once(noiseInitFlag, []()
		{
			biomeGenerator = FastNoise::New<FastNoise::CellularValue>();
			perlinGenerator = FastNoise::New<FastNoise::FractalFBm>();
			simplexGenerator = FastNoise::New<FastNoise::FractalFBm>();
			plantGenerator = FastNoise::New<FastNoise::Remap>();

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
		//float value = std::fabs(biomeNoise[i]);
		float value = biomeNoise[i] * 0.5f + 0.5f;

		Bm_t choosen = remap01_dsc(Bm_t(BiomeN::MIN) + 1, Bm_t(BiomeN::MAX) - 1, value);
		BiomeN biome = BiomeN(choosen);

//		biome = BiomeN::Polar;

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


	case BiomeN::Redwoodforest:
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


		case BiomeN::Redwoodforest:
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
				blockAtSet(Yt, x) = BlockN::drygrass;
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
			if (Yt >= WATER_LEVEL)
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
void TerrainGen::add_structures() const
{
	std::array<float, STRUCTURE_WIDTH> plantChanceNoise{};
	plantGenerator->GenUniformGrid2D(plantChanceNoise.data(), Xpos * CHUNK_WIDTH - STRUCTURE_OUT_RDS, 0, STRUCTURE_WIDTH, 1, 1.0f, 123456);

	for (BlkCrd x = -STRUCTURE_OUT_RDS; x < CHUNK_WIDTH + STRUCTURE_OUT_RDS; ++x)
	{
		BlkCrd i = x + STRUCTURE_OUT_RDS;
		switch (biomeAtGet(x))
		{
		case BiomeN::Polar:
			if (plantChanceNoise[i] > 0.999f)
				generate_structure(StrctrN::Igloo, x);
			break;
		case BiomeN::Taiga:
			if (plantChanceNoise[i] > 0.9f)
				generate_structure(StrctrN::Spruce, x);
			break;
		case BiomeN::Mountains:
			if (plantChanceNoise[i] > 0.995f)
				generate_structure(StrctrN::Spruce, x);
			break;
		case BiomeN::Ocean:
			if (plantChanceNoise[i] > 0.96f)
				generate_structure(StrctrN::Coral, x);
			break;
		case BiomeN::Hills:
			if (plantChanceNoise[i] > 0.99f)
				generate_structure(StrctrN::Oak, x);
			break;
		case BiomeN::Plains:
			if (plantChanceNoise[i] > 0.97f)
				generate_structure(StrctrN::Pumpkin, x);
			break;
		case BiomeN::Forest:
			if (plantChanceNoise[i] > 0.9f)
				generate_structure(StrctrN::Oak, x);
			break;
		case BiomeN::Redwoodforest:
			if (plantChanceNoise[i] > 0.95f)
				generate_structure(StrctrN::Redwood, x);
			break;
		case BiomeN::Savanna:
			if (plantChanceNoise[i] > 0.99f)
				generate_structure(StrctrN::Accacia, x);
			break;
		case BiomeN::Desert:
			if (plantChanceNoise[i] > 0.97f)
				generate_structure(StrctrN::Cactoo, x);
			break;
		}
	}
#if CONSOLE_LOG_GENERATION
	std::cout << std::endl;
#endif
}

// generate plants
inline void TerrainGen::generate_structure(StrctrN structure, BlkCrd x) const
{
	BlkCrd Xttl = Xpos * CHUNK_WIDTH + x;
#if CONSOLE_LOG_GENERATION
	std::cout << Xttl << ' ';
#endif
	BlkCrd Ymax = heightAtGet(x);

	switch (structure)
	{
	case StrctrN::Igloo:
		if (true) // just for variable space, always true
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);

			int side = int(rnd * 4) % 4;
			
			std::array<BlkCrd, 5> heights;
			for (BlkCrd i = 0; i <= 4; ++i)
				heights[i] = std::max(heightAtGet(x + i - 2), WATER_LEVEL) + 2;

			Ymax = median(heights);

			for (BlkCrd i = 2; i >= heights[0] - Ymax; --i)
				if (blockAtGet(Ymax + i, x - 2).ID == BlockN::air)
					blockAtSet(Ymax + i, x - 2) = (i > 0) ? BlockN::ice : BlockN::snow;

			for (BlkCrd i = 3; i >= heights[1] - Ymax; --i)
				if (blockAtGet(Ymax + i, x - 1).ID == BlockN::air)
					blockAtSet(Ymax + i, x - 1) = (i > 0) ? BlockN::ice : BlockN::snow;

			for (BlkCrd i = 3; i >= heights[2] - Ymax; --i)
				if (blockAtGet(Ymax + i, x).ID == BlockN::air)
					blockAtSet(Ymax + i, x + 0) = (i > 0) ? BlockN::ice : BlockN::snow;

			for (BlkCrd i = 3; i >= heights[3] - Ymax; --i)
				if (blockAtGet(Ymax + i, x + 1).ID == BlockN::air)
					blockAtSet(Ymax + i, x + 1) = (i > 0) ? BlockN::ice : BlockN::snow;

			for (BlkCrd i = 2; i >= heights[4] - Ymax; --i)
				if (blockAtGet(Ymax + i, x + 2).ID == BlockN::air)
					blockAtSet(Ymax + i, x + 2) = (i > 0) ? BlockN::ice : BlockN::snow;
			
			if (side == 0)
			{
				blockAtSet(Ymax + 1, x) = BlockN::air;
				blockAtSet(Ymax + 2, x) = BlockN::air;
			}
			if (side == 1)
			{
				if (blockAtGet(Ymax + 1, x + 3).ID == BlockN::air)
					blockAtSet(Ymax + 1, x + 3) = BlockN::ice;
				if (blockAtGet(Ymax + 2, x + 3).ID == BlockN::air)
					blockAtSet(Ymax + 2, x + 3) = BlockN::ice;
			}
			if (side == 2)
			{
				if (blockAtGet(Ymax + 1, x - 3).ID == BlockN::air)
					blockAtSet(Ymax + 1, x - 3) = BlockN::ice;
				if (blockAtGet(Ymax + 2, x - 3).ID == BlockN::air)
					blockAtSet(Ymax + 2, x - 3) = BlockN::ice;
			}
		}
		break;


	case StrctrN::Cactoo:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);

			int height = rnd * 3 + 1;
			for (int i = 1; i <= height; ++i)
				blockAtSet(Ymax + i, x) = BlockN::cactoo;
		}
		break;


	case StrctrN::Pumpkin:
		if (Ymax >= WATER_LEVEL)
			blockAtSet(Ymax + 1, x) = BlockN::pumpkin;
		break;


	case StrctrN::Spruce:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);

			int height = rnd * 10 + 5;
			int radius = rnd * 4 + 1;

			for (int i = 1; i <= height - 2; ++i)
				blockAtSet(Ymax + i, x) = BlockN::sprucewood;

			for (int i = 1; i <= height + 1; ++i)
				if (i > height/5)
					for (int j = 0; j <= remap(3, height, radius, 0, i); ++j)
					{
						if (blockAtGet(Ymax + i, x + j).ID == BlockN::air)
							blockAtSet(Ymax + i, x + j) = BlockN::spruceleaves;
						if (blockAtGet(Ymax + i, x - j).ID == BlockN::air)
							blockAtSet(Ymax + i, x - j) = BlockN::spruceleaves;
					}
		}
		break;


	case StrctrN::Oak:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd1 = plantGenerator->GenSingle2D(Xttl, 1, 123456);
			float rnd2 = plantGenerator->GenSingle2D(Xttl, 2, 123456);
			float rnd3 = plantGenerator->GenSingle2D(Xttl, 3, 123456);

			int height = rnd1 * 10 + 5;
			int branches = rnd1 * 5;
			int side = (rnd2 > 0.5f) ? 1 : -1;
			bool orient = rnd3 > 0.5f;

			for (int i = 1; i <= height; ++i)
				blockAtSet(Ymax + i, x) = BlockN::oakwood;

			int num = 0;
			int den = 1;
			for (int j = 1; j <= branches; ++j)
			{
				num = 2 * num + den;
				den *= 3;
				side *= -1;
				for (int i = 1; i <= height / (j + 1); ++i)
				{
					BlkCrd y = Ymax + height * num / den + 1 + remap(0, height / (j + 1), 0, std::max(1, height / (j + 4)), i);
//					if (blockAtGet(y, x + side * i).ID == BlockN::air)
						blockAtSet(y, x + side * i) = BlockN::oakwood;
				}
			}

			for (int i = -height / 2; i <= height / 2; ++i)
				for (int j = -height * 3 / 5; j <= height * 3 / 5; ++j)
					if (orient && j * j / 4 + i * i * 4 / 9 <= height * height / 9 || !orient && i * i / 4 + j * j * 4 / 9 <= height * height / 9)
						if (blockAtGet(Ymax + i + height * 2 / 3 + 1, x + j).ID == BlockN::air)
							blockAtSet(Ymax + i + height * 2 / 3 + 1, x + j) = BlockN::oakleaves;
		}
		break;


	case StrctrN::Accacia:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd1 = plantGenerator->GenSingle2D(Xttl, 1, 123456);
			float rnd2 = plantGenerator->GenSingle2D(Xttl, 2, 123456);

			int height = rnd1 * 7 + 5;
			int offset = (rnd2 * 2 - 1) * 4 * (rnd1 + 1);

			for (int i = 1; i <= height; ++i)
				blockAtSet(Ymax + i, x + remap(1, height, 0, offset, i)) = BlockN::accaciawood;

			for (int i = height * 5 / 6; i <= height * 6 / 5; ++i)
				for (int j = 0; j <= std::min(remap(height * 5 / 6, height, 2, height / 2, i), remap(height, height * 6 / 5, height / 2, 2, i)); ++j)
				{
					if (blockAtGet(Ymax + i, x + offset + j).ID == BlockN::air)
						blockAtSet(Ymax + i, x + offset + j) = BlockN::accacialeaves;
					if (blockAtGet(Ymax + i, x + offset - j).ID == BlockN::air)
						blockAtSet(Ymax + i, x + offset - j) = BlockN::accacialeaves;
				}
		}
		break;


	case StrctrN::Redwood:
		if (Ymax >= WATER_LEVEL)
		{
			float rnd = plantGenerator->GenSingle2D(Xttl, 1, 123456);

			int height = rnd * 15 + 20;
			int lvsw = rnd * 3 + 2;
			int trnkw = rnd * 3 + 1;

			if (trnkw >= 1)
				for (int j = 0; j <= height - 1; ++j)
					blockAtSet(Ymax + j, x) = BlockN::redwood;
			if (trnkw >= 2)
				for (int j = 0; j <= height * 4 / 5; ++j)
					blockAtSet(Ymax + j, x - 1) = BlockN::redwood;
			if (trnkw >= 3)
				for (int j = 0; j <= height / 2; ++j)
					blockAtSet(Ymax + j, x + 1) = BlockN::redwood;

			for (int i = height * 2 / 6 - 2; i <= height + 2; ++i)
				for (int j = 0; j <= std::min(std::min(remap(height *2/6 - 3, height * 2 / 6 +2, 0, lvsw, i), remap(height + 3, height*3/5, 0, lvsw, i)), lvsw); ++j)
				{
					if (blockAtGet(Ymax + i, x + j).ID == BlockN::air)
						blockAtSet(Ymax + i, x + j) = BlockN::redleaves;
					if (blockAtGet(Ymax + i, x - j).ID == BlockN::air)
						blockAtSet(Ymax + i, x - j) = BlockN::redleaves;
				}
		}
		break;


	case StrctrN::Coral:
		if (Ymax <= WATER_LEVEL - 5)
		{
			float rnd1 = plantGenerator->GenSingle2D(Xttl, 1, 123456);
			float rnd2 = plantGenerator->GenSingle2D(Xttl, 2, 123456);
			float rnd3 = plantGenerator->GenSingle2D(Xttl, 3, 123456);
			float rnd4 = plantGenerator->GenSingle2D(Xttl, 4, 123456);

			int height = rnd1 * 10 + 3;
			int branches = (rnd1 * 0.5f + 0.5f) * 7 * (rnd2 * 0.6f + 0.4f);
			int side = (rnd3 > 0.5f) ? 1 : -1;
			BlockN color = (BlockN)remap01_dsc(Bl_t(BlockN::redcoral), Bl_t(BlockN::bluecoral), rnd4);

			for (int i = 1; i <= height; ++i)
				if (blockAtGet(Ymax + i, x).ID == BlockN::water)
					blockAtSet(Ymax + i, x) = color;

			int num = 0;
			int den = 1;
			for (int j = 1; j <= branches; ++j)
			{
				num = 3 * num + den;
				den *= 4;
				side = -side;
				for (int i = 1; i <= height / (j + 1); ++i)
				{
					BlkCrd y = Ymax + height * num / den + 1 + remap(0, height / (j + 1), 0, std::max(1, height / (j + 4)), i);
					if (blockAtGet(y, x + side * i).ID == BlockN::water)
						blockAtSet(y, x + side * i) = color;
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
				blockAtSet(y, x) = BlockN::bedrock;
}

// replace all non-air block with single type, according to the biome at coordinate
void TerrainGen::debug_color_biomes() const
{
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
			if (blockAtGet(y, x).ID == BlockN::water || blockAtGet(y, x).ID == BlockN::lava)
				blockAtSet(y, x) = Block(16);
			else if (blockAtGet(y, x).ID != BlockN::air)
				blockAtSet(y, x) = Block(Bm_t(biomeAtGet(x)));
}



inline constexpr       Block& TerrainGen::blockAtSet(BlkCrd y, BlkCrd x) const
{
	if ((size_t)x < CHUNK_WIDTH && (size_t)y < CHUNK_HEIGHT)
		return dataRef[y * CHUNK_WIDTH + x];
	return BlNullRefSet;
}
inline constexpr const Block& TerrainGen::blockAtGet(BlkCrd y, BlkCrd x) const
{
	if ((size_t)x < CHUNK_WIDTH && (size_t)y < CHUNK_HEIGHT)
		return dataRef[y * CHUNK_WIDTH + x];
	return BlNullRefGet;
}

inline constexpr const BiomeN& TerrainGen::biomeAtGet(BlkCrd x) const
{
	size_t i = x + BIOME_OFFSET;
	if (i < BIOME_WIDTH)
		return biomeArr[i];
	return BmNullRefGet;
}

inline constexpr const BlkCrd& TerrainGen::heightAtGet(BlkCrd x) const
{
	size_t i = x + TERRAIN_OFFSET;
	if (i < TERRAIN_WIDTH)
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
	add_structures();
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
	case BiomeN::Redwoodforest:
		return "Rainforest";
	case BiomeN::Savanna:
		return "Savanna";
	case BiomeN::Desert:
		return "Desert";
	default:
		return "Nullbiome";
	}
}