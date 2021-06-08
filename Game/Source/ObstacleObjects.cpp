#include "ObstacleObjects.h"

#include "Defs.h"
#include "Log.h"


ObstacleObjects::ObstacleObjects() : Entity()
{
	name.Create("ObstacleObjects");
}

ObstacleObjects::ObstacleObjects(Entity* entity, SDL_Texture* tex)
{
	entityData = entity->entityData;
	entityData.texture = tex;
	this->tilePosition = app->map->WorldToMap(entityData.position.x, entityData.position.y);
	name.Create("ObstacleObjects");
}

// Destructor
ObstacleObjects::~ObstacleObjects()
{}

// Called before ObstacleObjects is available
bool ObstacleObjects::Awake(pugi::xml_node& config)
{
	bool ret = true;
	return ret;
}

// Called before the first frame
bool ObstacleObjects::Start()
{
	LOG("ObstacleObjects start");


	// back background
	return true;
}

// Called each loop iteration
bool ObstacleObjects::PreUpdate()
{
	return true;
}

bool ObstacleObjects::Update(float dt)
{

	//this->tilePosition = app->map->WorldToMap(entityData.position.x, entityData.position.y);
	//app->entityManager->boxes.At(posBox)->data->tilePosition = app->map->WorldToMap(aux.x, aux.y);


	return true;
}

bool ObstacleObjects::PostUpdate()
{
	SDL_Rect rect;
	
	if (entityData.type== TypeEntity::HOLE_ENTITY) {
		rect = { 32,32,32,32 };
		app->render->DrawTexture(this->entityData.texture, entityData.position.x, entityData.position.y , &rect);
	}
	else
	{
		rect = { 0,32,32,32 };
		app->render->DrawTexture(this->entityData.texture, entityData.position.x, entityData.position.y , &rect);

	}

	return true;
}

// Called before quitting
bool ObstacleObjects::CleanUp()
{
	LOG("Destroying ObstacleObjects");
	active = false;

	delete[] entityData.pointsCollision;
	entityData.pointsCollision = nullptr;

	return true;
}

// Load Game State
bool ObstacleObjects::LoadState(pugi::xml_node& data)
{
	return true;
}

// Save Game State
bool ObstacleObjects::SaveState(pugi::xml_node& data) const
{
	return true;
}

