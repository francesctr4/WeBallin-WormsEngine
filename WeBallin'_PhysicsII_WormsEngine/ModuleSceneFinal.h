#pragma once

#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"

class ModuleSceneFinal;

class ModuleSceneFinal : public Module
{
public:
	ModuleSceneFinal(Application* app, bool start_enabled = true);
	~ModuleSceneFinal();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	Timer timer;

	SDL_Texture* finalScreenLose;
	uint loseFx;

	SDL_Texture* finalScreenWin;
	uint winFx;

};