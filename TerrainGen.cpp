
#include <span>

#include "functions.h"


#include "TerrainGen.h"


std::once_flag TerrainGen::noiseInitFlag = std::once_flag();
FastNoise::SmartNode<FastNoise::CellularValue> TerrainGen::biomeGenerator   = FastNoise::New<FastNoise::CellularValue>();
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::perlinGenerator  = FastNoise::New<FastNoise::FractalFBm>();
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::simplexGenerator = FastNoise::New<FastNoise::FractalFBm>();


TerrainGen::TerrainGen(ChkCrd _Xpos, std::array<Block, CHUNK_BLOCKNUM>& _dataRef) : Xpos(_Xpos), dataRef(_dataRef)
{
	std::call_once(noiseInitFlag, []()
		{
			perlinGenerator->SetSource(FastNoise::New<FastNoise::Perlin>());
			perlinGenerator->SetOctaveCount(3);
			perlinGenerator->SetGain(0.5f);
			perlinGenerator->SetLacunarity(2.0f);

			simplexGenerator->SetSource(FastNoise::New<FastNoise::Simplex>());
			simplexGenerator->SetOctaveCount(3);
			simplexGenerator->SetGain(0.5f);
			simplexGenerator->SetLacunarity(2.0f);

			biomeGenerator->SetJitterModifier(0.7f);
		});

}

TerrainGen::~TerrainGen() { }


