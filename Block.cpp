#

#include "Block.h"



Block::Block(BlockN _ID) : ID(_ID) {}

Block::Block(Bl_t _ID) : ID(BlockN(_ID)) {}

Block::~Block() {}

EntCrd Block::boxTrel() const
{
	return 1.0;
}
EntCrd Block::boxBrel() const
{
	return 0.0;
}
EntCrd Block::boxRrel() const
{
	return 1.0;
}
EntCrd Block::boxLrel() const
{
	return 0.0;
}


Pos Block::posTRrel() const
{
	return { boxRrel(), boxTrel() };
}
Pos Block::posTLrel() const
{
	return { boxLrel(), boxTrel() };
}
Pos Block::posBLrel() const
{
	return { boxLrel(), boxBrel() };
}
Pos Block::posBRrel() const
{
	return { boxRrel(), boxBrel() };
}
