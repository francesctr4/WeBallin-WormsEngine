#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleScene::~ModuleScene()
{}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	App->audio->PlayMusic("Assets/Audio/Music/WormsTheme.ogg",0);

	endGame_bool = false;

	levelMap = App->textures->Load("Assets/Textures/WormsLevel.png");

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleScene::Update()
{

	if ((App->player->shootsLimit == 0 || App->physics->targets.empty()) && !endGame_bool) {

		endGame.Start();

		endGame_bool = true;

	}

	if (endGame_bool && endGame.ReadMSec() > 1500) {

		App->scene_final->Enable();

		this->Disable();
		App->physics->Disable();
		App->player->Disable();

	}

	App->renderer->Blit(levelMap,0,0);

	return UPDATE_CONTINUE;
}