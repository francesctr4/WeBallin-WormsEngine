#include "Globals.h"
#include "Application.h"
#include "ModuleSceneFinal.h"

ModuleSceneFinal::ModuleSceneFinal(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleSceneFinal::~ModuleSceneFinal()
{}

// Load assets
bool ModuleSceneFinal::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	App->audio->PlayMusic("Assets/Audio/Music/NoMusic.ogg",0);

	timer.Start();

	finalScreenLose = App->textures->Load("Assets/Textures/YouLose.png");
	finalScreenWin = App->textures->Load("Assets/Textures/YouWin.png");

	loseFx = App->audio->LoadFx("Assets/Audio/Fx/LoseFx.wav");
	winFx = App->audio->LoadFx("Assets/Audio/Fx/WinFx.wav");

	return ret;
}

// Load assets
bool ModuleSceneFinal::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneFinal::Update()
{

	if (App->player->winCondition) {

		App->renderer->Blit(finalScreenWin, 0, 0);
		App->audio->PlayFx(winFx);

	}
	else {

		App->renderer->Blit(finalScreenLose, 0, 0);
		App->audio->PlayFx(loseFx);

	}

	return UPDATE_CONTINUE;
}