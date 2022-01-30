#
#include "Chunk.h"

#include <fstream>
#include <iostream>
#include "Filesystem.h"

#include "ChunkGen.h"



Chunk::Chunk(ChkCrd _Xpos) : Xpos(_Xpos)
{
	std::string path = getPath();

	if (fs::exists(path))
	{
#if CONSOLE_LOG_CHUNKS
		std::cout << "Chunk:: reading from file " << Xpos << std::endl;
#endif
		std::ifstream chunkFile(path, std::ios_base::binary);
		if (chunkFile.good())
		{
			std::istream_iterator<uint8_t> v1(chunkFile >> std::noskipws);
			std::istream_iterator<uint8_t> v2;
			std::vector<uint8_t> bytesBuffer(v1, v2);

			Bl_t blockID = 0;
			std::size_t pos = 0;

			for (uint8_t bt : bytesBuffer)
			{
				// VarInt read
				blockID |= bt & (uint8_t)127;
				if (bt & (uint8_t)128)
					blockID <<= 7;
				else
				{
					blocks[pos] = BlockN(blockID);
					blockID = 0;

					if (++pos == CHUNK_BLOCKNUM)
						break;
				}
			}
		}
		chunkFile.close();

	}
	else
	{
#if CONSOLE_LOG_CHUNKS
		std::cout << "Chunk:: file " << _Xpos << " doesnt exist" << std::endl;
#endif
		
		ChunkGen generator(Xpos, blocks);

		generator.generate_chunk();

	}
}

Chunk::Chunk() {}

Chunk::~Chunk()
{
	//	Chunk::save();
}



void Chunk::draw(BlkCrd Ymin, BlkCrd Ymax) const
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(Xpos * CHUNK_WIDTH, 0.0f, Z_VAL_TERRAIN);

	Ymax = std::clamp(Ymax, 0, CHUNK_HEIGHT);
	Ymin = std::clamp(Ymin, 0, CHUNK_HEIGHT);

	size_t blockCount  = (Ymax - Ymin) * CHUNK_WIDTH * 4;
	size_t blockOffset = Ymin * CHUNK_WIDTH * 4;

#if DRAW_FACES
	{
#if CLR_BYTE
		uint8_t colors[CHUNK_VERTNUM * 4] = {};
#else
		GLfloat colors[CHUNK_VERTNUM * 4] = {};
#endif
		size_t i = (CHUNK_WIDTH + 1) * 4;
		for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
			{
#if CLR_BYTE
				memcpy(&colors[i], &Block::ColorsChr[Bl_t(blocks[y * CHUNK_WIDTH + x])], sizeof(Color<uint8_t>));
#else
				memcpy(&colors[i], &Block::ColorsFlt[Bl_t(blocks[y * CHUNK_WIDTH + x])], sizeof(Color<float>));
#endif
				i += 4;
			}
			i += 4;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, pointsPtr);
#if CLR_BYTE
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors);
#else
		glColorPointer(4, GL_FLOAT, 0, &colors);
#endif

		glDrawElements(GL_QUADS, blockCount, GL_UNSIGNED_SHORT, facesPtr + blockOffset);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
#endif

#if DRAW_BORDERS
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, pointsPtr);

		glDrawElements(GL_QUADS, blockCount, GL_UNSIGNED_SHORT, facesPtr + blockOffset);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
#endif
//	glPopMatrix();                      // Restore the model-view matrix
}

std::string Chunk::getPath() const
{
	return CHUNK_PATH + std::to_string(Xpos) + CHUNK_EXT;
}


bool Chunk::save() const
{
	constexpr size_t piecesCount = sizeof(Bl_t) * CHAR_BIT;

	std::string path = getPath();

	std::ofstream chunkFile;
	chunkFile.open(path, std::ios::binary | std::ios::out);

	if (chunkFile.good())
	{
		std::cout << "Chunk:: save" << std::endl;

		for (BlockN block : blocks)
		{
			auto result = integer_to_varint(static_cast<Bl_t>(block));
			
			for (; result.index < result.bytes.size(); ++result.index)
				chunkFile << result.bytes[result.index];

		}
	}
	chunkFile.close();

	return true;
}


void Chunk::testFill()
{
	size_t xd = 0;
	for (BlockN& block : blocks)
		block = BlockN(xd++);

}

void Chunk::flatFill()
{
	for (int lev = 0; lev <= 0; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = BlockN::bedrock;

	for (int lev = 1; lev <= 3; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = BlockN::stone;

	for (int lev = 4; lev <= 5; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = BlockN::dirt;

	for (int lev = 6; lev <= 6; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = BlockN::grass;

	for (int lev = 7; lev < CHUNK_HEIGHT; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = BlockN::air;

}



BlockN Chunk::getBlockAt(BlkCrd x, BlkCrd y) const
{
	if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT)
	{
//		std::cout << "shit";
		return BlockN::air;
	}
//	std::cout << "gudd";
	return blocks[y * CHUNK_WIDTH + x];
}
