#define NOMINMAX 1

#include <iostream>
#include <limits>
#include "Filesystem.h"

#include "GL/glut.h"

#include "ChunkManager.h"

#include "functions.h"



void ChunkManager::updateTnow(int _Tnow)
{
	Tnow = _Tnow;
}

void ChunkManager::removeOld()
{
	for (auto itr = chunks.cbegin(); itr != chunks.cend(); /**/)
		if (Tnow - itr->second.second > CHUNK_TTL_MS)
			itr = chunks.erase(itr);
		else
			itr = std::next(itr);
}

void ChunkManager::updateTTL(ChkCrd Xpos)
{
	chunks[Xpos].second = Tnow;
}

ChunkManager::ChT& ChunkManager::readChunk(ChkCrd Xpos)
{
	return chunks.emplace(Xpos, std::make_pair(Chunk(Xpos), Tnow)).first->second;
}

void ChunkManager::loadChunk(ChkCrd Xpos)
{
	if (chunks.find(Xpos) == chunks.end())
		readChunk(Xpos);
}

Chunk& ChunkManager::getChunk(ChkCrd Xpos)
{
	if (chunks.find(Xpos) == chunks.end())
		return readChunk(Xpos).first;

	return chunks.find(Xpos)->second.first;
}



std::vector<BlkCrd> possibleBlocksCollisions(EntCrd p1, EntCrd p2)
{
	BlkCrd first = floor(std::min(p1, p2));
	BlkCrd last  =  ceil(std::max(p1, p2)) - 1;

	size_t count = last - first + 1;

	std::vector<BlkCrd> temp(count);

	BlkCrd p = first;
	for (size_t i = 0; i < count; ++i)
		temp[i] = first + i;
	
	return temp;
}



size_t ChunkManager::count()
{
	return chunks.size();
}


Block ChunkManager::getBlockAt(BlkCrd x, BlkCrd y)
{
	BlkCrd blXpos = x % CHUNK_WIDTH;
	if (blXpos < 0)
		blXpos = (blXpos + CHUNK_WIDTH) % CHUNK_WIDTH;

	BlkCrd chXpos = (x - blXpos) / CHUNK_WIDTH;

//	std::cout << "Check chX blX blY: " << chXpos << ' ' << blXpos << ' ' << y << std::endl;

	return getChunk(chXpos).getBlockAt(blXpos, y);
}

