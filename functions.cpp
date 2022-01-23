#
#include "functions.h"

#include "GL/glut.h"



extern int currentWindowWidth;
extern int currentWindowHeight;


void drawStringOnWindow(float x, float y, float z, void* font, const char* str, int corner, int lineSpace)
{
	drawStringOnWindow(x, y, z, font, std::string(str), corner, lineSpace);
}

void drawStringOnWindow(float x, float y, float z, void* font, const std::string& str, int corner, int lineSpace)
{
	std::vector<std::string> lines;
	std::string lnbr("\n");
	std::size_t start = 0;
	std::size_t end = 0;
	do
	{
		end = str.find(lnbr, start);
//		std::cout << end << std::endl;
		lines.push_back(str.substr(start, end - start));
		start = end + 1;
	} 	while (end != std::string::npos);

	drawStringOnWindow(x, y, z, font, lines, corner, lineSpace);
}

void drawStringOnWindow(float x, float y, float z, void* font, const std::vector<std::string>& lines, int corner, int lineSpace)
{
	/*
	std::cout << "||";
	for (auto str : lines)
		std::cout << str << std::endl;
	std::cout << "||" << std::endl;
	//*/

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
			glVertex3f(xstr - 1,         ystr - 1,              z + FLT_EPSILON);
			glVertex3f(xstr + lineWidth, ystr - 1,              z + FLT_EPSILON);
			glVertex3f(xstr + lineWidth, ystr + lineHeight - 1, z + FLT_EPSILON);
			glVertex3f(xstr - 1,         ystr + lineHeight - 1, z + FLT_EPSILON);
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
