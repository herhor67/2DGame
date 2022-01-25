#pragma once
#include "defines.h"

#include <FastNoise/FastNoise.h>

class StaticGenerators
{
private:
	FastNoise::SmartNode<FastNoise::CellularValue> biome;
	FastNoise::SmartNode<FastNoise::FractalFBm> perlin;
	FastNoise::SmartNode<FastNoise::FractalFBm> simplex;
	FastNoise::SmartNode<FastNoise::Remap> structure;

	StaticGenerators();

	friend class TerrainGen;
};

StaticGenerators::StaticGenerators()
{
	biome     = FastNoise::New<FastNoise::CellularValue>();
	perlin    = FastNoise::New<FastNoise::FractalFBm>();
	simplex   = FastNoise::New<FastNoise::FractalFBm>();
	structure = FastNoise::New<FastNoise::Remap>();

	biome->SetJitterModifier(0.7f);

	perlin->SetSource(FastNoise::New<FastNoise::Perlin>());
	perlin->SetOctaveCount(3);
	perlin->SetGain(0.5f);
	perlin->SetLacunarity(2.0f);

	simplex->SetSource(FastNoise::New<FastNoise::Simplex>());
	simplex->SetOctaveCount(3);
	simplex->SetGain(0.5f);
	simplex->SetLacunarity(2.0f);

	structure->SetSource(FastNoise::New<FastNoise::White>());
	structure->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);
}