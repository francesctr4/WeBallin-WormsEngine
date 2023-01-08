#pragma once
#include "Module.h"
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include <vector>

#include <iostream>
#include <string>

#define PIXELS_PER_METER (20.0f) // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL (1.0f / PIXELS_PER_METER) // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) std::floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

enum class PhysicsIntegrationSchemes {

	BACKWARDS_EULER,
	FORWARDS_EULER,
	VELOCITY_VERLET

};

enum class PlayerControlSchemes {

	POSITION,
	SPEED,
	ACCELERATION,
	FORCE

};

enum class CollisionSolverSchemes {

	NO_POSITION_SOLVING,
	TELEPORT

};

class PhysBody {
public:
	PhysBody() {}

	PhysBody(float mass, float surface, float radius, float cd, float cl, float b, float coef_friction, float coef_restitution, float x, float y, float vx, float vy, int color_r, int color_g, int color_b) {

		// Set static properties of the ball
		this->mass = mass; // [kg]
		this->surface = surface; // [m^2]
		this->radius = radius; // [m]
		this->cd = cd; // [-]
		this->cl = cl; // [-]
		this->b = b; // [...]
		this->coef_friction = coef_friction; // [-]
		this->coef_restitution = coef_restitution; // [-]

		// Set initial position and velocity of the ball
		this->x = x;
		this->y = y;
		this->vx = vx;
		this->vy = vy;

		// Set color of the ball

		this->RGB_Red = color_r;
		this->RGB_Green = color_g;
		this->RGB_Blue = color_b;

	}

	// Position
	// You could also use an array/vector
	float x;
	float y;

	// Velocity
	float vx;
	float vy;

	// Acceleration
	float ax;
	float ay;

	// Force (total) applied to the ball
	float fx;
	float fy;

	// Mass
	float mass;

	// Aerodynamics stuff
	float surface; // Effective wet surface
	float cl; // Aerodynamic Lift coefficient
	float cd; // Aerodynamic Drag coefficient
	float b; // Hydrodynamic Drag coefficient

	// Coefficients of friction & restitution (for bounces)
	float coef_friction;
	float coef_restitution;

	// Shape
	float radius;

	// Has physics enabled?
	bool physics_enabled = true;

	// Color

	int RGB_Red;
	int RGB_Green;
	int RGB_Blue;

};

class PhysProyectile : public PhysBody {
public:

	PhysProyectile() {}

	PhysProyectile(float mass, float surface, float radius, float cd, float cl, float b, float coef_friction, float coef_restitution, float x, float y, float vx, float vy, int color_r, int color_g, int color_b) : PhysBody(mass, surface, radius, cd, cl, b, coef_friction, coef_restitution, x, y, vx, vy, color_r, color_g, color_b) {}

};

// Class: Ball object (a simple stoopid physics object)

class PhysBall : public PhysBody {
public:

	PhysBall() {}

	PhysBall(float mass, float surface, float radius, float cd, float cl, float b, float coef_friction, float coef_restitution, float x, float y, float vx, float vy, int color_r, int color_g, int color_b) : PhysBody(mass, surface, radius, cd, cl, b, coef_friction, coef_restitution, x, y, vx, vy, color_r, color_g, color_b) {}

};

// Class: Ground
class Ground : public SDL_Rect
{
public:

	Ground() {}

	Ground(float x, float y, float w, float h) {
		
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	
	}

	float x,y,w,h; // Geometry (x,y,w,h) [m]

	SDL_Rect pixels(); // Convert geometry to pixels to draw w/ SDL

};

// Class: Target
class Target : public SDL_Rect, public Ground
{
public:

	Target() {}

	Target(float x, float y, float w, float h, float color_r, float color_g, float color_b, int tex_posX, int tex_posY) : Ground(x,y,w,h) {

		this->RGB_Red = color_r;
		this->RGB_Green = color_g;
		this->RGB_Blue = color_b;

		this->tex_posX = tex_posX;
		this->tex_posY = tex_posY;

	}

	int RGB_Red;
	int RGB_Green;
	int RGB_Blue;

	int tex_posX;
	int tex_posY;

};

// Class: Water
class Water : public Ground
{
public:

	Water() {}

	Water(float x, float y, float w, float h, float density, float vx, float vy) : Ground(x, y, w, h) {

		this->density = density;
		this->vx = vx;
		this->vy = vy;

	}

	float density; // Density of fluid
	float vx; // Velocity x
	float vy; // Velocity y
};

// Class: Atmosphere
class Atmosphere
{
public:
	float density; // Density of air
	float windx; // Wind x
	float windy; // Wind y
};