std::tuple<Pos, Block, Block, int, EntCrd> ChunkManager::checkCollision(const Movement& movement, const Entity& entity)
{
	EntCrd dX = movement.second.X - movement.first.X;
	EntCrd dY = movement.second.Y - movement.first.Y;
	
	Pos finalPos_X = movement.second;
	Pos finalPos_Y = movement.second;
	Block finalBlock_X(0);
	Block finalBlock_Y(0);
	int finalSide_X = SIDE_NONE;
	int finalSide_Y = SIDE_NONE;

	EntCrd maxDistSq = getDistSq(dX, dY);
	EntCrd maxDistSq_X = maxDistSq;
	EntCrd maxDistSq_Y = maxDistSq;

//	std::cout << "Expected dist: X: " << dX << " Y: " << dY << " Total: " << sqrt(maxDistSq) << std::endl;

	//*
	if (dY < 0) // moving down, collide bottom of Entity with tops of blocks
	{
		EntCrd begY = movement.first.Y  + entity.boxBrel();
		EntCrd endY = movement.second.Y + entity.boxBrel();

		EntCrd begX = std::min(movement.first.X, movement.second.X) + entity.boxLrel();
		EntCrd endX = std::max(movement.first.X, movement.second.X) + entity.boxRrel();

//		std::cout << "MinY: " << begY << " MaxY: " << endY << std::endl;
//		std::cout << "MinX: " << begX << " MaxX: " << endX << std::endl;

		uint count = floor(entity.width) + 2; // perhaps use dynamically for each or check smallest block in the area? / smallestBlockWidth

		// make a list of points (relative to entity position) which we want to check
		std::vector<Pos> checkPts = interpolate(entity.posBLrel(), entity.posBRrel(), count);
//		std::cout << "Check points:" << std::endl;
//		for (Pos& x : checkPts)
//			x.cout();

		// make a list of blocks which we want to check
		std::vector<BlkCrd> loopYs = possibleBlocksCollisions(begY, endY);
		std::vector<BlkCrd> loopXs = possibleBlocksCollisions(begX, endX);

//		std::cout << std::endl;

		// for each block, check if any of the checked lines collides with the side
		for (const BlkCrd& blkY : loopYs)
		{
			for (const BlkCrd& blkX : loopXs)
			{
				Block blk = getBlockAt(blkX, blkY);

				if (blk.ID != 0)
				{
					Pos blkPos = { blkX, blkY };

//					std::cout << "Checking block: X: " << blkX << " Y: " << blkY << std::endl;
//					std::cout << "Top layer: " << blkY + blk.boxTrel() << " bounded: (" << blkX + blk.boxLrel() << ", " << blkX + blk.boxRrel() << ") " << std::endl;

					for (const Pos& chkPt : checkPts)
					{
//						std::cout << "Checking point: ";
//						chkPt.cout();

						std::pair<bool, Pos> collParams = findIntersect(movement.first + chkPt, movement.second + chkPt, blkPos + blk.posTLrel(), blkPos + blk.posTRrel());

//						std::cout << "Collided? " << (collParams.first ? "yes" : "no") << std::endl;
						if (collParams.first)
						{

							Pos newEntPos = collParams.second - chkPt; // pos + pt = line, then coll - pt = pos
							EntCrd newDistSq = getDistSq(movement.first, newEntPos);

							if (newDistSq < maxDistSq_Y)
							{
								maxDistSq_Y = newDistSq;
								finalPos_Y  = newEntPos;

								finalSide_Y  = SIDE_BOTTOM;
								finalBlock_Y = blk;
							}
						}
					}
//					std::cout << std::endl;
				}
			}
		}
	}
	//*/
	//*
	if (dY > 0) // moving up, collide top of Entity with bottoms of blocks
	{
		EntCrd begY = movement.first.Y + entity.boxTrel();
		EntCrd endY = movement.second.Y + entity.boxTrel();

		EntCrd begX = std::min(movement.first.X, movement.second.X) + entity.boxLrel();
		EntCrd endX = std::max(movement.first.X, movement.second.X) + entity.boxRrel();

//		std::cout << "MinY: " << begY << " MaxY: " << endY << std::endl;
//		std::cout << "MinX: " << begX << " MaxX: " << endX << std::endl;

		uint count = floor(entity.width) + 2; // perhaps use dynamically for each or check smallest block in the area? / smallestBlockWidth

		// make a list of points (relative to entity position) which we want to check
		std::vector<Pos> checkPts = interpolate(entity.posTLrel(), entity.posTRrel(), count);
//		std::cout << "Check points:" << std::endl;
//		for (Pos& x : checkPts)
//			x.cout();

		// make a list of blocks which we want to check
		std::vector<BlkCrd> loopYs = possibleBlocksCollisions(begY, endY);
		std::vector<BlkCrd> loopXs = possibleBlocksCollisions(begX, endX);

//		std::cout << std::endl;

		// for each block, check if any of the checked lines collides with the side
		for (const BlkCrd& blkY : loopYs)
		{
			for (const BlkCrd& blkX : loopXs)
			{
				Block blk = getBlockAt(blkX, blkY);

				if (blk.ID != 0)
				{
					Pos blkPos = { blkX, blkY };

					for (const Pos& chkPt : checkPts)
					{
//						std::cout << "Checking point: ";
//						chkPt.cout();

						std::pair<bool, Pos> collParams = findIntersect(movement.first + chkPt, movement.second + chkPt, blkPos + blk.posBLrel(), blkPos + blk.posBRrel());

//						std::cout << "Collided? " << (collParams.first ? "yes" : "no") << std::endl;
						if (collParams.first)
						{

							Pos newEntPos = collParams.second - chkPt; // pos + pt = line, then coll - pt = pos
							EntCrd newDistSq = getDistSq(movement.first, newEntPos);

							if (newDistSq < maxDistSq_Y)
							{
								maxDistSq_Y = newDistSq;
								finalPos_Y = newEntPos;

								finalSide_Y = SIDE_TOP;
								finalBlock_Y = blk;
							}
						}
					}
//					std::cout << std::endl;
				}
			}
		}
	}
	//*/


	//*
	if (dX < 0) // moving down, collide bottom of Entity with tops of blocks
	{
		EntCrd begX = movement.first.X + entity.boxLrel();
		EntCrd endX = movement.second.X + entity.boxLrel();

		EntCrd begY = std::min(movement.first.Y, movement.second.Y) + entity.boxBrel();
		EntCrd endY = std::max(movement.first.Y, movement.second.Y) + entity.boxTrel();

//		std::cout << "MinX: " << begX << " MaxX: " << endX << std::endl;
//		std::cout << "MinY: " << begY << " MaxY: " << endY << std::endl;

		uint count = floor(entity.height) + 2; // perhaps use dynamically for each or check smallest block in the area? / smallestBlockWidth

		// make a list of points (relative to entity position) which we want to check
		std::vector<Pos> checkPts = interpolate(entity.posTLrel(), entity.posBLrel(), count);
//		std::cout << "Check points:" << std::endl;
//		for (Pos& x : checkPts)
//			x.cout();

		// make a list of blocks which we want to check
		std::vector<BlkCrd> loopYs = possibleBlocksCollisions(begY, endY);
		std::vector<BlkCrd> loopXs = possibleBlocksCollisions(begX, endX);

//		std::cout << std::endl;

		// for each block, check if any of the checked lines collides with the side
		for (const BlkCrd& blkY : loopYs)
		{
			for (const BlkCrd& blkX : loopXs)
			{
				Block blk = getBlockAt(blkX, blkY);

				if (blk.ID != 0)
				{
					Pos blkPos = { blkX, blkY };

//					std::cout << "Checking block: X: " << blkX << " Y: " << blkY << std::endl;

					for (const Pos& chkPt : checkPts)
					{
//						std::cout << "Checking point: ";
//						chkPt.cout();

						std::pair<bool, Pos> collParams = findIntersect(movement.first + chkPt, movement.second + chkPt, blkPos + blk.posTRrel(), blkPos + blk.posBRrel());

//						std::cout << "Collided? " << (collParams.first ? "yes" : "no") << std::endl;
						if (collParams.first)
						{

							Pos newEntPos = collParams.second - chkPt; // pos + pt = line, then coll - pt = pos
							EntCrd newDistSq = getDistSq(movement.first, newEntPos);

							if (newDistSq < maxDistSq_X)
							{
								maxDistSq_X = newDistSq;
								finalPos_X = newEntPos;

								finalSide_X = SIDE_RIGHT;
								finalBlock_X = blk;
							}
						}
					}
//					std::cout << std::endl;
				}
			}
		}
	}
	//*/
	//*
	if (dX > 0) // moving down, collide bottom of Entity with tops of blocks
	{
		EntCrd begX = movement.first.X + entity.boxRrel();
		EntCrd endX = movement.second.X + entity.boxRrel();

		EntCrd begY = std::min(movement.first.Y, movement.second.Y) + entity.boxBrel();
		EntCrd endY = std::max(movement.first.Y, movement.second.Y) + entity.boxTrel();

//		std::cout << "MinX: " << begX << " MaxX: " << endX << std::endl;
//		std::cout << "MinY: " << begY << " MaxY: " << endY << std::endl;

		uint count = floor(entity.height) + 2; // perhaps use dynamically for each or check smallest block in the area? / smallestBlockWidth

		// make a list of points (relative to entity position) which we want to check
		std::vector<Pos> checkPts = interpolate(entity.posTRrel(), entity.posBRrel(), count);
//		std::cout << "Check points:" << std::endl;
//		for (Pos& x : checkPts)
//			x.cout();

		// make a list of blocks which we want to check
		std::vector<BlkCrd> loopYs = possibleBlocksCollisions(begY, endY);
		std::vector<BlkCrd> loopXs = possibleBlocksCollisions(begX, endX);

//		std::cout << std::endl;

		// for each block, check if any of the checked lines collides with the side
		for (const BlkCrd& blkY : loopYs)
		{
			for (const BlkCrd& blkX : loopXs)
			{
				Block blk = getBlockAt(blkX, blkY);

				if (blk.ID != 0)
				{
					Pos blkPos = { blkX, blkY };

//					std::cout << "Checking block: X: " << blkX << " Y: " << blkY << std::endl;

					for (const Pos& chkPt : checkPts)
					{
//						std::cout << "Checking point: ";
//						chkPt.cout();

						std::pair<bool, Pos> collParams = findIntersect(movement.first + chkPt, movement.second + chkPt, blkPos + blk.posTLrel(), blkPos + blk.posBLrel());

//						std::cout << "Collided? " << (collParams.first ? "yes" : "no") << std::endl;
						if (collParams.first)
						{

							Pos newEntPos = collParams.second - chkPt; // pos + pt = line, then coll - pt = pos
							EntCrd newDistSq = getDistSq(movement.first, newEntPos);

							if (newDistSq < maxDistSq_X)
							{
								maxDistSq_X = newDistSq;
								finalPos_X = newEntPos;

								finalSide_X = SIDE_LEFT;
								finalBlock_X = blk;
							}
						}
					}
//					std::cout << std::endl;
				}
			}
		}
	}
	//*/

//	std::cout << "maxDist_X: " << sqrt(maxDistSq_X) << std::endl;
//	std::cout << "maxDist_Y: " << sqrt(maxDistSq_Y) << std::endl;

	
	EntCrd ratio = 1;
	
	if (maxDistSq_X == maxDistSq_Y)
	{
		ratio = maxDistSq ? sqrt(maxDistSq_X / maxDistSq) : ratio;
		return std::make_tuple(finalPos_X, finalBlock_X, finalBlock_Y, finalSide_X | finalSide_Y, ratio);
	}
	
	else if (maxDistSq_X > maxDistSq_Y)
	{
		ratio = maxDistSq ? sqrt(maxDistSq_Y / maxDistSq) : ratio;
		return std::make_tuple(finalPos_Y, Block(0), finalBlock_Y, finalSide_Y, ratio);
	}
	else if (maxDistSq_X < maxDistSq_Y)
	{
		ratio = maxDistSq ? sqrt(maxDistSq_X / maxDistSq) : ratio;
		return std::make_tuple(finalPos_X, finalBlock_X, Block(0), finalSide_X, ratio);
	}
	else
	{
		throw std::logic_error("What the fuck happened, all ifs failed");
	}
}