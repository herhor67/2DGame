#pragma once
#include "defines.h"

#include <FastNoise/FastNoise.h>

#include "TimeDiff.h"

class Generators
{
private:
	FastNoise::SmartNode<FastNoise::CellularValue> biome;
	FastNoise::SmartNode<FastNoise::FractalFBm> perlin;
	FastNoise::SmartNode<FastNoise::FractalFBm> simplex;
	FastNoise::SmartNode<FastNoise::Remap> strctr;
	FastNoise::SmartNode<FastNoise::Remap> rand;

	Generators(const Generators&) = delete;
	void operator=(const Generators&) = delete;

	Generators();
	~Generators();

	template <float J = 0.0f, FastNoise::DistanceFunction DF = FastNoise::DistanceFunction::EuclideanSquared>
	static const auto& cellularRemap()
	{
//		auto t1 = std::chrono::steady_clock::now();

		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: CellularValue, J=" << J << ", DF=" << FastNoise::kDistanceFunction_Strings[(int)DF] << std::endl;
#endif
			static auto generator = FastNoise::New<FastNoise::CellularValue>();
			generator->SetJitterModifier(J);
			generator->SetDistanceFunction(DF);
			return generator;
		}();

//		std::cout << &generatorRef << "\t" << duration2readable(t1) << std::endl;

		return generatorRef;
	}


	template <float G = 0.5f, int O = 3, float L = 2.0f>
	static const auto& perlinFractal()
	{
		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: Fractal<Perlin>, G=" << G << ", O=" << O << ", L=" << L << std::endl;
#endif
			static auto generator = FastNoise::New<FastNoise::FractalFBm>();
			generator->SetSource(FastNoise::New<FastNoise::Perlin>());
			generator->SetOctaveCount(O);
			generator->SetGain(G);
			generator->SetLacunarity(L);
			return generator;
		}();
		return generatorRef;
	}


	template <float G = 0.5f, int O = 3, float L = 2.0f>
	static const auto& simplexFractal()
	{
		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: Fractal<Simplex>, G=" << G << ", O=" << O << ", L=" << L << std::endl;
#endif
			static auto generator = FastNoise::New<FastNoise::FractalFBm>();
			generator->SetSource(FastNoise::New<FastNoise::Simplex>());
			generator->SetOctaveCount(O);
			generator->SetGain(G);
			generator->SetLacunarity(L);
			return generator;
		}();
		return generatorRef;
	}


	static const Generators& Get();

	friend class TerrainGen;
};

