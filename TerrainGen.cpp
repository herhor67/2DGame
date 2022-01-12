
#include <span>

#include "functions.h"


#include "TerrainGen.h"


std::once_flag TerrainGen::noiseInitFlag = std::once_flag();
FastNoise::SmartNode<FastNoise::CellularValue> TerrainGen::biomeGenerator   = FastNoise::New<FastNoise::CellularValue>();
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::perlinGenerator  = FastNoise::New<FastNoise::FractalFBm>();
FastNoise::SmartNode<FastNoise::FractalFBm>    TerrainGen::simplexGenerator = FastNoise::New<FastNoise::FractalFBm>();
FastNoise::SmartNode<>                         TerrainGen::caveGenerator    = FastNoise::NewFromEncodedNodeTree(R"(EwCamZk+GgABEQACAAAAAADgQBAAAACIQR8AFgABAAAACwADAAAAAgAAAAMAAAAEAAAAAAAAAD8BFAD//wAAAAAAAD8AAAAAPwAAAAA/AAAAAD8BFwAAAIC/AACAPz0KF0BSuB5AEwAAAKBABgAAj8J1PACamZk+AAAAAAAA4XoUPw==)");

TerrainGen::TerrainGen(ChkCrd _Xpos, std::array<Block, CHUNK_BLOCKNUM>& _dataRef) : Xpos(_Xpos), dataRef(_dataRef)
{
	std::call_once(noiseInitFlag, [this]()
		{
			perlinGenerator->SetSource(FastNoise::New<FastNoise::Perlin>());
			simplexGenerator->SetSource(FastNoise::New<FastNoise::Simplex>());
			biomeGenerator->SetJitterModifier(0.7f);
		});

}

TerrainGen::~TerrainGen() { }


