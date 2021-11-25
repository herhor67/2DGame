#pragma once

#include "typedefs.h"
#include "defines.h"

#include "Environment.h"


class Block
{
public:
	uint ID;
	Block(uint = 0);
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