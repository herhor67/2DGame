#pragma once
#include "defines.h"

#include <typeinfo>

#include <FastNoise/FastNoise.h>

#include "TimeDiff.h"



namespace Generators
{
	using namespace FastNoise;

	//		auto t1 = std::chrono::steady_clock::now();
	//		std::cout << &generatorRef << "\t" << duration2readable(t1) << std::endl;

	static const auto& white()
	{
		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: WhiteNoise" << std::endl;
#endif
			static auto generator = New<White>();
			return generator;
		}();

		return generatorRef;
	}



	static const auto& white01()
	{
		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: WhiteNoise01" << std::endl;
#endif
			static auto generator = New<Remap>();
			generator->SetSource(New<White>());
			generator->SetRemap(-1.0f, 1.0f, 0.0f, 1.0f);
			return generator;
		}();

		return generatorRef;
	}



	static const auto& perlin()
	{
		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: PerlinNoise" << std::endl;
#endif
			static auto generator = New<Perlin>();
			return generator;
		}();

		return generatorRef;
	}



	template <float J = 1.0f, DistanceFunction DF = DistanceFunction::EuclideanSquared>
	static const auto& cellular()
	{
		static const auto& generatorRef = []()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: CellularValue, J=" << J << ", DF=" << kDistanceFunction_Strings[(int)DF] << std::endl;
#endif
			static auto generator = New<CellularValue>();
			generator->SetJitterModifier(J);
			generator->SetDistanceFunction(DF);
			return generator;
		}();
		return generatorRef;
	}

	/*
	
	template <float G = 0.5f, int O = 3, float L = 2.0f, typename T, std::enable_if_t<std::is_base_of<Generator, T>::value, bool> = true>
	static const auto& fractalBM(const SmartNode<T>& S)
	{
		static const auto& generatorRef = [&]()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: FractalBm<" << typeid(T).name() << ">, G=" << G << ", O=" << O << ", L=" << L << ", S=" << &S << std::endl;
#endif
			static auto generator = New<FractalFBm>();
			generator->SetSource(S);
			generator->SetOctaveCount(O);
			generator->SetGain(G);
			generator->SetLacunarity(L);
			return generator;
		}();
		return generatorRef;
	}
	*/

	template <typename T, float G = 0.5f, int O = 3, float L = 2.0f, std::enable_if_t<std::is_base_of<Generator, T>::value, bool> = true>
	static const auto& fractalBM()
	{
		static const auto& generatorRef = [&]()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: FractalBm<" << typeid(T).name() << ">, G=" << G << ", O=" << O << ", L=" << L << ", S=New" << std::endl;
#endif
			static auto generator = New<FractalFBm>();
			generator->SetSource(New<T>());
			generator->SetOctaveCount(O);
			generator->SetGain(G);
			generator->SetLacunarity(L);
			return generator;
		}();
		return generatorRef;
	}

	template <typename T, float G = 0.5f, int O = 3, float L = 2.0f, std::enable_if_t<std::is_base_of<Generator, T>::value, bool> = true>
	static const auto& fractalDR()
	{
		static const auto& generatorRef = [&]()
		{
#if CONSOLE_LOG_GENERATORS
			std::cout << "Generator created: FractalDR<" << typeid(T).name() << ">, G=" << G << ", O=" << O << ", L=" << L << ", S=New" << std::endl;
#endif
			static auto generator = New<FractalRidged>();
			generator->SetSource(New<T>());
			generator->SetOctaveCount(O);
			generator->SetGain(G);
			generator->SetLacunarity(L);
			return generator;
		}();
		return generatorRef;
	}


}

