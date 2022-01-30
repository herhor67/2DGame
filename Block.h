#pragma once
#include "defines.h"

#include <array>
#include <type_traits>

#include "Environment.h"
#include "GLhelpers.h"


enum class BlockN : uint32_t { air = 0, stone, dirt, grass, drygrass, bedrock, water, lava, sand, sandstone, snow, ice, cactoo, pumpkin, sprucewood, spruceleaves, oakwood, oakleaves, acaciawood, acacialeaves, redwood, redwleaves, redcoral, greencoral, yellowcoral, bluecoral, MAX };
typedef std::underlying_type_t<BlockN> Bl_t;

namespace Block
{

	constexpr auto ColorsChr = []()
	{
		std::array<Color<uint8_t>, static_cast<Bl_t>(BlockN::MAX) + 1> blocksColors{};

		blocksColors[Bl_t(BlockN::air)]				= {   0,   0,   0,   3 };
		blocksColors[Bl_t(BlockN::stone)]			= { 110, 114, 117, 255 };
		blocksColors[Bl_t(BlockN::dirt)]			= { 135,  97,  74, 255 };
		blocksColors[Bl_t(BlockN::grass)]			= {  82, 152,   0, 255 };
		blocksColors[Bl_t(BlockN::drygrass)]		= { 183, 127,  52, 255 };
		blocksColors[Bl_t(BlockN::bedrock)]			= {  60,  62,  63, 255 };
		blocksColors[Bl_t(BlockN::water)]			= {  44, 122, 168, 230 };
		blocksColors[Bl_t(BlockN::lava)]			= { 255,  51,  10, 230 };
		blocksColors[Bl_t(BlockN::sand)]			= { 232, 217, 196, 255 };
		blocksColors[Bl_t(BlockN::sandstone)]		= { 182, 149, 101, 255 };
		blocksColors[Bl_t(BlockN::snow)]			= { 255, 255, 255, 255 };
		blocksColors[Bl_t(BlockN::ice)]				= { 170, 201, 255, 204 };
		blocksColors[Bl_t(BlockN::cactoo)]			= {  47,  77,  19, 255 };
		blocksColors[Bl_t(BlockN::pumpkin)]			= { 228,  83,  18, 255 };
		blocksColors[Bl_t(BlockN::sprucewood)]		= {  41,  18,   0, 255 };
		blocksColors[Bl_t(BlockN::spruceleaves)]	= {   9,  19,  10, 230 };
		blocksColors[Bl_t(BlockN::oakwood)]			= {  91,  36,   0, 255 };
		blocksColors[Bl_t(BlockN::oakleaves)]		= {  25,  78,   0, 230 };
		blocksColors[Bl_t(BlockN::acaciawood)]		= { 180, 119,  73, 255 };
		blocksColors[Bl_t(BlockN::acacialeaves)]	= { 121, 106,  48, 230 };
		blocksColors[Bl_t(BlockN::redwood)]			= {  91,  27,   8, 255 };
		blocksColors[Bl_t(BlockN::redwleaves)]		= {  26,  48,  13, 230 };
		blocksColors[Bl_t(BlockN::redcoral)]		= { 241,  30,  53, 255 };
		blocksColors[Bl_t(BlockN::greencoral)]		= {  61, 187,  44, 255 };
		blocksColors[Bl_t(BlockN::yellowcoral)]		= { 207, 177,   3, 255 };
		blocksColors[Bl_t(BlockN::bluecoral)]		= {   3,  13,  98, 255 };


		blocksColors[Bl_t(BlockN::MAX)]				= { 255,   0, 220, 255 };

#if DEBUG_COLOR_BIOMES
		blocksColors[1]		= {   0,   0,   0, 255 };
		blocksColors[2]		= {   0,   0, 128, 255 };
		blocksColors[3]		= {   0, 128,   0, 255 };
		blocksColors[4]		= {   0, 128, 128, 255 };
		blocksColors[5]		= { 128,   0,   0, 255 };
		blocksColors[6]		= { 128,   0, 128, 255 };
		blocksColors[7]		= { 128, 128,   0, 255 };
		blocksColors[8]		= { 128, 128, 128, 255 };
		blocksColors[9]		= {   0,   0, 255, 255 };
		blocksColors[10]	= {   0, 255,   0, 255 };
		blocksColors[11]	= {   0, 255, 255, 255 };
		blocksColors[12]	= { 255,   0,   0, 255 };
		blocksColors[13]	= { 255,   0, 255, 255 };
		blocksColors[14]	= { 255, 255,   0, 255 };
		blocksColors[15]	= { 191, 191, 191, 255 };
		blocksColors[16]	= { 255, 255, 255, 255 };
#endif

		return blocksColors;
	}();

	constexpr auto ColorsFlt = []()
	{
		std::array<Color<float>, static_cast<Bl_t>(BlockN::MAX) + 1> blocksColors{};
		constexpr float div = std::numeric_limits<uint8_t>::max();

		for (size_t i = 0; i < ColorsChr.size(); ++i)
			blocksColors[i] = { ColorsChr[i].R / div, ColorsChr[i].G / div, ColorsChr[i].B / div, ColorsChr[i].A / div };

		return blocksColors;
	}();




}
