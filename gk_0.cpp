#
#define NOMINMAX 1

#include <iostream>
#include <string>
#include "Filesystem.h"
#include "Perlin.h"

#define GLUT_DISABLE_ATEXIT_HACK
#define GLUT_API_VERSION 4
#include "GL/glut.h"
#include <FastNoise/FastNoise.h>

#include "ChunkManager.h"
#include "Entity.h"

#include "typedefs.h"
#include "defines.h"
#include "functions.h"

using namespace std;



char title[] = "The Game";  // Windowed mode's title
int windowedWidth = 1440;     // Windowed mode's width
int windowedHeight = 810;     // Windowed mode's height
int windowedPosX = 1000;      // Windowed mode's top-left corner x
int windowedPosY = 200;      // Windowed mode's top-left corner y
int windowID;

int refreshMills = 1000 / 60 / 2; // refresh interval in milliseconds
//int refreshMills = 100;

enum ScreenModes { windowed = -1, borderless, fullscreen };
int windowMode = windowed;


bool paused = false;
bool debugMenu = true;

int fps = 0;
int frameCount = 0;
int Tprev = 0;
int Tnow = 0;
TDT Tdiff = 0;

int currentWindowWidth;
int currentWindowHeight;

GLfloat aspectRatio = DEFAULT_ASPECT; // Width / Height

int zoomoutLvl = ZOOMOUT_LVLS / 2;
float ZOOMOUT = 400.0f;


ChunkManager chunkManager;
Player player;


struct Environments
{
	static constexpr Environment test = { {0, 0}, {-ENV_GRAVITY, 0}, {1.0f, 1.0f} };

	static constexpr Environment midair   = { {0, 0}, {0, ENV_GRAVITY}, {1.0f, 1.0f} };
//	static constexpr Environment midair   = { {0, 0}, {0, 0}, {0, 0} };
	static constexpr Environment standing = { {0, 0}, {0, ENV_GRAVITY}, {300.0f, 1.0f} };
};



void LOOP(/*int value*/)
{
	// Debug
#if CONSOLE_LOG_CALLBACKS
	cout << "Timer called" << endl;
#endif

	// Time management
	Tnow = glutGet(GLUT_ELAPSED_TIME);
	Tdiff = (Tnow - Tprev) / 1000.0f;
	Tprev = Tnow;

	static int Tprevfps = 0;
	if (Tnow - Tprevfps > 1000.0f)
	{
		fps = frameCount * 1000.0f / (Tnow - Tprevfps);
		Tprevfps = Tnow;
		frameCount = 0;
	}
	
	chunkManager.updateTnow(Tnow);
	
//	glutTimerFunc(refreshMills, LOOP, 0);
	// /Time management

	player.env = Environments::midair;

	if (!paused)
	{
		TDT remainingTime = Tdiff;
		TDT sumExecutedTime = 0.0f;
		TDT currExecutedTime = 0.0f;

#if CONSOLE_LOG_MOVEMENT
		cout << "Starting movement:" << endl;
#endif
		
		int side = SIDE_NONE;
		
		do
		{
			Movement movement = player.tryMovement(remainingTime, side);
			Pos xd;
			auto collisionParams = chunkManager.checkCollision(movement, player); //std::tuple<Pos, Block, Block, int, float>

			float fractionExecuted = std::get<4>(collisionParams);

#if CONSOLE_LOG_MOVEMENT
			cout << "Fraction: " << fractionExecuted << endl;
#endif

			currExecutedTime = fractionExecuted * remainingTime;

			side = side | std::get<3>(collisionParams);

			player.doMovement(currExecutedTime, side);

			player.pos = std::get<0>(collisionParams);

/*
			if (side & SIDE_VERTICAL)
			{
				player.vel.X = 0;
				player.env.acc.X = 0;
			}

			if (side & SIDE_HORIZONTAL)
			{
				player.vel.Y = 0;
				player.env.acc.Y = 0;
			}
			*/
			if (side & SIDE_BOTTOM)
				player.env = Environments::standing;


			remainingTime -= currExecutedTime;

		}
		while (remainingTime > 0 && !(side & SIDE_VERTICAL && side & SIDE_HORIZONTAL));// && (player.vel.X != 0 || player.vel.Y != 0));// (fractionExecuted < 0.1);//  0.1 * Tdiff 

		player.env = Environments::midair;

#if CONSOLE_LOG_MOVEMENT
		cout << "Finished movement" << endl << endl << endl;
#endif
	}

//	paused = true;

	// Post re-paint request
//	glutPostRedisplay();
}

