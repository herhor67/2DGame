#pragma once

#include <utility>

#include "typedefs.h"
#include "defines.h"
#include "functions.h"

#include "Environment.h"



class Entity
{
public:
	float mass = 0;
	float height = 0;
	float width = 0;
	Pos pos;
	Vel vel;
	Acc acc;

	Environment env;

	Entity(float = 0, float = 0, float = 0, EntCrd = 0, EntCrd = 0, float = 0, float = 0, float = 0, float = 0);
	virtual ~Entity() = 0;
	Movement doMovement(TDT, int);
	Movement tryMovement(TDT, int);
	Pos getNewPos(TDT, int, bool=false);
	virtual void draw() const;
	
	EntCrd boxTrel() const;
	EntCrd boxBrel() const;
	EntCrd boxRrel() const;
	EntCrd boxLrel() const;
	
	EntCrd boxTabs() const;
	EntCrd boxBabs() const;
	EntCrd boxRabs() const;
	EntCrd boxLabs() const;
	
	Pos posTRrel() const;
	Pos posTLrel() const;
	Pos posBLrel() const;
	Pos posBRrel() const;

	Pos posTRabs() const;
	Pos posTLabs() const;
	Pos posBLabs() const;
	Pos posBRabs() const;

	/*
	void setPos(Crd, Crd);
	void setVel(float, float);
	void setAcc(float, float);
	*/
	/*
	void setPosX(Crd);
	void setPosY(Crd);
	void setVelX(Crd);
	void setVelY(Crd);
	void setAccX(Crd);
	void setAccY(Crd);
	//*/
};



class Player : public Entity
{
public:
	Player();
	~Player();
	void draw();
};