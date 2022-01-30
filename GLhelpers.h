#pragma once
#include "defines.h"

#include <array>

#include "GL/glut.h"

#include "functions.h"


extern int currentWindowWidth;
extern int currentWindowHeight;


static void drawStringOnWindow(float x, float y, float z, void* font, const std::vector<std::string>& lines, int corner = 2, int lineSpace = 10)
{
	corner = corner % 4;

	int lineHeight = lineSpace; // + glutBitmapHeight(font);

	if (!lines.empty())
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0f, currentWindowWidth, 0.0f, currentWindowHeight, 1.0f, -1.0f);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		//		glDisable(GL_LIGHTING);

		size_t linesCount = lines.size();
		size_t lineNum = 1;
		for (const std::string& line : lines)
		{
			// 1|0
			// -+-
			// 2|3

			int xstr, ystr;
			int lineWidth = 0;
			for (char const& c : line)
				lineWidth += glutBitmapWidth(font, c);

			if (corner == 0 || corner == 3)
				xstr = currentWindowWidth - x - lineWidth;
			else
				xstr = x;

			if (corner == 0 || corner == 1)
				ystr = currentWindowHeight - y - lineHeight * lineNum;
			else
				ystr = y + lineHeight * (linesCount - lineNum);

			glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_QUADS);
			glVertex3f(xstr - 1, ystr - 1, z + FLT_EPSILON);
			glVertex3f(xstr + lineWidth, ystr - 1, z + FLT_EPSILON);
			glVertex3f(xstr + lineWidth, ystr + lineHeight - 1, z + FLT_EPSILON);
			glVertex3f(xstr - 1, ystr + lineHeight - 1, z + FLT_EPSILON);
			glEnd();

			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glRasterPos3i(xstr, ystr, z);

			for (char const& c : line)
				glutBitmapCharacter(font, c);

			++lineNum;
		}

//		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
}

static void drawStringOnWindow(float x, float y, float z, void* font, const std::string& str, int corner = 2, int lineSpace = 10)
{
	std::vector<std::string> lines;
	std::string lnbr("\n");
	std::size_t start = 0;
	std::size_t end = 0;
	do
	{
		end = str.find(lnbr, start);
		lines.push_back(str.substr(start, end - start));
		start = end + 1;
	} while (end != std::string::npos);

	drawStringOnWindow(x, y, z, font, lines, corner, lineSpace);
}

static inline void drawStringOnWindow(float x, float y, float z, void* font, const char* str, int corner = 2, int lineSpace = 10)
{
	drawStringOnWindow(x, y, z, font, std::string(str), corner, lineSpace);
}



template <typename T = uint8_t, std::enable_if_t<std::is_arithmetic<T>::value, bool> = true>
class Color
{
public:
	T R;
	T G;
	T B;
	T A;
};



#if HEIGHT_ITPL_GAUSS
static constexpr std::array<float, BIOME_ITPL_RDS + 1> generate_biome_weights()
{
	constexpr float sigmas = 2.5f;
	constexpr float std = (BIOME_ITPL_RDS ? BIOME_ITPL_RDS : 1) / sigmas;
	constexpr float DCoffset = gauss_pdf_dscrt(-2, BIOME_ITPL_RDS, std); // -1?

	std::array<float, BIOME_ITPL_RDS + 1> multipliers{ };

	size_t i = 0;
	float sum = 0.0f;
	for (size_t i = 0; i < BIOME_ITPL_RDS; ++i)
	{
		multipliers[i] = gauss_pdf_dscrt(i, BIOME_ITPL_RDS, std) - DCoffset;
		sum += 2 * multipliers[i];
	}
	multipliers[BIOME_ITPL_RDS] = gauss_pdf_dscrt(BIOME_ITPL_RDS, BIOME_ITPL_RDS, std) - DCoffset;
	sum += multipliers[BIOME_ITPL_RDS];

	for (size_t i = 0; i <= BIOME_ITPL_RDS; ++i)
		multipliers[i] /= sum;

	return multipliers;
}
#endif

constexpr auto generate_vertices()
{
	std::array<GLfloat, CHUNK_VERTNUM * 3> points{};

	size_t i = 0;
	for (BlkCrd y = 0; y <= CHUNK_HEIGHT; ++y)
	{
		for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		{
			points[i++] = static_cast<GLfloat>(x);
			points[i++] = static_cast<GLfloat>(y);
		}
		points[i++] = nextafter(static_cast<GLfloat>(CHUNK_WIDTH));
		points[i++] = static_cast<GLfloat>(y);
	}
	return points;
}

constexpr auto generate_faces()
{
	typedef GLushort DtTp;
	std::array<DtTp, CHUNK_BLOCKNUM * 4> points{}; // GLubyte, GLushort, GLuint

	size_t i = 0;
	for (BlkCrd y = 0; y < CHUNK_HEIGHT; ++y)
	{
		for (BlkCrd x = 0; x < CHUNK_WIDTH; ++x)
		{
			points[i++] = static_cast<DtTp>(y * (CHUNK_WIDTH + 1) + x);
			points[i++] = static_cast<DtTp>(y * (CHUNK_WIDTH + 1) + x + 1);
			points[i++] = static_cast<DtTp>((y + 1) * (CHUNK_WIDTH + 1) + x + 1);
			points[i++] = static_cast<DtTp>((y + 1) * (CHUNK_WIDTH + 1) + x);
		}
	}
	return points;
}

