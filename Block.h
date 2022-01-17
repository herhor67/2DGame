#pragma once

#include "typedefs.h"
#include "defines.h"

#include "Environment.h"


enum class BlockN { MIN = -1, air, stone, dirt, grass, bedrock, water, lava, sand, sandstone, snow, ice, cactoo, pumpkin, sprucewood, spruceleaves, oakwood, oakleaves, accaciawood, accacialeaves, redwood, redleaves, redcoral, greencoral, yellowcoral, bluecoral, drygrass, MAX };
typedef std::underlying_type_t<BlockN> Bl_t;

class Block
{
public:
	BlockN ID;

	Block(BlockN = BlockN::air);
	Block(Bl_t);
	~Block();

	EntCrd boxTrel() const;
	EntCrd boxBrel() const;
	EntCrd boxRrel() const;
	EntCrd boxLrel() const;

	Pos posTRrel() const;
	Pos posTLrel() const;
	Pos posBLrel() const;
	Pos posBRrel() const;
};