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
					blocks[pos] = Block(blockID);
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



void setColor(Bl_t block)
{
	switch (block)
	{
	case 1: //stone
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		break;
	case 2: // dirt
		glColor4f(0.545098f, 0.270588f, 0.0745098f, 1.0f);
		break;
	case 3: // grass
		glColor4f(0.0f, 0.5f, 0.0f, 1.0f);
		break;

	case 6: // bedrock
		glColor4f(0.2f, 0.2f, 0.2f, 1.0f);
		break;

	case 8: // water
		glColor4f(0.0f, 0.02f, 1.0f, 0.75f);
		break;
	case 9: // lava
		glColor4f(1.0f, 0.2f, 0.2f, 0.75f);
		break;

	case 10: // sand
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		break;

	default: // error
		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
		break;
	}
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
#if CLR_NO_FLOAT
		ClrT colors[CHUNK_VERTNUM*4] = {};
#else
		GLfloat colors[CHUNK_VERTNUM*4] = {};
#endif
		size_t i = (CHUNK_WIDTH + 1) * 4;
		for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
			{
#if CLR_NO_FLOAT
				memcpy(&colors[i], &colorsArr[Bl_t(blocks[y * CHUNK_WIDTH + x].ID) * 4], 4 * sizeof(ClrT));
#else
				memcpy(&colors[i], &blockColorsFloat[Bl_t(blocks[y * CHUNK_WIDTH + x].ID) * 4], 4 * sizeof(GLfloat));
#endif
				i += 4;
			}
			i += 4;
		}
//		setColor(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, pointsPtr);
#if CLR_NO_FLOAT
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
	std::string path = getPath();

	std::ofstream chunkFile;
	chunkFile.open(path, std::ios::binary | std::ios::out);

	if (chunkFile.good())
	{
		std::cout << "Chunk:: save" << std::endl;

		for (Block block : blocks)
		{
			Bl_t blockID = Bl_t(block.ID);
//		std::cout << "Write: " << blockID << "\t";
			// VarInt write
			uint8_t buffer[5];
			buffer[0] = (blockID >> 28) & (uint8_t)127 | (uint8_t)128;
			buffer[1] = (blockID >> 21) & (uint8_t)127 | (uint8_t)128;
			buffer[2] = (blockID >> 14) & (uint8_t)127 | (uint8_t)128;
			buffer[3] = (blockID >> 7)  & (uint8_t)127 | (uint8_t)128;
			buffer[4] = (blockID)       & (uint8_t)127;

			if (blockID >= (Bl_t)1 << 28)
			{
				chunkFile << buffer[0];
//				std::cout << (uint)buffer[0] << " ";
			}

			if (blockID >= (Bl_t)1 << 21)
			{
				chunkFile << buffer[1];
//				std::cout << (uint)buffer[1] << " ";
			}

			if (blockID >= (Bl_t)1 << 14)
			{
				chunkFile << buffer[2];
//				std::cout << (uint)buffer[2] << " ";
			}

			if (blockID >= (Bl_t)1 << 7)
			{
				chunkFile << buffer[3];
//				std::cout << (uint)buffer[3] << " ";
			}

			chunkFile << buffer[4];
//			std::cout << (uint)buffer[4] << " " << std::endl;

		}
	}
	chunkFile.close();

	return true;
}


void Chunk::testFill()
{
	size_t xd = 0;
	for (Block& block : blocks)
		block = Block(xd++);

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



Block Chunk::getBlockAt(BlkCrd x, BlkCrd y) const
{
	if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT)
	{
//		std::cout << "shit";
		return BlockN::air;
	}
//	std::cout << "gudd";
	return blocks[y * CHUNK_WIDTH + x];
}
