#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"
#include <cmath>

#define EARTH_GRAVITY -9.807f // [ m / s^2 ]
#define MOON_GRAVITY -1.625f // [ m / s^2 ]

// TODO 1: Include Box 2 header and library

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
	frames = 0;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	timer = Timer();

	FPSiterator = 4;

	RCiterator = 4;

	Impulseiterator = 3;

	Winditerator = 1;

	maxFrameDuration = FPSarray[FPSiterator] * 1000;

	timer.Start();
	startupTime.Start();
	lastSecFrameTime.Start();

	// Create ground

	ground = Ground(0.0f, 0.0f, 8.5f, 5.5f);
	grounds.emplace_back(ground);

	ground2 = Ground(1.7f, 20.7f, 2.6f, 7.7f);
	grounds.emplace_back(ground2);

	ground3 = Ground(10.7f, 24.0f, 6.0f, 2.0f);
	grounds.emplace_back(ground3);

	ground4 = Ground(20.3f, 30.1f, 4.7f, 1.7f);
	grounds.emplace_back(ground4);

	ground5 = Ground(15.7f, 13.3f, 6.0f, 3.0f);
	grounds.emplace_back(ground5);

	ground6 = Ground(28.4f, 11.0f, 4.7f, 2.7f);
	grounds.emplace_back(ground6);

	ground7 = Ground(40.4f, 21.3f, 2.5f, 11.5f);
	grounds.emplace_back(ground7);

	ground8 = Ground(49.3f, 2.3f, 1.9f, 30.5f);
	grounds.emplace_back(ground8);

	ground9 = Ground(25.1f, 0.0f, 14.5f, 2.8f);
	grounds.emplace_back(ground9);

	// Out of bounds

	ground10 = Ground(-3.5f, 0.0f, 2.0f, 42.0f);
	grounds.emplace_back(ground10);

	ground11 = Ground(53.5f, 0.0f, 2.0f, 42.0f);
	grounds.emplace_back(ground11);

	ground12 = Ground(0.0f, 42.0f, 55.0f, 2.0f);
	grounds.emplace_back(ground12);

	// Create targets

	target = Target(13.05f, 26.4f, 1.5f, 1.5f, 255, 0, 0, 250, 188);
	targets.emplace_back(target);

	target2 = Target(18.0f, 11.2f, 1.5f, 1.5f, 255, 0, 0, 349, 492);
	targets.emplace_back(target2);

	target3 = Target(31.65f, 3.7f, 1.5f, 1.5f, 255, 0, 0, 622, 643);
	targets.emplace_back(target3);

	target4 = Target(37.95f, 15.75f, 1.5f, 1.5f, 255, 0, 0, 747, 402);
	targets.emplace_back(target4);

	target5 = Target(45.4f, 28.6f, 1.5f, 1.5f, 255, 0, 0, 896, 145);
	targets.emplace_back(target5);

	// Create Water

	water = Water(8.6f, 0.0f, 16.5f, 2.4f, 50.0f, -1.0f, 0.0f);
	waters.emplace_back(water);

	water2 = Water(39.6f, 0.0f, 12.0f, 2.4f, 50.0f, -1.0f, 0.0f);
	waters.emplace_back(water2);

	water3 = Water(42.9f, 2.3f, 6.4f, 29.3f, 50.0f, -1.0f, 0.0f);
	waters.emplace_back(water3);

	// Create atmosphere

	atmosphere = Atmosphere();
	atmosphere.windx = WindarrayX[Winditerator]; // [m/s]
	atmosphere.windy = WindarrayY[Winditerator]; // [m/s]
	atmosphere.density = 1.0f; // [kg/m^3]

	// Create spring

	spring = Spring();
	spring.spring_constant = 10.0f; // [N/m]
	spring.displacementx = 0.0f; // [m]
	spring.displacementy = 0.5f; // [m]

	gravityIterator = false;

	enableImpulsive = true;
	enableGravity = true;
	enableLift = true;
	enableHydroDrag = true;
	enableBuoyancy = true;
	enableElastic = true;

	showTechnicalDebug = false;

	actualPhysicsIntegrationScheme = PhysicsIntegrationSchemes::VELOCITY_VERLET;

	actualPlayerControlScheme = PlayerControlSchemes::FORCE;

	actualCollisionSolverScheme = CollisionSolverSchemes::TELEPORT;

	score = 0;

	targetfx = App->audio->LoadFx("Assets/Audio/Fx/HitSound.wav");

	diana = App->textures->Load("Assets/Textures/Diana.png");

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	frameTime.Start();

	atmosphere.windx = WindarrayX[Winditerator];
	atmosphere.windy = WindarrayY[Winditerator];

	// Process all balls in the scenario
	for (auto& ball : balls)
	{
		// Skip ball if physics not enabled
		if (!ball.physics_enabled)
		{
			continue;
		}

		// Step #0: Clear old values
		// ----------------------------------------------------------------------------------------
		
		// Reset total acceleration and total accumulated force of the ball
		ball.fx = ball.fy = 0.0f;
		ball.ax = ball.ay = 0.0f;

		// Step #1: Compute forces
		// ----------------------------------------------------------------------------------------

		if (enableGravity) {

			// Gravity force
			float fgx = 0.0f; float fgy = 0.0f;
			compute_gravity(fgx, fgy, ball);
			ball.fx += fgx; ball.fy += fgy; // Add this force to ball's total force

		}

		for (auto& water : waters) {

			// Aerodynamic Drag force (only when not in water) (Lift force)
			if (!is_colliding_with_water(ball, water))
			{
				if (enableLift) {

					float fdx = 0.0f; float fdy = 0.0f;
					compute_aerodynamic_drag(fdx, fdy, ball, atmosphere);
					ball.fx += fdx; ball.fy += fdy; // Add this force to ball's total force

				}

			}

			// Hydrodynamic forces (only when in water)
			if (is_colliding_with_water(ball, water))
			{

				if (enableHydroDrag) {

					// Hydrodynamic Drag force
					float fhdx = 0.0f; float fhdy = 0.0f;
					compute_hydrodynamic_drag(fhdx, fhdy, ball, water);
					ball.fx += fhdx; ball.fy += fhdy; // Add this force to ball's total force

				}

				if (enableBuoyancy) {

					// Hydrodynamic Buoyancy force
					float fhbx = 0.0f; float fhby = 0.0f;
					compute_hydrodynamic_buoyancy(fhbx, fhby, ball, water);
					ball.fx += fhbx; ball.fy += fhby; // Add this force to ball's total force

				}

			}

		}

		// Other forces
		// ...

		if (enableImpulsive) {

			// Impulsive force
			float fix = 0.0f; float fiy = 0.0f;
			compute_impulse(fix, fiy, ball);
			ball.fx += fix; ball.fy += fiy; // Add this force to ball's total force

		}

		if (enableElastic) {

			// Elastic force
			float fex = 0.0f; float fey = 0.0f;
			compute_elasticity(fex, fey, ball, spring);
			ball.fx += fex; ball.fy += fey; // Add this force to ball's total force

		}
		
		// Step #2: 2nd Newton's Law
		// ----------------------------------------------------------------------------------------
		
		// SUM_Forces = mass * accel --> accel = SUM_Forces / mass
		ball.ax = ball.fx / ball.mass;
		ball.ay = ball.fy / ball.mass;

		// Step #3: Integrate --> from accel to new velocity & new position
		// ----------------------------------------------------------------------------------------

		switch (actualPhysicsIntegrationScheme) {

			case PhysicsIntegrationSchemes::BACKWARDS_EULER:

				integrator_backwards_euler(ball, maxFrameDuration / 1000);

				break;

			case PhysicsIntegrationSchemes::FORWARDS_EULER:

				integrator_forwards_euler(ball, maxFrameDuration / 1000);

				break;

			case PhysicsIntegrationSchemes::VELOCITY_VERLET:

				integrator_velocity_verlet(ball, maxFrameDuration / 1000);

				break;

		}

		// Step #4: solve collisions
		// ----------------------------------------------------------------------------------------

		for (auto& ground : grounds) {

			// Solve collision between ball and ground
			if (is_colliding_with_ground(ball, ground))
			{
				switch (actualCollisionSolverScheme) {

				case CollisionSolverSchemes::NO_POSITION_SOLVING:

					collision_solver_no_position_solving(ball, ground);

					break;

				case CollisionSolverSchemes::TELEPORT:

					collision_solver_teleport(ball, ground);

					break;

				}

			}

		}

		for (auto& target : targets) {

			// Solve collision between ball and ground
			if (is_colliding_with_ground(ball, target))
			{
				switch (actualCollisionSolverScheme) {

				case CollisionSolverSchemes::NO_POSITION_SOLVING:

					collision_solver_no_position_solving(ball, target);

					break;

				case CollisionSolverSchemes::TELEPORT:

					collision_solver_teleport(ball, target);

					break;

				}

			}

		}

	}

	// Process all proyectiles in the scenario
	for (auto& proyectile : proyectiles)
	{
		// Skip ball if physics not enabled
		if (!proyectile.physics_enabled)
		{
			continue;
		}

		// Step #0: Clear old values
		// ----------------------------------------------------------------------------------------

		// Reset total acceleration and total accumulated force of the ball
		proyectile.fx = proyectile.fy = 0.0f;
		proyectile.ax = proyectile.ay = 0.0f;

		// Step #1: Compute forces
		// ----------------------------------------------------------------------------------------

		if (enableGravity) {

			// Gravity force
			float fgx = 0.0f; float fgy = 0.0f;
			compute_gravity(fgx, fgy, proyectile);
			proyectile.fx += fgx; proyectile.fy += fgy; // Add this force to ball's total force

		}

		// Aerodynamic Drag force (only when not in water) (Lift force)
		if (!is_colliding_with_water(proyectile, water))
		{
			if (enableLift) {

				float fdx = 0.0f; float fdy = 0.0f;
				compute_aerodynamic_drag(fdx, fdy, proyectile, atmosphere);
				proyectile.fx += fdx; proyectile.fy += fdy; // Add this force to ball's total force

			}

		}

		for (auto& water : waters) {

			// Hydrodynamic forces (only when in water)
			if (is_colliding_with_water(proyectile, water))
			{

				if (enableHydroDrag) {

					// Hydrodynamic Drag force
					float fhdx = 0.0f; float fhdy = 0.0f;
					compute_hydrodynamic_drag(fhdx, fhdy, proyectile, water);
					proyectile.fx += fhdx; proyectile.fy += fhdy; // Add this force to ball's total force

				}

				if (enableBuoyancy) {

					// Hydrodynamic Buoyancy force
					float fhbx = 0.0f; float fhby = 0.0f;
					compute_hydrodynamic_buoyancy(fhbx, fhby, proyectile, water);
					proyectile.fx += fhbx; proyectile.fy += fhby; // Add this force to ball's total force

				}

			}

		}

		// Other forces
		// ...

		if (enableElastic) {

			// Elastic force
			float fex = 0.0f; float fey = 0.0f;
			compute_elasticity(fex, fey, proyectile, spring);
			proyectile.fx += fex; proyectile.fy += fey; // Add this force to ball's total force

		}

		// Step #2: 2nd Newton's Law
		// ----------------------------------------------------------------------------------------

		// SUM_Forces = mass * accel --> accel = SUM_Forces / mass
		proyectile.ax = proyectile.fx / proyectile.mass;
		proyectile.ay = proyectile.fy / proyectile.mass;

		// Step #3: Integrate --> from accel to new velocity & new position
		// ----------------------------------------------------------------------------------------

		switch (actualPhysicsIntegrationScheme) {

		case PhysicsIntegrationSchemes::BACKWARDS_EULER:

			integrator_backwards_euler(proyectile, maxFrameDuration / 1000);

			break;

		case PhysicsIntegrationSchemes::FORWARDS_EULER:

			integrator_forwards_euler(proyectile, maxFrameDuration / 1000);

			break;

		case PhysicsIntegrationSchemes::VELOCITY_VERLET:

			integrator_velocity_verlet(proyectile, maxFrameDuration / 1000);

			break;

		}

		// Step #4: solve collisions
		// ----------------------------------------------------------------------------------------

		for (auto& ground : grounds) {

			// Solve collision between ball and ground
			if (is_colliding_with_ground(proyectile, ground))
			{
				switch (actualCollisionSolverScheme) {

				case CollisionSolverSchemes::NO_POSITION_SOLVING:

					collision_solver_no_position_solving(proyectile, ground);

					break;

				case CollisionSolverSchemes::TELEPORT:

					collision_solver_teleport(proyectile, ground);

					break;

				}
			}
		}


		for (int i = 0; i < targets.size(); i++) {

			// Solve collision between ball and ground
			if (is_colliding_with_ground(proyectile, targets[i]))
			{

				switch (actualCollisionSolverScheme) {

				case CollisionSolverSchemes::NO_POSITION_SOLVING:

					collision_solver_no_position_solving(proyectile, targets[i]);

					score += 100;

					App->audio->PlayFx(targetfx);

					targets.erase(targets.begin() + i);

					break;

				case CollisionSolverSchemes::TELEPORT:

					collision_solver_teleport(proyectile, targets[i]);

					score += 100;

					App->audio->PlayFx(targetfx);

					targets.erase(targets.begin() + i);

					break;

				}
			}

		}

	}

	// Continue game
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	// Colors
	int color_r, color_g, color_b;

	// Draw ground

	for (auto& ground : grounds) {

		color_r = 0; color_g = 255; color_b = 0;

		if (showTechnicalDebug) App->renderer->DrawQuad(ground.pixels(), color_r, color_g, color_b, 80);

	}

	for (auto& target : targets) {

		App->renderer->Blit(diana, target.tex_posX, target.tex_posY);

		if (showTechnicalDebug) App->renderer->DrawQuad(target.pixels(), target.RGB_Red, target.RGB_Green, target.RGB_Blue, 125);

	}

	// Draw water

	for (auto& water : waters) {

		color_r = 0; color_g = 0; color_b = 255;

		App->renderer->DrawQuad(water.pixels(), color_r, color_g, color_b, 60);

	}

	// Draw all balls in the scenario
	for (auto& ball : balls)
	{
		switch (actualPlayerControlScheme)
		{
			case PlayerControlSchemes::POSITION:

				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) ball.x -= 0.2f * maxFrameDuration / 10;
				if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) ball.x += 0.2f * maxFrameDuration / 10;
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) ball.y += 0.2f * maxFrameDuration / 10;
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) ball.y -= 0.2f * maxFrameDuration / 10;

				break;

			case PlayerControlSchemes::SPEED:

				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) ball.vx = -10;
				if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) ball.vx = 10;
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) ball.vy = 10;
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) ball.vy = -10;

				break;

			case PlayerControlSchemes::ACCELERATION:

				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) ball.vx -= 0.5f * maxFrameDuration / 10;
				if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) ball.vx += 0.5f * maxFrameDuration / 10;
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) ball.vy += 0.5f * maxFrameDuration / 10;
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) ball.vy -= 0.5f * maxFrameDuration / 10;

				break;

		}

		// Convert from physical magnitudes to geometrical pixels
		int pos_x = METERS_TO_PIXELS(ball.x);
		int pos_y = SCREEN_HEIGHT - METERS_TO_PIXELS(ball.y);
		int size_r = METERS_TO_PIXELS(ball.radius);

		// Select color
		if (ball.physics_enabled)
		{
			color_r = 255; color_g = 255; color_b = 255;
		}
		else
		{
			color_r = 255; color_g = 0; color_b = 0;
		}

		// Draw ball
		if (showTechnicalDebug) App->renderer->DrawCircle(pos_x, pos_y, size_r, ball.RGB_Red, ball.RGB_Green, ball.RGB_Blue);

	}

	for (auto& proyectile : proyectiles)
	{
		// Convert from physical magnitudes to geometrical pixels
		int pos_x = METERS_TO_PIXELS(proyectile.x);
		int pos_y = SCREEN_HEIGHT - METERS_TO_PIXELS(proyectile.y);
		int size_r = METERS_TO_PIXELS(proyectile.radius);

		// Select color
		if (proyectile.physics_enabled)
		{
			color_r = 255; color_g = 255; color_b = 255;
		}
		else
		{
			color_r = 255; color_g = 0; color_b = 0;
		}

		// Draw proyectile
		App->renderer->DrawCircle(pos_x, pos_y, size_r, proyectile.RGB_Red, proyectile.RGB_Green, proyectile.RGB_Blue);

	}

	// Amount of frames since startup

	frameCount++;

	// Amount of time since game start (use a low resolution timer)

	secondsSinceStartup = startupTime.ReadSec();

	App->renderer->BlitText("Time (seconds):", 650, 90, 200, 30, textColor);

	if (secondsSinceStartup < 10) {

		App->renderer->BlitText(std::to_string((int)(secondsSinceStartup)), 860, 92, 20, 30, textColorYellow);

	}
	else {

		App->renderer->BlitText(std::to_string((int)(secondsSinceStartup)), 860, 92, 40, 30, textColorYellow);

	}

	// Amount of ms took the last update

	dt = frameTime.ReadMSec();

	// Amount of frames during the last second

	lastSecFrameCount++;

	if (lastSecFrameTime.ReadMSec() > 1000) {

		lastSecFrameTime.Start();

		framesPerSecond = lastSecFrameCount;

		lastSecFrameCount = 0;

		// Average FPS for the whole game life

		averageFps = (averageFps + framesPerSecond) / 2;

	}

	maxFrameDuration = FPSarray[FPSiterator] * 1000;

	float delay = float(maxFrameDuration) - dt;

	PerfTimer delayTimer = PerfTimer();

	delayTimer.Start();

	if (maxFrameDuration > 0 && delay > 0) {

		SDL_Delay(delay);

		LOG("We waited for %f milliseconds and the real delay is % f", delay, delayTimer.ReadMs());

		dt = maxFrameDuration;

	}

	// Shows the time measurements in the window title

	static char title[256];

	sprintf_s(title, 256, "Av.FPS: %.2f Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %I64u ",
		averageFps, framesPerSecond, dt, secondsSinceStartup, frameCount);

	App->window->SetTitle(title);

	App->renderer->BlitText("Score:", 650, 10, 100, 30, textColor);

	if (score == 0) {

		App->renderer->BlitText(std::to_string((int)(score)), 760, 12, 20, 30, textColorYellow);

	}
	else {

		App->renderer->BlitText(std::to_string((int)(score)), 760, 12, 50, 30, textColorYellow);

	}

	if (score >= 500) {

		App->player->winCondition = true;

	}

	if (showTechnicalDebug) {

		App->renderer->BlitText("Current FPS Target:", 10, 10, 200, 30, textColor);
		App->renderer->BlitText(std::to_string((int)(std::round(1 / FPSarray[FPSiterator]))), 220, 10, 40, 30, textColorOrange);

		App->renderer->BlitText("Impulsive Force Enabled:", 10, 60, 250, 30, textColor);

		if (enableImpulsive) App->renderer->BlitText("Yes", 270, 60, 40, 30, textColorGreen);
		else App->renderer->BlitText("No", 270, 60, 40, 30, textColorRed);

		App->renderer->BlitText("Gravity Force Enabled:", 10, 110, 200, 30, textColor);

		if (enableGravity) App->renderer->BlitText("Yes", 220, 110, 40, 30, textColorGreen);
		else App->renderer->BlitText("No", 220, 110, 40, 30, textColorRed);

		App->renderer->BlitText("Lift Force Enabled:", 10, 160, 200, 30, textColor);

		if (enableLift) App->renderer->BlitText("Yes", 220, 160, 40, 30, textColorGreen);
		else App->renderer->BlitText("No", 220, 160, 40, 30, textColorRed);

		App->renderer->BlitText("Hydrodynamic Drag Force Enabled:", 10, 210, 300, 30, textColor);

		if (enableHydroDrag) App->renderer->BlitText("Yes", 320, 210, 40, 30, textColorGreen);
		else App->renderer->BlitText("No", 320, 210, 40, 30, textColorRed);

		App->renderer->BlitText("Buoyancy Force Enabled:", 10, 260, 250, 30, textColor);

		if (enableBuoyancy) App->renderer->BlitText("Yes", 270, 260, 40, 30, textColorGreen);
		else App->renderer->BlitText("No", 270, 260, 40, 30, textColorRed);

		App->renderer->BlitText("Elastic Force Enabled:", 10, 310, 200, 30, textColor);

		if (enableElastic) App->renderer->BlitText("Yes", 220, 310, 40, 30, textColorGreen);
		else App->renderer->BlitText("No", 220, 310, 40, 30, textColorRed);

		App->renderer->BlitText("Player's Impulse Force:", 10, 360, 200, 30, textColor);
		App->renderer->BlitText(std::to_string((Impulsearray[Impulseiterator])), 220, 360, 40, 30, textColorOrange);

		App->renderer->BlitText("Player's Restitution Coeficient:", 10, 410, 300, 30, textColor);
		App->renderer->BlitText(std::to_string((RCarray[RCiterator])), 320, 410, 100, 30, textColorOrange);

		App->renderer->BlitText("Gravity Used:", 10, 460, 150, 30, textColor);

		if (!gravityIterator) App->renderer->BlitText("Earth (-9.807 m/s^2)", 170, 460, 200, 30, textColorGreen);
		else App->renderer->BlitText("Moon (-1.625 m/s^2)", 170, 460, 200, 30, textColorOrange);

		App->renderer->BlitText("Physics Integration Scheme:", 10, 510, 300, 30, textColor);

		switch (actualPhysicsIntegrationScheme) {

		case PhysicsIntegrationSchemes::BACKWARDS_EULER:

			App->renderer->BlitText("Backwards Euler", 320, 510, 200, 30, textColorRed);

			break;

		case PhysicsIntegrationSchemes::FORWARDS_EULER:

			App->renderer->BlitText("Forwards Euler", 320, 510, 200, 30, textColorOrange);

			break;

		case PhysicsIntegrationSchemes::VELOCITY_VERLET:

			App->renderer->BlitText("Velocity Verlet", 320, 510, 180, 30, textColorGreen);

			break;

		}

		App->renderer->BlitText("Collision Solver Scheme:", 10, 560, 300, 30, textColor);

		switch (actualCollisionSolverScheme) {

		case CollisionSolverSchemes::NO_POSITION_SOLVING:

			App->renderer->BlitText("No Position Solving", 320, 560, 230, 30, textColorRed);

			break;

		case CollisionSolverSchemes::TELEPORT:

			App->renderer->BlitText("Teleport", 320, 560, 100, 30, textColorOrange);

			break;

		}

		App->renderer->BlitText("Player Control Scheme:", 10, 610, 270, 30, textColor);

		switch (actualPlayerControlScheme) {

		case PlayerControlSchemes::POSITION:

			App->renderer->BlitText("Position", 290, 610, 120, 30, textColorRed);

			break;

		case PlayerControlSchemes::SPEED:

			App->renderer->BlitText("Speed", 290, 610, 90, 30, textColorOrange);

			break;

		case PlayerControlSchemes::ACCELERATION:

			App->renderer->BlitText("Acceleration", 290, 610, 180, 30, textColorGreen);

			break;

		case PlayerControlSchemes::FORCE:

			App->renderer->BlitText("Force", 290, 610, 80, 30, textColorYellow);

			break;

		}

		App->renderer->BlitText("Wind vx:", 360, 10, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((int)(atmosphere.windx)), 470, 10, 25, 30, textColorOrange);

		App->renderer->BlitText("Wind vy:", 360, 60, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((int)(atmosphere.windy)), 470, 60, 25, 30, textColorOrange);

		App->renderer->BlitText("Player x:", 650, 160, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((float)(balls[0].x)), 770, 160, 100, 30, textColorGreen);
		App->renderer->BlitText("Player y:", 650, 200, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((float)(balls[0].y)), 770, 200, 100, 30, textColorGreen);

		App->renderer->BlitText("Player vx:", 650, 240, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((float)(balls[0].vx)), 770, 240, 100, 30, textColorOrange);
		App->renderer->BlitText("Player vy:", 650, 280, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((float)(balls[0].vy)), 770, 280, 100, 30, textColorOrange);

		App->renderer->BlitText("Player ax:", 650, 320, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((float)(balls[0].ax)), 770, 320, 100, 30, textColorRed);
		App->renderer->BlitText("Player ay:", 650, 360, 100, 30, textColor);
		App->renderer->BlitText(std::to_string((float)(balls[0].ay)), 770, 360, 100, 30, textColorRed);

		if (!proyectiles.empty()) {

			App->renderer->BlitText("Last proyectile x:", 650, 400, 200, 30, textColor);
			App->renderer->BlitText(std::to_string((float)(proyectiles.back().x)), 870, 400, 100, 30, textColorGreen);
			App->renderer->BlitText("Last proyectile y:", 650, 440, 200, 30, textColor);
			App->renderer->BlitText(std::to_string((float)(proyectiles.back().y)), 870, 440, 100, 30, textColorGreen);

			App->renderer->BlitText("Last proyectile vx:", 650, 480, 200, 30, textColor);
			App->renderer->BlitText(std::to_string((float)(proyectiles.back().vx)), 870, 480, 100, 30, textColorOrange);
			App->renderer->BlitText("Last proyectile vy:", 650, 520, 200, 30, textColor);
			App->renderer->BlitText(std::to_string((float)(proyectiles.back().vy)), 870, 520, 100, 30, textColorOrange);

			App->renderer->BlitText("Last proyectile ax:", 650, 560, 200, 30, textColor);
			App->renderer->BlitText(std::to_string((float)(proyectiles.back().ax)), 870, 560, 100, 30, textColorRed);
			App->renderer->BlitText("Last proyectile ay:", 650, 600, 200, 30, textColor);
			App->renderer->BlitText(std::to_string((float)(proyectiles.back().ay)), 870, 600, 100, 30, textColorRed);

		}

	}

	if (secondsSinceStartup <= 10) {

		App->renderer->BlitText("DESTROY ALL THE TARGETS BEFORE YOU RUN OUT OF BULLETS!", 10, 720, 900, 30, App->physics->textColorRed);

	}

	// --------------------------------- DEBUG KEYS ----------------------------------------

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) { // Show Colliders And Technical Debug Information in Real Time

		if (!showTechnicalDebug) showTechnicalDebug = true;
		else showTechnicalDebug = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {

		if (!enableImpulsive) enableImpulsive = true;
		else enableImpulsive = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {

		if (!enableGravity) enableGravity = true;
		else enableGravity = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {

		if (!enableLift) enableLift = true;
		else enableLift = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {

		if (!enableHydroDrag) enableHydroDrag = true;
		else enableHydroDrag = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {

		if (!enableBuoyancy) enableBuoyancy = true;
		else enableBuoyancy = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {

		if (!enableElastic) enableElastic = true;
		else enableElastic = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) { // Iterate Player's Impulse

		if (Impulseiterator < 4) {

			Impulseiterator++;
		}
		else {

			Impulseiterator = 0;
		}

	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) { // Iterate Restitution Coeficient

		if (RCiterator < 5) {

			RCiterator++;
		}
		else {

			RCiterator = 0;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) { // Iterate Gravity Values

		if (!gravityIterator) gravityIterator = true;
		else gravityIterator = false;

	}

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) { // FPS Iterator

		if (FPSiterator < 9) {

			FPSiterator++;

		}
		else {

			FPSiterator = 0;

		}


	}

	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) { // Wind velocity Iterator

		if (Winditerator < 4) {

			Winditerator++;

		}
		else {

			Winditerator = 0;

		}

	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) actualPhysicsIntegrationScheme = PhysicsIntegrationSchemes::BACKWARDS_EULER;
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) actualPhysicsIntegrationScheme = PhysicsIntegrationSchemes::FORWARDS_EULER;
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) actualPhysicsIntegrationScheme = PhysicsIntegrationSchemes::VELOCITY_VERLET;

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) actualCollisionSolverScheme = CollisionSolverSchemes::NO_POSITION_SOLVING;
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) actualCollisionSolverScheme = CollisionSolverSchemes::TELEPORT;

	if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN) actualPlayerControlScheme = PlayerControlSchemes::POSITION;
	if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN) actualPlayerControlScheme = PlayerControlSchemes::SPEED;
	if (App->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) actualPlayerControlScheme = PlayerControlSchemes::ACCELERATION;
	if (App->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN) actualPlayerControlScheme = PlayerControlSchemes::FORCE;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	return true;
}