void TerrainGen::get_biomes()
{
	std::array<float, BIOME_WIDTH> biomeNoise{};

	biomeGenerator->GenUniformGrid2D(biomeNoise.data(), Xpos * CHUNK_WIDTH - BIOME_ITPL_R, 0, BIOME_WIDTH, 1, 0.01f, 123456);
	
	for (size_t i = 0; i < BIOME_WIDTH; ++i)
	{
		Biomes biome = Biomes(std::clamp(BmT(std::fabs(biomeNoise[i]) * BmT(Biomes::MAX) - 1) + 1, 1, BmT(Biomes::MAX) - 1));

		biome = Biomes::Polar;

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

	std::array<float, CHUNK_WIDTH> heightNoise{};

	switch (biome)
	{
	case Biomes::Polar:
	{
		perlinGenerator->SetOctaveCount(3);
		perlinGenerator->SetGain(0.5f);
		perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.005f, 123456);

		for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
			ptr[i] = heightNoise[i] * WATER_LEVEL * 0.75f + WATER_LEVEL;
	}
	break;

	case Biomes::Ocean:
		{
			perlinGenerator->SetOctaveCount(3);
			perlinGenerator->SetGain(0.5f);
			perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.003f, 123456);

			for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
				ptr[i] = std::fabs(heightNoise[i]) * WATER_LEVEL * 0.2f + WATER_LEVEL * 0.25f;
		}
		break;

	case Biomes::Hills:
		{
			perlinGenerator->SetOctaveCount(3);
			perlinGenerator->SetGain(0.5f);
			perlinGenerator->GenUniformGrid2D(heightNoise.data(), Xpos * CHUNK_WIDTH, 0, CHUNK_WIDTH, 1, 0.01f, 123456);

			for (BlkCrd i = 0; i < CHUNK_WIDTH; ++i)
				ptr[i] = heightNoise[i] * WATER_LEVEL * 0.7f + WATER_LEVEL * 1.2f;
		}
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


void TerrainGen::fill_chunk()
{
	get_biomes();
	get_height_for_biomes();
	get_height();

	//for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	//	for (BlkCrd y = 0; y <= heightArr[x]; ++y)
	//		dataRef[y * CHUNK_WIDTH + x] = Block(BmT(biomeArr[x + BIOME_ITPL_R]));


	// generate terrain caves
	const float scale = 0.005f;
	const float max = 0.2f;
	std::array<bool, CHUNK_BLOCKNUM> caves{};
	std::array<float, CHUNK_BLOCKNUM> cavesNoise{};

	BlkCrd maxHeight = *std::max_element(heightArr.begin(), heightArr.end()) + 1;

	simplexGenerator->SetOctaveCount(3);
	simplexGenerator->SetGain(0.7);
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
}


/*
	const siv::PerlinNoise perlin;
	float freq = 0.01f;


	std::array<BlkCrd, CHUNK_WIDTH> Ymax{ 64 };

	// generate terrain caves
	static const float shape = 0.01f/2;
	const float max = 0.2f;
	std::array<bool, CHUNK_BLOCKNUM> caves{};
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	{
		BlkCrd xPos = x + Xpos * CHUNK_WIDTH;
		for (BlkCrd y = 0; y <= Ymax[x]; ++y)
		{
//			float perl = 1 - abs(perlin.noise2D((x + Xpos * CHUNK_WIDTH) * shape, y * shape * 2));
//			perl *= std::clamp(std::min(y * 0.2, (Ymax[x] - y) * 0.05), 0.0, 1.0);
//			caves[y * CHUNK_WIDTH + x] = perl > (1 - 0.05);
			float perl = abs(perlin.octave2D(xPos * shape, y * shape * 2, 3, 0.7));
			perl += max - std::clamp(std::min(y * 0.05f, (Ymax[x] - y) * 0.05f + 0.1f), 0.0f, max);
			caves[y * CHUNK_WIDTH + x] = perl < shape * 7;
		}
	}

	// fill world with stone, from Y0 to Ymax, except caves
	for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		for (BlkCrd y = 0; y <= Ymax[x]; ++y)
			if (!caves[y * CHUNK_WIDTH + x])
				blocks[y * CHUNK_WIDTH + x] = Block(1);



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


	// fill terrain holes with water
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
		if (Ymax[x] <= WATER_LEVEL)
			for (BlkCrd y = WATER_LEVEL; y > 0; --y)
			{
				if (blocks[y * CHUNK_WIDTH + x].ID == 0)
					blocks[y * CHUNK_WIDTH + x] = Block(8);
				else
					break;
			}

	// fill cave holes with lava
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
		for (BlkCrd y = 0; y <= LAVA_LEVEL; ++y)
		{
			if (blocks[y * CHUNK_WIDTH + x].ID == 0)
				blocks[y * CHUNK_WIDTH + x] = Block(9);
		}


	//for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	//	for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
	//		if (caves[y * CHUNK_WIDTH + x])
	//			blocks[y * CHUNK_WIDTH + x] = Block(10);




	// bedrock
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
	{
		BlkCrd xPos = x + Xpos * CHUNK_WIDTH;
		for (BlkCrd y = 0; y <= 3; ++y)
//			if (y == 0 || y == (BlkCrd)(pow((((x + Xpos * CHUNK_WIDTH)) * 0.6978), M_E)) % (BlkCrd)(pow(3, y - 1) + 1))
			if (y == 3 && x % 2 || y == 2 && x / 2 % 2 || y == 1 && (x+3) / 4 % 2 || y == 0)
				blocks[y * CHUNK_WIDTH + x] = Block(6);
	}


*/


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


	// fill terrain holes with water
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
		if (Ymax[x] <= WATER_LEVEL)
			for (BlkCrd y = WATER_LEVEL; y > 0; --y)
			{
				if (blocks[y * CHUNK_WIDTH + x].ID == 0)
					blocks[y * CHUNK_WIDTH + x] = Block(8);
				else
					break;
			}

	// fill cave holes with lava
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
		for (BlkCrd y = 0; y <= LAVA_LEVEL; ++y)
		{
			if (blocks[y * CHUNK_WIDTH + x].ID == 0)
				blocks[y * CHUNK_WIDTH + x] = Block(9);
		}


	//for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
	//	for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
	//		if (caves[y * CHUNK_WIDTH + x])
	//			blocks[y * CHUNK_WIDTH + x] = Block(10);




	// bedrock
	for (BlkCrd x = 0; x <= CHUNK_WIDTH - 1; ++x)
	{
		BlkCrd xPos = x + Xpos * CHUNK_WIDTH;
		for (BlkCrd y = 0; y <= 3; ++y)
			//			if (y == 0 || y == (BlkCrd)(pow((((x + Xpos * CHUNK_WIDTH)) * 0.6978), M_E)) % (BlkCrd)(pow(3, y - 1) + 1))
			if (y == 3 && x % 2 || y == 2 && x / 2 % 2 || y == 1 && (x + 3) / 4 % 2 || y == 0)
				blocks[y * CHUNK_WIDTH + x] = Block(6);
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