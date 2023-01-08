#pragma once
#include "Module.h"
#include "Animation.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics.h"
#include "math.h"

class PhysBody;

enum class ProyectileModes {

	NORMAL,
	LIMITLESS

};

struct Object
{
	SDL_Texture* graphic;
	PhysBody* body;
	uint fx;

	Object() : graphic(NULL), body(NULL) {}

};

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	PhysBall ball;

	float proyectile_vx;
	float proyectile_vy;
	float theta;

	int shootsLimit;

	bool winCondition;

	ProyectileModes currentProyectileMode;

	uint shootfx;

	SDL_Texture* wormTextureRight;
	SDL_Texture* wormTextureLeft;

	bool playerDirection;

};