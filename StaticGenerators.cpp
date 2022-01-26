#

#include "StaticGenerators.h"


Generators::Generators()
{
	biome   = FastNoise::New<FastNoise::CellularValue>();
	perlin  = FastNoise::New<FastNoise::FractalFBm>();
	simplex = FastNoise::New<FastNoise::FractalFBm>();
	strctr  = FastNoise::New<FastNoise::Remap>();
	rand    = FastNoise::New<FastNoise::Remap>();

	biome->SetJitterModifier(0.7f);

	perlin->SetSource(FastNoise::New<FastNoise::Perlin>());
	perlin->SetOctaveCount(3);
	perlin->SetGain(0.5f);
	perlin->SetLacunarity(2.0f);

	simplex->SetSource(FastNoise::New<FastNoise::Simplex>());
	simplex->SetOctaveCount(3);
	simplex->SetGain(0.5f);
	simplex->SetLacunarity(2.0f);

	strctr->SetSource(FastNoise::New<FastNoise::White>());
	strctr->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);

	rand->SetSource(FastNoise::New<FastNoise::White>());
	rand->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);
}


Generators::~Generators() {}


const Generators& Generators::Get()
{
	static const Generators instance;
	return instance;
}