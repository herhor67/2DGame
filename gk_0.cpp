#define NOMINMAX 1

#include <iostream>
#include <string>
#include "Filesystem.h"
#include "Perlin.h"

#define GLUT_DISABLE_ATEXIT_HACK
#define GLUT_API_VERSION 4
#include "GL/glut.h"

#include "ChunkManager.h"
#include "Entity.h"

#include "typedefs.h"
#include "defines.h"
#include "functions.h"

using namespace std;


/*
// Drawing (display) routine.
void drawScene(void)
{
	// Clear screen to background color.
	glClear(GL_COLOR_BUFFER_BIT);

	//ToDo -- drawing

	// Flush created objects to the screen, i.e., force rendering.
	glFlush();
}

// Initialization routine.
void setup(void)
{
	// Set background (or clearing) color.
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	// Set viewport size to be entire OpenGL window.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// Set matrix mode to projection.
	glMatrixMode(GL_PROJECTION);

	// Clear current projection matrix to identity.
	glLoadIdentity();

	// Specify the orthographic (or perpendicular) projection,
	// i.e., define the viewing box.
	glOrtho(-10.0, 10.0, -10.0, 10.0,-10.0, 10.0);
	//glFrustum(-10.0, 10.0, -10.0, 10.0, 5.0, 100.0);

	// Set matrix mode to modelview.
	glMatrixMode(GL_MODELVIEW);

	// Clear current modelview matrix to identity.
	glLoadIdentity();
}

void animate(int value) {


	// Note that glutTimerFunc() calls animate() *once* after the
	// specified msecs. Therefore, to make repeated calls, animate()
	// must call itself again with glutTimerFunc() as below.
	glutTimerFunc(100, animate, 1);
	glutPostRedisplay();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	//ToDo

}

// Mouse callback routine.
void mouseControl(int button, int state, int x, int y)
{

  //ToDo
}

// Main routine: defines window properties, creates window,
// registers callback routines and begins processing.
int main(int argc, char **argv)
{
	// Initialize GLUT.
	glutInit(&argc, argv);

	// Set display mode as single-buffered and RGB color.
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	// Set OpenGL window size.
	glutInitWindowSize(500, 500);

	// Set position of OpenGL window upper-left corner.
	glutInitWindowPosition(100, 100);

	// Create OpenGL window with title.
	glutCreateWindow("Laboratorium GK: 00_OGL");

	// Initialize.
	setup();

	// Register display routine.
	glutDisplayFunc(drawScene);

	// Register reshape routine.
	glutReshapeFunc(resize);


	// Register the mouse and keyboard callback function.
	glutMouseFunc(mouseControl);
	glutKeyboardFunc(keyInput);

	//Animation
	glutTimerFunc(5, animate, 1);

	// Begin processing.
	glutMainLoop();

	return 0;
}
*/


char title[] = "The Game";  // Windowed mode's title
int windowedWidth = 1024*3/2;     // Windowed mode's width
int windowedHeight = 576*3/2;     // Windowed mode's height
int windowedPosX = 1000;      // Windowed mode's top-left corner x
int windowedPosY = 200;      // Windowed mode's top-left corner y
int windowID;

int refreshMills = 1000 / 60 / 2; // refresh interval in milliseconds
//int refreshMills = 100;

enum ScreenModes { windowed = -1, borderless, fullscreen };
int windowMode = windowed; // Full-screen or windowed mode?


bool paused = true;
bool debugMenu = true;

int fps = 0;
int frameCount = 0;
int Tprev = 0;
int Tnow = 0;
TDT Tdiff = 0;

int currentWindowWidth;
int currentWindowHeight;

ChunkManager chunkManager;
Player player;

GLfloat aspectRatio = 1.0f;


struct Environments
{
	static constexpr Environment test = { {0, 0}, {-ENV_GRAVITY, 0}, {1.0f, 1.0f} };

//	static constexpr Environment midair   = { {0, 0}, {0, ENV_GRAVITY}, {1.0f, 1.0f} };
	static constexpr Environment midair   = { {0, 0}, {0, 0}, {0, 0} };
	static constexpr Environment standing = { {0, 0}, {0, ENV_GRAVITY}, {300.0f, 1.0f} };
};

//Environments envs;


