#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "PhysBody.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	PhysBody ball = PhysBall(10.0f, 1.0f, 1.0f, 0.4f, 1.2f, 10.0f, 0.9f, 0.8f, 2.0f, (App->physics->ground.y + App->physics->ground.h) + 2.0f, 0, 0, 255, 255, 255);

	// Add ball to the collection
	App->physics->balls.emplace_back(ball);

	proyectile_vx = 10.0f;
	proyectile_vy = 10.0f;

	theta = 0;

	shootsLimit = 8;

	currentProyectileMode = ProyectileModes::NORMAL;

	shootfx = App->audio->LoadFx("Assets/Audio/Fx/ThrowSound.wav");

	winCondition = false;

	wormTextureRight = App->textures->Load("Assets/Textures/WormTextureRight.png");

	wormTextureLeft = App->textures->Load("Assets/Textures/WormTextureLeft.png");

	playerDirection = false;

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) {

		theta = 1 * M_PI / 180;

	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) {

		theta = -1 * M_PI / 180;

	}
	
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_UP) == KEY_IDLE) {

		theta = 0;

	}

	proyectile_vx = proyectile_vx * cos(theta) - proyectile_vy * sin(theta);
	proyectile_vy = proyectile_vx * sin(theta) + proyectile_vy * cos(theta);

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN) {

		if (currentProyectileMode == ProyectileModes::NORMAL) currentProyectileMode = ProyectileModes::LIMITLESS;
		else currentProyectileMode = ProyectileModes::NORMAL;

	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {

		switch (currentProyectileMode)
		{
			case ProyectileModes::NORMAL:

				if (shootsLimit > 0) {

					App->audio->PlayFx(shootfx);

					PhysBody proyectile = PhysProyectile(5.0f, 1.0f, 0.2f, 0.4f, 1.2f, 10.0f, 0.9f, 0.8f, App->physics->balls[0].x, App->physics->balls[0].y, proyectile_vx, proyectile_vy, 255, 0, 0);

					App->physics->proyectiles.emplace_back(proyectile);

				}

				if (shootsLimit != 0) shootsLimit--;

				break;


			case ProyectileModes::LIMITLESS:

				App->audio->PlayFx(shootfx);

				PhysBody proyectile = PhysProyectile(5.0f, 1.0f, 0.2f, 0.4f, 1.2f, 10.0f, 0.9f, 0.8f, App->physics->balls[0].x, App->physics->balls[0].y, proyectile_vx, proyectile_vy, 255, 0, 0);

				App->physics->proyectiles.emplace_back(proyectile);

				break;
		}

	}

	App->renderer->BlitText("Remaining Proyectiles:", 650, 50, 250, 30, App->physics->textColor);

	switch (currentProyectileMode)
	{
		case ProyectileModes::NORMAL:

			App->renderer->BlitText(std::to_string((int)(shootsLimit)), 910, 52, 20, 30, App->physics->textColorYellow);

			break;


		case ProyectileModes::LIMITLESS:

			App->renderer->BlitText("Inf.", 910, 52, 40, 30, App->physics->textColorRed);

			break;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) playerDirection = false;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) playerDirection = true;

	if (!playerDirection) App->renderer->Blit(wormTextureRight,METERS_TO_PIXELS(App->physics->balls[0].x) - 45, SCREEN_HEIGHT - METERS_TO_PIXELS(App->physics->balls[0].y) - 45);
	else App->renderer->Blit(wormTextureLeft, METERS_TO_PIXELS(App->physics->balls[0].x) - 45, SCREEN_HEIGHT - METERS_TO_PIXELS(App->physics->balls[0].y) - 45);

	App->renderer->DrawLine(METERS_TO_PIXELS(App->physics->balls[0].x), SCREEN_HEIGHT - METERS_TO_PIXELS(App->physics->balls[0].y), METERS_TO_PIXELS(App->physics->balls[0].x + proyectile_vx * 4), SCREEN_HEIGHT - METERS_TO_PIXELS(App->physics->balls[0].y + proyectile_vy * 4), 0, 255, 0);

	return UPDATE_CONTINUE;
}