void TerrainGen::get_biomes()
{
	std::array<float, BIOME_WIDTH> biomeNoise{};

	biomeGenerator->GenUniformGrid2D(biomeNoise.data(), Xpos * CHUNK_WIDTH - BIOME_ITPL_RDS, 0, BIOME_WIDTH, 1, 0.003f, 123455);
	
	for (size_t i = 0; i < BIOME_WIDTH; ++i)
	{
		Biomes biome = Biomes(std::clamp(BmT(std::fabs(biomeNoise[i]) * BmT(Biomes::MAX) - 1) + 1, 1, BmT(Biomes::MAX) - 1));

//		biome = Biomes::Taiga;

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


void TerrainGen::get_height_for_biome(Biomes biome)
{
	auto resp = heightsArr.emplace(biome, std::array<float, CHUNK_WIDTH>{});
	auto& ptr = (*resp.first).second;

	//for (size_t i = 0; i < CHUNK_WIDTH; ++i)
	//	ptr[i] = WATER_LEVEL + (BmT(biome) - 3) * 5;
	//return;

	std::array<float, CHUNK_WIDTH> heightNoise{};

	switch (biome)
	{
	case Biomes::Polar:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->SetLacunarity(2.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.005f, 123456);
			
		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.75f + WATER_LEVEL;
		break;
		
		
	case Biomes::Taiga:
		perlinGenerator->SetOctaveCount(2);
		perlinGenerator->SetGain(2.0f);
		perlinGenerator->SetLacunarity(2.5f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.15f + WATER_LEVEL * 1.02f;
		break;
		
		
	case Biomes::Mountains:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.6f);
		perlinGenerator->SetLacunarity(3.14159f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.003f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * CHUNK_HEIGHT * 0.8f + CHUNK_HEIGHT * 0.5f;
		break;
		
		
	case Biomes::Ocean:
		perlinGenerator->SetOctaveCount(5);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->SetLacunarity(3.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.001f, 123456);
			
		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = std::fabs(heightNoise[i]) * WATER_LEVEL * 2.0f + WATER_LEVEL * 0.2f;
		break;


	case Biomes::Hills:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->SetLacunarity(2.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.01f, 123456);
			
		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.9f + WATER_LEVEL * 1.3f;
		break;


	case Biomes::Plains:
		perlinGenerator->SetOctaveCount(2);
		perlinGenerator->SetGain(2.0f);
		perlinGenerator->SetLacunarity(2.5f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.25f + WATER_LEVEL * 1.07f;
		break;


	case Biomes::Forest:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.6f);
		perlinGenerator->SetLacunarity(2.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.4f + WATER_LEVEL * 1.2f;
		break;


	case Biomes::Rainforest:
		perlinGenerator->SetOctaveCount(4);
		perlinGenerator->SetGain(0.7f);
		perlinGenerator->SetLacunarity(1.8f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.008f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.8f + WATER_LEVEL * 1.15f;
		break;


	case Biomes::Savanna:
		perlinGenerator->SetOctaveCount(2);
		perlinGenerator->SetGain(2.0f);
		perlinGenerator->SetLacunarity(2.3f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.01f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.15f + WATER_LEVEL * 1.1f;
		break;


	case Biomes::Desert:
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(2.3f);
		perlinGenerator->SetLacunarity(3.0f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.001f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.6f + WATER_LEVEL * 1.25f;
		break;


	default:
		for (size_t i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = WATER_LEVEL + (BmT(biome)-3) * 5;
		break;
	}
}


void TerrainGen::get_height_for_biomes()
{
	for (const Biomes& biome : biomesUnq)
		get_height_for_biome(biome);
}

void TerrainGen::get_height()
{
	if (biomesUnq.size() == 1)
		for (size_t i = 0; i < CHUNK_WIDTH; ++i)
			heightArr[i] = std::clamp((BlkCrd)std::round(heightsArr[*biomesUnq.begin()][i]), 0, CHUNK_HEIGHT - 1);
	else
	{
		for (size_t i = 0; i < CHUNK_WIDTH; ++i)
		{
			float height = 0;
			
			for (size_t j = 0; j < BIOME_ITPL_RDS; ++j)
				height += itpl_coeffs[j] * (heightsArr[biomeArr[i + j]][i] + heightsArr[biomeArr[i + BIOME_ITPL_RNG - 1 - j]][i]);

			height += itpl_coeffs[BIOME_ITPL_RDS] * heightsArr[biomeArr[i + BIOME_ITPL_RDS]][i];

//			for (size_t j = 0; j < BIOME_ITPL_RNG; ++j)
//				height += itpl_coeffs[j] * heightsArr[biomeArr[i + j]][i];

			heightArr[i] = std::clamp((BlkCrd)std::round(height), 0, CHUNK_HEIGHT - 1);
		}
	}
}

/*/
void TerrainGen::get_height()
{
	if (biomesUnq.size() == 1)
		for (size_t i = 0; i < CHUNK_WIDTH; ++i)
			heightArr[i] = std::clamp((BlkCrd)std::round(heightsArr[*biomesUnq.begin()][i]), 0, CHUNK_HEIGHT - 1);
	else
	{
		std::map<Biomes, size_t> biomeItpl;

		for (size_t i = 0; i < BIOME_ITPL_R * 2; ++i)
			++biomeItpl[biomeArr[i]];

		for (size_t i = 0; i < CHUNK_WIDTH; ++i)
		{
			++biomeItpl[biomeArr[i + BIOME_ITPL_R * 2]];
			--biomeItpl[biomeArr[i]];

			float height = 0;
			for (const auto& [biome, count] : biomeItpl)
				height += heightsArr[biome][i] * count;

			height /= BIOME_ITPL_R * 2 + 1;

			heightArr[i] = std::clamp((BlkCrd)std::round(height), 0, CHUNK_HEIGHT - 1);
		}
	}
}
//*/

void TerrainGen::fill_chunk()
{
	get_biomes();
	get_height_for_biomes();
	get_height();



	// generate terrain caves
	const float scale = 0.005f;
	const float max = 0.2f;
	std::array<bool, CHUNK_BLOCKNUM> caves{};
	std::array<float, CHUNK_BLOCKNUM> cavesNoise{};

	BlkCrd maxHeight = *std::max_element(heightArr.begin(), heightArr.end()) + 1;

	simplexGenerator->SetOctaveCount(3);
	simplexGenerator->SetGain(0.7f);
	simplexGenerator->SetLacunarity(2.0f);
	simplexGenerator->GenUniformGrid2D(cavesNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, maxHeight, scale, 123456);

	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	{
		BlkCrd xPos = x + Xpos * CHUNK_WIDTH;
		for (BlkCrd y = 0; y <= heightArr[x]; ++y)
		{
			float val = std::fabs(cavesNoise[y * CHUNK_WIDTH + x]) - std::clamp(std::min(y * 0.05f, (heightArr[x] - y) * 0.05f + 0.1f), 0.0f, max);
			caves[y * CHUNK_WIDTH + x] = val < scale * 4 - max;
		}
	}

	// fill world with stone, from Y0 to Ymax, except caves
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y <= heightArr[x]; ++y)
			if (!caves[y * CHUNK_WIDTH + x])
				dataRef[y * CHUNK_WIDTH + x] = Block(1);


	// fill terrain holes with water
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
		if (heightArr[x] <= WATER_LEVEL)
			for (BlkCrd y = WATER_LEVEL; y > 0; --y)
			{
				if (dataRef[y * CHUNK_WIDTH + x].ID == 0)
					dataRef[y * CHUNK_WIDTH + x] = Block(8);
				else
					break;
			}
	
	// fill cave holes with lava
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
		for (BlkCrd y = 0; y <= LAVA_LEVEL; ++y)
		{
			if (dataRef[y * CHUNK_WIDTH + x].ID == 0)
				dataRef[y * CHUNK_WIDTH + x] = Block(9);
		}





	// bedrock
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y <= 3; ++y)
			//			if (y == 0 || y == (BlkCrd)(pow((((x + Xpos * CHUNK_WIDTH)) * 0.6978), M_E)) % (BlkCrd)(pow(3, y - 1) + 1))
			if (y == 3 && x % 2 || y == 2 && x / 2 % 2 || y == 1 && (x + 3) / 4 % 2 || y == 0)
				dataRef[y * CHUNK_WIDTH + x] = Block(6);


#if DEBUG_COLOR_BIOMES
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
			if (dataRef[y * CHUNK_WIDTH + x].ID == 8 || dataRef[y * CHUNK_WIDTH + x].ID == 9)
				dataRef[y * CHUNK_WIDTH + x] = Block(16);
			else if (dataRef[y * CHUNK_WIDTH + x].ID != 0)
				dataRef[y * CHUNK_WIDTH + x] = Block(BmT(biomeArr[x + BIOME_ITPL_RDS]));
#endif
}

/*
void Chunk::perlinFill()
{

	// replace top with grass and dirt
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	{
		if (Ymax[x] >= WATER_LEVEL && blocks[(Ymax[x]) * CHUNK_WIDTH + x].ID != 0)
			blocks[Ymax[x] * CHUNK_WIDTH + x] = Block(3);
		if (Ymax[x] - 1 >= WATER_LEVEL && blocks[(Ymax[x] - 1) * CHUNK_WIDTH + x].ID != 0)
			blocks[(Ymax[x] - 1) * CHUNK_WIDTH + x] = Block(2);
		if (Ymax[x] - 2 >= WATER_LEVEL && blocks[(Ymax[x] - 2) * CHUNK_WIDTH + x].ID != 0)
			blocks[(Ymax[x] - 2) * CHUNK_WIDTH + x] = Block(2);
		if (Ymax[x] - 3 >= WATER_LEVEL && blocks[(Ymax[x] - 3) * CHUNK_WIDTH + x].ID != 0)
			blocks[(Ymax[x] - 3) * CHUNK_WIDTH + x] = Block(2);
	}

}
//*/




std::string TerrainGen::biome_to_name(Biomes biome)
{
	switch (biome)
	{
	case Biomes::Polar:
		return "Polar";
	case Biomes::Taiga:
		return "Taiga";
	case Biomes::Mountains:
		return "Mountains";
	case Biomes::Ocean:
		return "Ocean";
	case Biomes::Hills:
		return "Hills";
	case Biomes::Plains:
		return "Plains";
	case Biomes::Forest:
		return "Forest";
	case Biomes::Rainforest:
		return "Rainforest";
	case Biomes::Savanna:
		return "Savanna";
	case Biomes::Desert:
		return "Desert";
	default:
		return "Nullbiome";
	}
}