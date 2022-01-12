
#include <fstream>
#include <iostream>
#include "Filesystem.h"

#include "Chunk.h"
#include "TerrainGen.h"

#include "Perlin.h"

static constexpr auto pointsArr = generate_vertices<>();
static constexpr auto pointsPtr = pointsArr.data();
static constexpr auto facesArr = generate_faces<>();
static constexpr auto facesPtr = facesArr.data();
static constexpr auto colorsArr = generate_colors<ClrT>();


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
			std::istream_iterator<byte> v1(chunkFile >> std::noskipws);
			std::istream_iterator<byte> v2;
			std::vector<byte> bytesBuffer(v1, v2);

			uint blockID = 0;
			std::size_t pos = 0;

			for (byte bt : bytesBuffer)
			{
				// VarInt read
				blockID |= bt & 127;
				if (bt & 128)
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
		
		TerrainGen generator(Xpos, blocks);

		generator.fill_chunk();

	}
}

Chunk::Chunk() {}

Chunk::~Chunk()
{
	//	Chunk::save();
}



void setColor(uint block)
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


void Chunk::draw() const
{
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);     // To operate on Model-View matrix
	glLoadIdentity();               // Reset the model-view matrix
	glPushMatrix();                     // Save model-view matrix setting
	glTranslatef(Xpos * CHUNK_WIDTH, 0.0f, Z_VAL_TERRAIN);    // Translate

#if DRAW_FACES
	{
#if CLR_NO_FLOAT
		ClrT colors[CHUNK_VERTNUM*4] = {};
#else
		GLfloat colors[CHUNK_VERTNUM*4] = {};
#endif
		size_t i = 0;
		for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
		{
			for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
			{
#if CLR_NO_FLOAT
				memcpy(&colors[i], &colorsArr[blocks[y * CHUNK_WIDTH + x].ID * 4], 4 * sizeof(ClrT));
#else
				memcpy(&colors[i], &blockColorsFloat[blocks[y * CHUNK_WIDTH + x].ID * 4], 4 * sizeof(GLfloat));
#endif
				i += 4;
			}
			i += 4;
		}
		setColor(0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, pointsPtr);
#if CLR_NO_FLOAT
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors);
#else
		glColorPointer(4, GL_FLOAT, 0, &colors);
#endif

		glDrawElements(GL_QUADS, CHUNK_BLOCKNUM*4, GL_UNSIGNED_SHORT, facesPtr); // GL_UNSIGNED_SHORT, GL_UNSIGNED_INT

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

		glDrawElements(GL_QUADS, CHUNK_BLOCKNUM*4, GL_UNSIGNED_SHORT, facesPtr);

		glDisableClientState(GL_VERTEX_ARRAY);
	}
#endif
	glPopMatrix();                      // Restore the model-view matrix
}

/*
void Chunk::draw() const
{
	glMatrixMode(GL_MODELVIEW);     // To operate on Model-View matrix
	glLoadIdentity();               // Reset the model-view matrix

	glPushMatrix();                     // Save model-view matrix setting
	glTranslatef(Xpos * CHUNK_WIDTH, 0.0f, Z_VAL_TERRAIN);    // Translate

#if DRAW_FACES
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);                  // Each set of 4 vertices form a quad

		uint yBcrd = 0;
		uint xBcrd = 0;
		for (Block block : blocks)
		{
			if (block.ID != 0)
			{
				setColor(block.ID);

				glVertex2f(xBcrd, yBcrd);
				glVertex2f(xBcrd + 1, yBcrd);
				glVertex2f(xBcrd + 1, yBcrd + 1);
				glVertex2f(xBcrd, yBcrd + 1);
			}

			if (++xBcrd == CHUNK_WIDTH)
			{
				xBcrd = 0;
				++yBcrd;
			}
		}
		glEnd();
	}
#endif
#if DRAW_BORDERS
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUADS);                  // Each set of 4 vertices form a quad
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		uint yBcrd = 0;
		uint xBcrd = 0;
		for (Block block : blocks)
		{
			glVertex2f(xBcrd, yBcrd);
			glVertex2f(xBcrd + 1, yBcrd);
			glVertex2f(xBcrd + 1, yBcrd + 1);
			glVertex2f(xBcrd, yBcrd + 1);

			if (++xBcrd == CHUNK_WIDTH)
			{
				xBcrd = 0;
				++yBcrd;
			}
		}
		glEnd();
	}
#endif
	glPopMatrix();                      // Restore the model-view matrix
}
//*/


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
			uint blockID = block.ID;
//		std::cout << "Write: " << blockID << "\t";
			// VarInt write
			byte buffer[5] = { 0 };
			buffer[0] = (blockID >> 28) & 127 | 128;
			buffer[1] = (blockID >> 21) & 127 | 128;
			buffer[2] = (blockID >> 14) & 127 | 128;
			buffer[3] = (blockID >> 7) & 127 | 128;
			buffer[4] = (blockID) & 127;

			if (blockID >= (uint)1 << 28)
			{
				chunkFile << buffer[0];
//				std::cout << (uint)buffer[0] << " ";
			}

			if (blockID >= (uint)1 << 21)
			{
				chunkFile << buffer[1];
//				std::cout << (uint)buffer[1] << " ";
			}

			if (blockID >= (uint)1 << 14)
			{
				chunkFile << buffer[2];
//				std::cout << (uint)buffer[2] << " ";
			}

			if (blockID >= (uint)1 << 7)
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
	uint xd = 0;
	for (Block& block : blocks)
		block = Block(xd++);

}

void Chunk::flatFill()
{
	for (int lev = 0; lev <= 0; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = Block(6);

	for (int lev = 1; lev <= 3; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = Block(1);

	for (int lev = 4; lev <= 5; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = Block(2);

	for (int lev = 6; lev <= 6; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = Block(3);

	for (int lev = 7; lev < CHUNK_HEIGHT; ++lev)
		for (int x = 0; x < CHUNK_WIDTH; ++x)
			blocks[lev * CHUNK_WIDTH + x] = Block(0);

}



Block Chunk::getBlockAt(BlkCrd x, BlkCrd y) const
{
	if (x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT)
	{
//		std::cout << "shit";
		return Block(0);
	}
//	std::cout << "gudd";
	return blocks[y * CHUNK_WIDTH + x];
}