void idleCllbck()
{
	glutPostRedisplay();
}

void displayCllbck()
{
#if CONSOLE_LOG_CALLBACKS
	cout << "displayCllbck called" << endl;
#endif
	++frameCount;

	LOOP();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	Pos plrCntr = player.posCabs();
	Pos camPos = plrCntr;

	ZOOMOUT = MIN_ZOOMOUT*3;

	BlkCrd YminI = 0;
	BlkCrd YmaxI = CHUNK_HEIGHT;
	ChkCrd minChunk = 0;
	ChkCrd maxChunk = 0;

#if DYN_ZOOMOUT
	float Ymin = std::min(plrCntr.Y, double(WATER_LEVEL)) - MIN_ZOOMOUT;
	float Ymax = plrCntr.Y + MIN_ZOOMOUT;

	float MAX_ZOOMOUT = (Ymax - Ymin) * 0.5f;
	float maxCamY     = (Ymax + Ymin) * 0.5f;

	float pwrrat = std::pow(MAX_ZOOMOUT / MIN_ZOOMOUT, float(zoomoutLvl) / ZOOMOUT_LVLS);

	ZOOMOUT = MIN_ZOOMOUT * pwrrat;

	if (zoomoutLvl == 0 || MAX_ZOOMOUT <= MIN_ZOOMOUT)
		camPos.Y = plrCntr.Y;
	else
		if (aspectRatio <= DEFAULT_ASPECT)
			camPos.Y = plrCntr.Y + (maxCamY - plrCntr.Y) * (pwrrat - 1.0f) / (MAX_ZOOMOUT / MIN_ZOOMOUT - 1.0f);
		else
			camPos.Y = plrCntr.Y + (maxCamY - plrCntr.Y) * (pwrrat - 1.0f) / (MAX_ZOOMOUT / MIN_ZOOMOUT - 1.0f) * DEFAULT_ASPECT / aspectRatio;
#else
	float MAX_ZOOMOUT = CHUNK_HEIGHT * 0.5f;
	float pwrrat = std::pow(MAX_ZOOMOUT / MIN_ZOOMOUT, float(zoomoutLvl) / ZOOMOUT_LVLS);
	ZOOMOUT = MIN_ZOOMOUT * pwrrat;
#endif

	float scaleX = 1;
	float scaleY = 1;

	// W/H <= r  =>  W <= r*H
	if (aspectRatio <= DEFAULT_ASPECT)
	{
		scaleX = 1.0f / aspectRatio;
		scaleY = 1.0f;
	}
	else
	{
		scaleX = 1.0f / DEFAULT_ASPECT;
		scaleY = 1.0f / DEFAULT_ASPECT * aspectRatio;
	}

	glOrtho(-ZOOMOUT, ZOOMOUT, -ZOOMOUT, ZOOMOUT, 1.0f, -1.0f);
	glScalef(scaleX, scaleY, 1.0f);
	glTranslatef(-camPos.X, -camPos.Y, 0.0f);

	minChunk = floor((camPos.X - ZOOMOUT / scaleX) / CHUNK_WIDTH);
	maxChunk = floor((camPos.X + ZOOMOUT / scaleX) / CHUNK_WIDTH);

	YminI = std::floor(camPos.Y - ZOOMOUT / scaleY);
	YmaxI = std:: ceil(camPos.Y + ZOOMOUT / scaleY);

#if REQUIRE_ENTIRE_CHUNK
	minChunk += 1;
	maxChunk -= 1;
#endif
#if REQUIRE_ENTIRE_BLOCK
	YminI += 1;
	YmaxI -= 1;
#endif

	// PREPARE 4 DRAWING
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (ChkCrd ch = minChunk; ch <= maxChunk; ++ch)
		chunkManager.getChunk(ch).draw(YminI, YmaxI);

	player.draw();

	// DEBUG MENU
	if (debugMenu)
	{
		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, {
			"FPS: " + to_string(fps),
			"Chunks: " + to_string(chunkManager.count()),
		}, 0, DEBUG_FONT_H);

		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, {
			"Pos: " + to_string(player.pos.X) + ", " + to_string(player.pos.Y),
			"Vel: " + to_string(player.vel.X) + ", " + to_string(player.vel.Y),
			"Acc: " + to_string(player.acc.X) + ", " + to_string(player.acc.Y)
		}, 1, DEBUG_FONT_H);

		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, {
			"Cam: " + to_string(camPos.X) + ", " + to_string(camPos.Y),
			"Max Zoom: " + to_string(MAX_ZOOMOUT),
			"Cur Zoom: " + to_string(ZOOMOUT),
			"Zoom Lvl: " + to_string(zoomoutLvl),
			"Ymin: " + to_string(YminI),
			"Ymax: " + to_string(YmaxI)
		}, 2, DEBUG_FONT_H);

		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, {
			"Tnow: " + to_string(Tnow),
			"Tdiff: " + to_string(Tdiff)
		}, 3, DEBUG_FONT_H);
	}
	
	// DRAW