// Class: Spring
class Spring
{
public:
	float spring_constant; // Spring constant in newtons per meter 
	float displacementx; // Displacement x of the spring in meters
	float displacementy; // Displacement y of the spring in meters
};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	// Compute modulus of a vector

	float modulus(float vx, float vy);

	// Compute Impulsive force

	void compute_impulse(float& fx, float& fy, const PhysBody& body);

	// Compute Gravity force

	void compute_gravity(float& fx, float& fy, const PhysBody& body);

	// Compute Aerodynamic Drag force (Lift force)

	void compute_aerodynamic_drag(float& fx, float& fy, const PhysBody& body, const Atmosphere& atmosphere);

	// Compute Hydrodynamic Drag force

	void compute_hydrodynamic_drag(float& fx, float& fy, const PhysBody& body, const Water& water);

	// Compute Hydrodynamic Buoyancy force

	void compute_hydrodynamic_buoyancy(float& fx, float& fy, const PhysBody& body, const Water& water);

	// Compute Elastic force

	void compute_elasticity(float& fx, float& fy, const PhysBody& body, const Spring& spring);

	// Integration scheme: Backwards Euler

	void integrator_backwards_euler(PhysBody& body, float dt);

	// Integration scheme: Forwards Euler

	void integrator_forwards_euler(PhysBody& body, float dt);

	// Integration scheme: Velocity Verlet

	void integrator_velocity_verlet(PhysBody& body, float dt);

	// Collision Solver scheme: No Position Solving

	void collision_solver_no_position_solving(PhysBody& body, const Ground& ground);

	// Collision Solver scheme: Teleport

	void collision_solver_teleport(PhysBody& body, const Ground& ground);

	// Detect collision with ground

	bool is_colliding_with_ground(const PhysBody& body, const Ground& ground);

	// Detect collision with water

	bool is_colliding_with_water(const PhysBody& body, const Water& water);

	// Detect collision with other balls

	bool is_colliding_with_ball(const PhysBody& body1, const PhysBody& body2);

	// Detect collision between circle and rectangle

	bool check_collision_circle_rectangle(float cx, float cy, float cr, float rx, float ry, float rw, float rh);

	// Detect collision between circle and circle

	bool check_collision_circle_circle(float cx1, float cy1, float cr1, float cx2, float cy2, float cr2);

	// Physics objects

	std::vector<PhysBody> balls{};
	std::vector<PhysBody> proyectiles{};
	std::vector<Ground> grounds{};
	std::vector<Target> targets{};
	std::vector<Water> waters{};

	Atmosphere atmosphere{};

	Ground ground{};
	Ground ground2{};
	Ground ground3{};
	Ground ground4{};
	Ground ground5{};
	Ground ground6{};
	Ground ground7{};
	Ground ground8{};
	Ground ground9{};
	Ground ground10{};
	Ground ground11{};
	Ground ground12{};

	Target target{};
	Target target2{};
	Target target3{};
	Target target4{};
	Target target5{};

	SDL_Texture* diana;

	Water water{};
	Water water2{};
	Water water3{};

	Spring spring{};

	int score;

	// Misc
	
	uint frames;
	float dt;

	uint targetfx;

	// Enable - Disable Forces

	bool enableImpulsive;
	bool enableGravity;
	bool enableLift;
	bool enableHydroDrag;
	bool enableBuoyancy;
	bool enableElastic;

	SDL_Color textColor = { 255, 255, 255, 0 };
	SDL_Color textColorGreen = { 0, 255, 0, 0 };
	SDL_Color textColorRed = { 255, 0, 0, 0 };
	SDL_Color textColorOrange = { 255, 165, 0, 0 };
	SDL_Color textColorYellow = { 255, 255, 0, 0 };

	bool showTechnicalDebug;

	// Schemes configuration

	PhysicsIntegrationSchemes actualPhysicsIntegrationScheme;

	PlayerControlSchemes actualPlayerControlScheme;

	CollisionSolverSchemes actualCollisionSolverScheme;

private:

	bool debug;

	bool gravityIterator;

	Timer timer;
	PerfTimer ptimer;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float secondsSinceStartup = 0.0f;

	float maxFrameDuration = 0;

	int FPSiterator;

	float FPSarray[10] = 
	{ 
		1.0f / 10.0f,
		1.0f / 20.0f,
		1.0f / 30.0f,
		1.0f / 45.0f,
		1.0f / 60.0f,
		1.0f / 75.0f,
		1.0f / 90.0f,
		1.0f / 100.0f,
		1.0f / 120.0f,
		1.0f / 144.0f
	};

	float actualRC;

	int RCiterator;

	float RCarray[6] =
	{
		0,
		0.2,
		0.4,
		0.6,
		0.8,
		1
	};

	int Impulseiterator;

	int Impulsearray[5] =
	{
		50,
		100,
		200,
		300,
		400
	};

	int Winditerator;

	float WindarrayX[5] =
	{
		2.0f,
		4.0f,
		6.0f,
		8.0f,
		10.0f
	};

	float WindarrayY[5] =
	{
		1.0f,
		2.0f,
		3.0f,
		4.0f,
		5.0f
	};


};