#pragma once

#include "Module.h"
#include "Animation.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Timer.h"

class ModuleScene;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	Timer endGame;
	bool endGame_bool;

	SDL_Texture* levelMap;

};