#if DOUBLE_BUFFERED
	glutSwapBuffers();
#else
	glFlush();
#endif
}

/* Handler for window re-size event. Called back when the window first appears and whenever the window is re-sized with its new width and height */
void reshapeCllbck(GLsizei width, GLsizei height)
{
#if CONSOLE_LOG_CALLBACKS
	cout << "reshapeCllbck called" << endl;
#endif

	if (height <= 0)
		height = 1;
	if (width <= 0)
		height = 1;

	currentWindowWidth = width;
	currentWindowHeight = height;

	aspectRatio = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);
}

void applyWindowMode(int mode)
{
	switch (mode)
	{
	case windowed:
		// Restore saved window parameters
		glutReshapeWindow(windowedWidth, windowedHeight);
		glutPositionWindow(windowedPosX, windowedPosY);
		break;

	case borderless:
		// Save window parameters for restoring
		windowedPosX = glutGet(GLUT_WINDOW_X);
		windowedPosY = glutGet(GLUT_WINDOW_Y);
		windowedWidth = glutGet(GLUT_WINDOW_WIDTH);
		windowedHeight = glutGet(GLUT_WINDOW_HEIGHT);
		// Switch into full screen
		glutReshapeWindow(GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
		glutPositionWindow(0, 0);
		break;

	case fullscreen:
		// Make app fullscreen
		glutFullScreen();
		break;

	default:
		throw new invalid_argument("Wrong Window mode!");
		break;
	}
}

void specialKeyEvent(int key, int x, int y)
{
#if CONSOLE_LOG_EVENTS
	cout << "specialKeyEvent called:" << key << endl;
#endif
	switch (key)
	{
	case GLUT_KEY_F3:    // F3: Toggle debug menu
		debugMenu = !debugMenu;
		break;

	case GLUT_KEY_F11:    // F11: Toggle between full-screen and windowed mode
		switch (windowMode)
		{
		case windowed:
			windowMode = borderless;
			break;
		case borderless:
			windowMode = fullscreen;
			break;
		case fullscreen:
			windowMode = windowed;
			break;
		default:
			throw new invalid_argument("Wrong Window mode!");
			break;
		}
		applyWindowMode(windowMode);
		break;

	case GLUT_KEY_UP:
		player.pos.Y += 0.5f;
		break;
	case GLUT_KEY_DOWN:
		player.pos.Y -= 0.5f;
		break;
	case GLUT_KEY_LEFT:
		player.pos.X -= 0.5f;
		break;
	case GLUT_KEY_RIGHT:
		player.pos.X += 0.5f;
		break;

	default:
		break;
	}
}

void regularKeyEvent(unsigned char key, int x, int y)
{
#if CONSOLE_LOG_EVENTS
	cout << "regularKeyEvent called: " << key << ' ' << (uint)key << endl;
#endif
	switch (toupper(key))
	{
	case 27: // ESC key
		glutDestroyWindow(windowID);
		exit(0);
		break;
/*/
	case 'W':
		player.vel.X = 0.0f;
		player.vel.Y = 5.0f;
		break;
	case 'S':
		player.vel.X = 0.0f;
		player.vel.Y = -5.0f;
		break;
	case 'A':
		player.vel.X = -5.0f;
		player.vel.Y = 0.0f;
		break;
	case 'D':
		player.vel.X = 5.0f;
		player.vel.Y = 0.0f;
		break;
	case ' ':
//		player.vel.Y = 5.0f;
		paused = !paused;
		break;
//*/
//*/
	case 'W':
		player.vel.Y += 40.0f;
		break;
	case 'S':
		player.vel.Y += -40.0f;
		break;
	case 'A':
		player.vel.X += -40.0f * 4;
		break;
	case 'D':
		player.vel.X += 40.0f * 4;
		break;
	case ' ':
		paused = !paused;
		break;

	case 'P':
		player.vel.X = 0.0f;
		player.vel.Y = 0.0f;
		break;

	case '-':
	case '_':
		if (zoomoutLvl < ZOOMOUT_LVLS)
			++zoomoutLvl;
		break;
	case '=':
	case '+':
		if (zoomoutLvl > 0)
			--zoomoutLvl;
		break;
//*/
	}
}

void mouseEvent(int button, int state, int x, int y)
{
#if CONSOLE_LOG_EVENTS
	cout << "mouseEvent called" << endl;
#endif
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
//		paused = !paused;         // Toggle state
	}
}