/* Called back when timer expired */
void LOOP(/*int value*/)
{
	// Debug
#if CONSOLE_LOG_CALLBACKS
	cout << "Timer called" << endl;
#endif

	// Time management
	Tnow = glutGet(GLUT_ELAPSED_TIME);
	Tdiff = (Tnow - Tprev) / 1000.0;
	Tprev = Tnow;

	static int Tprevfps = 0;
	if (Tnow - Tprevfps > 1000.0)
	{
		fps = frameCount * 1000.0 / (Tnow - Tprevfps);
		Tprevfps = Tnow;
		frameCount = 0;
	}
	
	chunkManager.updateTnow(Tnow);
	
//	glutTimerFunc(refreshMills, LOOP, 0);
	// /Time management

	player.env = Environments::midair;

	if (!paused)
	{
		// we should really make some movement here...

		TDT remainingTime = Tdiff;
		TDT sumExecutedTime = 0.0f;
		TDT currExecutedTime = 0.0f;
//		float fractionExecuted;
		cout << "Starting movement:" << endl;
		
		int side = SIDE_NONE;
		
		do
		{
			Movement movement = player.tryMovement(remainingTime, side);
			Pos xd;
			auto collisionParams = chunkManager.checkCollision(movement, player); //std::tuple<Pos, Block, Block, int, float>

			float fractionExecuted = std::get<4>(collisionParams);
			cout << "Fraction: " << fractionExecuted << endl;

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

		cout << "Finished movement" << endl << endl << endl;
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

	// I should make some players first perhaps
	// now just let's load and draw chunks at camera





	// Set the aspectRatio ratio of the clipping area to match the viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int minChunk, maxChunk;
	// W/H > 1  =>  W > H
	if (aspectRatio <= DEFAULT_ASPECT) // set the height from -1 to 1, with larger width
	{
		glOrtho(-ZOOMOUT * aspectRatio + player.pos.X, ZOOMOUT * aspectRatio + player.pos.X, -ZOOMOUT + player.pos.Y, ZOOMOUT + player.pos.Y, 1.0f, -1.0f);
		minChunk = floor((-ZOOMOUT * aspectRatio + player.pos.X) / CHUNK_WIDTH);
		maxChunk = floor(( ZOOMOUT * aspectRatio + player.pos.X) / CHUNK_WIDTH);
	}
	else // set the width to -1 to 1, with larger height
	{
		glOrtho(-ZOOMOUT + player.pos.X, ZOOMOUT + player.pos.X, -ZOOMOUT / aspectRatio + player.pos.Y, ZOOMOUT / aspectRatio + player.pos.Y, 1.0f, -1.0f);
		minChunk = floor((-ZOOMOUT + player.pos.X) / CHUNK_WIDTH);
		maxChunk = floor(( ZOOMOUT + player.pos.X) / CHUNK_WIDTH);
	}

#if REQUIRE_ENTIRE
	minChunk += 1;
	maxChunk -= 1;
#endif

	// PREPARE 4 DRAWING
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// TERRAIN
	for (int ch = minChunk; ch <= maxChunk; ++ch)
		chunkManager.getChunk(ch).draw();

//	chunkManager.getChunk(0).save();

	player.draw();

	// DEBUG MENU
	if (debugMenu)
	{
		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, { "FPS: " + to_string(fps), "Chunks: " + to_string(chunkManager.count()) }, 0, DEBUG_FONT_H);
		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, { "Pos: " + to_string(player.pos.X) + ", " + to_string(player.pos.Y) }, 1, DEBUG_FONT_H);
		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, { "Vel: " + to_string(player.vel.X) + ", " + to_string(player.vel.Y), "Acc: " + to_string(player.acc.X) + ", " + to_string(player.acc.Y) }, 2, DEBUG_FONT_H);
		drawStringOnWindow(10, 10, Z_VAL_MENU, DEBUG_FONT, { "Tnow: " + to_string(Tnow), "Tdiff: " + to_string(Tdiff) }, 3, DEBUG_FONT_H);
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

	currentWindowWidth = width;
	currentWindowHeight = height;


	// GLsizei for non-negative integer
	// Compute aspectRatio ratio of the new window
	if (height == 0)
		height = 1;                // To prevent divide by 0

	aspectRatio = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
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
		player.vel.Y = 50.0f;
		break;
	case 'S':
		player.vel.Y = -30.0f;
		break;
	case 'A':
		player.vel.X = -40.0f*4;
		break;
	case 'D':
		player.vel.X = 40.0f * 4;
		break;
	case ' ':
		paused = !paused;
		break;

	case 'P':
		player.vel.X = 0.0f;
		player.vel.Y = 0.0f;
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

	player.pos = { 0.0f, 50.0f };
//	player.pos = { 0.0f, 10.0f };

/*
	{
		Chunk ch(0);
		ch.flatFill();
		ch.save();
	}
	{
		Chunk ch(-1);
		ch.flatFill();
		ch.save();
	}
	//*/

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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.678f, 0.847f, 0.902f, 1.0f); // 67.8% red, 84.7% green and 90.2
	glClearDepth(Z_VAL_BACKGROUND);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if DOUBLE_BUFFERED
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // Enable double buffered mode
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH); // Disable double buffered mode
#endif

	Tnow = glutGet(GLUT_ELAPSED_TIME);


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
		cout << perlin.octave1D(float(i)/100, 1) << endl;
	const siv::PerlinNoise perlin2;
	for (size_t i = 0; i <= 10; ++i)
		cout << perlin2.octave1D(float(i) / 100, 1) << endl;

	glutMainLoop();               // Enter event-processing loop
	
	return 0;
}