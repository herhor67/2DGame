
#

#include <iostream>

#include "GL/glut.h"

#include "Entity.h"


Entity::Entity(float _mass, float _height, float _width, EntCrd _Xpos, EntCrd _Ypos, float _Xvel, float _Yvel, float _Xacc, float _Yacc) : mass(_mass), height(_height), width(_width), pos({ _Xpos, _Ypos }), vel({ _Xvel, _Yvel }), acc({ _Xacc, _Yacc }) {}

Entity::~Entity() {}


void Entity::draw() const
{
	glMatrixMode(GL_MODELVIEW);     // To operate on Model-View matrix
	glLoadIdentity();               // Reset the model-view matrix

	glPushMatrix();                     // Save model-view matrix setting
	glTranslatef(pos.X, pos.Y, Z_VAL_ENTITY);    // Translate

#if DRAW_FACES
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);                  // Each set of 4 vertices form a quad

		glVertex2f(boxLrel(), boxBrel());
		glVertex2f(boxRrel(), boxBrel());
		glVertex2f(boxRrel(), boxrel_X());
		glVertex2f(boxLrel(), boxrel_X());

		glEnd();
	}
#endif
#if DRAW_BORDERS
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_QUADS);                  // Each set of 4 vertices form a quad
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

		glVertex2f(boxLrel(), boxBrel());
		glVertex2f(boxRrel(), boxBrel());
		glVertex2f(boxRrel(), boxrel_X());
		glVertex2f(boxLrel(), boxrel_X());

		glEnd();
	}
#endif
	glPopMatrix();                      // Restore the model-view matrix
}

Movement Entity::doMovement(TDT Tdiff, int side)
{
	Pos oldPos = pos;
	getNewPos(Tdiff, side, true);

	return std::make_pair(oldPos, pos);
}

Movement Entity::tryMovement(TDT Tdiff, int side)
{
	return std::make_pair(pos, getNewPos(Tdiff, side));
}

Pos Entity::getNewPos(TDT Tdiff, int side, bool write)
{
	Acc A = acc;
	Vel V = vel;
	Pos P = pos;

#if ENV_INTERACTION
	Frc F;
	F.X = -sgn(vel.X) * pow(env.vel.X - vel.X, 2) * width * height * env.res.X;
	F.Y = -sgn(vel.Y) * pow(env.vel.Y - vel.Y, 2) * width * width  * env.res.Y;
	A.X = F.X / mass;
	A.Y = F.Y / mass;

	V.X += env.acc.X * Tdiff;
	V.Y += env.acc.Y * Tdiff;
#endif
	
	V.X += A.X * Tdiff;
	V.Y += A.Y * Tdiff;

	P.X += V.X * Tdiff;
	P.Y += V.Y * Tdiff;

	if (side & SIDE_VERTICAL)
	{
		V.X = 0;
		P.X = pos.X;
	}

	if (side & SIDE_HORIZONTAL)
	{
		V.Y = 0;
		P.Y = pos.Y;
	}

	if (write)
	{
		acc = A;
		vel = V;
		pos = P;
	}
	//	std::cout << "TryPos: X: " << newPos.X << " Y: " << newPos.Y << std::endl;

	return P;
}


EntCrd Entity::boxrel_X() const
{
	return height;
}
EntCrd Entity::boxBrel() const
{
	return 0;
}
EntCrd Entity::boxRrel() const
{
	return width * 0.5;
}
EntCrd Entity::boxLrel() const
{
	return -width * 0.5;
}

EntCrd Entity::boxTabs() const
{
	return pos.Y + boxrel_X();
}
EntCrd Entity::boxBabs() const
{
	return pos.Y + boxBrel();
}
EntCrd Entity::boxRabs() const
{
	return pos.X + boxRrel();
}
EntCrd Entity::boxLabs() const
{
	return pos.X + boxLrel();
}

Pos Entity::posTRrel() const
{
	return { boxRrel(), boxrel_X() };
}
Pos Entity::posTLrel() const
{
	return { boxLrel(), boxrel_X() };
}
Pos Entity::posBLrel() const
{
	return { boxLrel(), boxBrel() };
}
Pos Entity::posBRrel() const
{
	return { boxRrel(), boxBrel() };
}

Pos Entity::posTRabs() const
{
	return pos + posTRrel();
}
Pos Entity::posTLabs() const
{
	return pos + posTLrel();
}
Pos Entity::posBLabs() const
{
	return pos + posBLrel();
}
Pos Entity::posBRabs() const
{
	return pos + posBRrel();
}

Pos Entity::posCrel() const
{
	return { (boxRrel() + boxLrel()) * (EntCrd)0.5, (boxrel_X() + boxBrel()) * (EntCrd)0.5 };
}
Pos Entity::posCabs() const
{
	return pos + posCrel();
}

Player::Player() : Entity(75.0f, 2.0f, 1.0f)//Entity(75.0f, 1.8f, 0.5f)
{
}

Player::~Player()
{
}

void Player::draw()
{
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	Entity::draw();
}