// Compute modulus of a vector
float ModulePhysics::modulus(float vx, float vy)
{
	return std::sqrt(vx * vx + vy * vy);
}

void ModulePhysics::compute_impulse(float& fx, float& fy, const PhysBody& body) {

	if (actualPlayerControlScheme == PlayerControlSchemes::FORCE) {

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {

			fx = Impulsearray[Impulseiterator];
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {

			fx = -Impulsearray[Impulseiterator];

		}
		else {

			fx = 0;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {

			fy = Impulsearray[Impulseiterator];
		}
		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {

			fy = -Impulsearray[Impulseiterator];
		}
		else {

			fy = 0;
		}

	}

}

void ModulePhysics::compute_gravity(float& fx, float& fy, const PhysBody& body) {

	fx = body.mass * 0.0f;

	if (!gravityIterator) {

		fy = body.mass * EARTH_GRAVITY;

	}
	else {

		fy = body.mass * MOON_GRAVITY;

	}

}

// Compute Aerodynamic Drag force
void ModulePhysics::compute_aerodynamic_drag(float &fx, float& fy, const PhysBody &body, const Atmosphere &atmosphere)
{
	float rel_vel[2] = { body.vx - atmosphere.windx, body.vy - atmosphere.windy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = 0.5f * atmosphere.density * speed * speed * body.surface * body.cd; // Drag force (modulus)
	fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
}

// Compute Hydrodynamic Drag force
void ModulePhysics::compute_hydrodynamic_drag(float& fx, float& fy, const PhysBody& body, const Water& water)
{
	float rel_vel[2] = { body.vx - water.vx, body.vy - water.vy }; // Relative velocity
	float speed = modulus(rel_vel[0], rel_vel[1]); // Modulus of the relative velocity
	float rel_vel_unitary[2] = { rel_vel[0] / speed, rel_vel[1] / speed }; // Unitary vector of relative velocity
	float fdrag_modulus = body.b * speed; // Drag force (modulus)
	fx = -rel_vel_unitary[0] * fdrag_modulus; // Drag is antiparallel to relative velocity
	fy = -rel_vel_unitary[1] * fdrag_modulus; // Drag is antiparallel to relative velocity
}

// Compute Hydrodynamic Buoyancy force
void ModulePhysics::compute_hydrodynamic_buoyancy(float& fx, float& fy, const PhysBody& body, const Water& water)
{
	// Compute submerged area (assume ball is a rectangle, for simplicity)
	float water_top_level = water.y + water.h; // Water top level y
	float h = 2.0f * body.radius; // Ball "hitbox" height
	float surf = h * (water_top_level - body.y); // Submerged surface
	if ((body.y + body.radius) < water_top_level) surf = h * h; // If ball completely submerged, use just all ball area
	surf *= 0.4; // FUYM to adjust values (should compute the area of circle segment correctly instead; I'm too lazy for that)

	// Compute Buoyancy force
	double fbuoyancy_modulus = water.density * 10.0 * surf; // Buoyancy force (modulus)
	fx = 0.0; // Buoyancy is parallel to pressure gradient
	fy = fbuoyancy_modulus; // Buoyancy is parallel to pressure gradient
}

// Compute Elastic force
void ModulePhysics::compute_elasticity(float& fx, float& fy, const PhysBody& body, const Spring& spring) {

	fx = -spring.spring_constant * spring.displacementx;
	fy = -spring.spring_constant * spring.displacementy;

}

// Integration scheme: Backwards Euler
void ModulePhysics::integrator_backwards_euler(PhysBody& body, float dt)
{
	body.x += body.vx * dt;
	body.y += body.vy * dt;
	body.vx += body.ax * dt;
	body.vy += body.ay * dt;
}

// Integration scheme: Forwards Euler
void ModulePhysics::integrator_forwards_euler(PhysBody& body, float dt)
{
	body.vx += body.ax * dt;
	body.vy += body.ay * dt;
	body.x += body.vx * dt;
	body.y += body.vy * dt;
}

// Integration scheme: Velocity Verlet
void ModulePhysics::integrator_velocity_verlet(PhysBody& body, float dt)
{
	body.x += body.vx * dt + 0.5f * body.ax * dt * dt;
	body.y += body.vy * dt + 0.5f * body.ay * dt * dt;
	body.vx += body.ax * dt;
	body.vy += body.ay * dt;
}

// Collision Solver scheme: No Position Solving

void ModulePhysics::collision_solver_no_position_solving(PhysBody& body, const Ground& ground) {

	body.coef_restitution = RCarray[RCiterator];

	if (body.y + body.radius > ground.y + ground.h) {

		body.vy = -body.vy;

	}
	else if (body.y < ground.y) {
		
		body.vy = -body.vy;

	}
	else {

		if (body.x > ground.x) {
			
			body.vx = -body.vx;
		}
		else {
			
			body.vx = -body.vx;

		}

	}

	// FUYM non-elasticity
	body.vx *= body.coef_friction;
	body.vy *= body.coef_restitution;

}

// Collision Solver scheme: Teleport

void ModulePhysics::collision_solver_teleport(PhysBody& body, const Ground& ground) {

	body.coef_restitution = RCarray[RCiterator];

	// TP ball to ground surface

	if (body.y + body.radius > ground.y + ground.h) {

		body.y = ground.y + ground.h + body.radius;
		body.vy = -body.vy;

	}
	else if (body.y < ground.y) {

		body.y = ground.y - body.radius;
		body.vy = -body.vy;

	}
	else {

		if (body.x > ground.x) {

			body.x = ground.x + ground.w + body.radius;
			body.vx = -body.vx;
		}
		else {

			body.x = ground.x - body.radius;
			body.vx = -body.vx;

		}

	}

	// FUYM non-elasticity
	body.vx *= body.coef_friction;
	body.vy *= body.coef_restitution;

}

// Detect collision with ground
bool ModulePhysics::is_colliding_with_ground(const PhysBody& body, const Ground& ground)
{
	float rect_x = (ground.x + ground.w / 2.0f); // Center of rectangle
	float rect_y = (ground.y + ground.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(body.x, body.y, body.radius, rect_x, rect_y, ground.w, ground.h);
}

// Detect collision with water
bool ModulePhysics::is_colliding_with_water(const PhysBody& body, const Water& water)
{
	float rect_x = (water.x + water.w / 2.0f); // Center of rectangle
	float rect_y = (water.y + water.h / 2.0f); // Center of rectangle
	return check_collision_circle_rectangle(body.x, body.y, body.radius, rect_x, rect_y, water.w, water.h);
}

// Detect collision with other balls

bool ModulePhysics::is_colliding_with_ball(const PhysBody& body1, const PhysBody& body2)
{
	return check_collision_circle_circle(body1.x, body1.y, body1.radius, body2.x, body2.y, body2.radius);
}

// Detect collision between circle and rectangle
bool ModulePhysics::check_collision_circle_rectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh)
{
	// Algorithm taken from https://stackoverflow.com/a/402010

	// Distance from center of circle to center of rectangle
	float dist_x = std::abs(cx - rx);
	float dist_y = std::abs(cy - ry);

	// If circle is further than half-rectangle, not intersecting
	if (dist_x > (rw / 2.0f + cr)) { return false; }
	if (dist_y > (rh / 2.0f + cr)) { return false; }

	// If circle is closer than half-rectangle, is intersecting
	if (dist_x <= (rw / 2.0f)) { return true; }
	if (dist_y <= (rh / 2.0f)) { return true; }

	// If all of above fails, check corners
	float a = dist_x - rw / 2.0f;
	float b = dist_y - rh / 2.0f;
	float cornerDistance_sq = a * a + b * b;
	return (cornerDistance_sq <= (cr * cr));
}

bool ModulePhysics::check_collision_circle_circle(float cx1, float cy1, float cr1, float cx2, float cy2, float cr2)
{
	return ((cx2 - cx1) * (cx2 - cx1)) + ((cy2 - cy1) * (cy2 - cy1)) <= ((cr1 + cr2) * (cr1 + cr2));
}

// Convert from meters to pixels (for SDL drawing)
SDL_Rect Ground::pixels()
{
	SDL_Rect pos_px{};
	pos_px.x = METERS_TO_PIXELS(x);
	pos_px.y = SCREEN_HEIGHT - METERS_TO_PIXELS(y);
	pos_px.w = METERS_TO_PIXELS(w);
	pos_px.h = METERS_TO_PIXELS(-h); // Can I do this? LOL
	return pos_px;
}