int main(int argc, char** argv)
{
	std::ios_base::sync_with_stdio(false);

//	player.pos = { 0.0f, WATER_LEVEL };
	player.pos = { 0.0f, 127.0f };


	glutInit(&argc, argv);            // Initialize GLUT

	glutInitWindowSize(windowedWidth, windowedHeight);  // Initial window width and height
	glutInitWindowPosition(windowedPosX, windowedPosY); // Initial window top-left corner (x, y)
	windowID = glutCreateWindow(title);      // Create window with given title
	applyWindowMode(windowMode);             // Put into window or borderless or full screen

	glutDisplayFunc(displayCllbck);     // Register callback handler for window re-draw
	glutReshapeFunc(reshapeCllbck);     // Register callback handler for window re-shape
	glutIdleFunc(idleCllbck);

//	glutTimerFunc(0, LOOP, 0);   // First timer called immediately

	glutSpecialFunc(specialKeyEvent); // Register callback handler for special-key event
	glutKeyboardFunc(regularKeyEvent); // Register callback handler for regular-key event
	glutMouseFunc(mouseEvent);   // Register callback handler for mouse event

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.678f, 0.847f, 0.902f, 1.0f); // 67.8% red, 84.7% green and 90.2
	glClearDepth(Z_VAL_BACKGROUND);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_FLAT);

#if DOUBLE_BUFFERED
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // Enable double buffered mode
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH); // Disable double buffered mode
#endif

	// Unit Tests
	{
		vector<Pos> xd = interpolate(player.posBLrel(), player.posBRrel(), 3);
		cout << "Pos interpolation test: " << endl;
		for (Pos& x : xd)
			x.cout();

		cout << "Pos*k test: " << endl;
		Pos dd = player.pos;
		dd *= 2.5f;
		dd.cout();

		cout << "Collisions test: " << endl;
		vector<BlkCrd> test = possibleBlocksCollisions(player.pos.Y, player.pos.Y + 1);
		for (BlkCrd& t : test)
			cout << t << '\t';
		cout << endl;

		cout << "Perlin test: " << endl;
		const siv::PerlinNoise perlin;
		for (size_t i = 0; i <= 10; ++i)
			cout << perlin.octave1D(float(i) / 100, 1) << endl;
		const siv::PerlinNoise perlin2;
		for (size_t i = 0; i <= 10; ++i)
			cout << perlin2.octave1D(float(i) / 100, 1) << endl;

		cout << "Noise generator test:" << endl;
		std::vector<float> noiseOutput(3 * 3 * 2);
		FastNoise::SmartNode<> fnGenerator = FastNoise::NewFromEncodedNodeTree("CgABAAAAAAAAAAAAAIA/");
		fnGenerator->GenUniformGrid2D(noiseOutput.data(), 0, 0, 4, 2, 0.2f, 1337);
		int index = 0;

		for (const float xd : noiseOutput)
		{
			cout << xd << endl;
			if (xd == 0)
				break;
		}

		std::cout << "Powers test:" << std::endl;
		for (int i = 0; i <= 5; ++i)
			std::cout << std::pow(2.0, double(i) / 5) << " ";
		std::cout << std::endl;

		std::cout << "Name to block test:" << std::endl;;
		Block xd2 = BlockN::bedrock;
		std::cout << Bl_t(xd2.ID) << endl;
	}
	

//	system("pause");
//	return 0;

	Tnow = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop(); // Enter event-processing loop
	
	return 0;
}