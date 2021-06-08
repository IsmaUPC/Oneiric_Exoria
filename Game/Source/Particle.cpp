#include "Particle.h"
#include "Log.h"

Particle::Particle() : Module()
{
	name.Create("moduleParticles");
	
}

// Destructor
Particle::~Particle()
{}
// Called before Particle is available
bool Particle::Awake(pugi::xml_node& config)
{
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Particle::Start()
{
	LOG("ModuleParticles start");
	active = true;
	// back background
	return true;
}

// Called each loop iteration
bool Particle::PreUpdate()
{
	return true;
}

bool Particle::Update(float dt)
{
	return true;
}

bool Particle::PostUpdate()
{

	return true;
}

// Called before quitting
bool Particle::CleanUp()
{
	LOG("Destroying ModuleParticles");
	active = false;
	free(this);
	return true;
}

bool Particle::LoadState(pugi::xml_node&)
{
	return false;
}

bool Particle::SaveState(pugi::xml_node&) const
{
	return false;
}
