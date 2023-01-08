#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	initialScreen = App->textures->Load("Assets/Textures/WormsInitialScreen.png");

	App->audio->PlayMusic("Assets/Audio/Music/IntroTheme.ogg",0);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{

	App->renderer->Blit(initialScreen, 0, 0);

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {

		App->scene->Enable();
		App->physics->Enable();
		App->player->Enable();

		this->Disable();

	}

	return UPDATE_CONTINUE;
}
