#include "Entity.h"
#include "Player.h"
#include "EntityManager.h"

#include "Defs.h"
#include "Log.h"

Entity::Entity(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture, int dropScore, uint deadFx) : Module()
{
	name.Create("Entity");
	isAlive = true;
	entityData = new EntityData(pTypeEntity, pPosition, pVelocity, pTexture,dropScore, deadFx);
}

Entity::Entity() : Module()
{
	name.Create("Entity");
	isAlive = true;
	entityData = new EntityData();
}

// Destructor
Entity::~Entity()
{}

// Called before Entity is available
bool Entity::Awake(pugi::xml_node& config)
{
	bool ret = true;
	return ret;
}

// Called before the first frame
bool Entity::Start()
{
	LOG("Entity start");
	// back background
	return true;
}

// Called each loop iteration
bool Entity::PreUpdate()
{
	return true;
}

bool Entity::Update(float dt)
{
	return true;
}

bool Entity::PostUpdate()
{

	return true;
}

// Called before quitting
bool Entity::CleanUp()
{
	LOG("Destroying Entity");
	active = false;

	delete entityData->currentAnimation;
	delete entityData->pointsCollision;

	delete entityData;
	return true;
}

// Load Game State
bool Entity::LoadState(pugi::xml_node& data)
{
	return true;
}

// Save Game State
bool Entity::SaveState(pugi::xml_node& data) const
{
	return true;
}
iPoint Entity::TransformFPoint(fPoint fpoint)
{
	iPoint transformation;

	transformation.x = fpoint.x;
	transformation.y = fpoint.y;

	return transformation;
}
iPoint Entity::MapToWorld(iPoint position)
{
	app->entityManager->mapDimensions = app->map->GetDimensionsMap();
	iPoint ret;
	ret.x = position.x * app->entityManager->mapDimensions.x;
	ret.y = position.y * app->entityManager->mapDimensions.y;
	return ret;
}
// Comprove position player in array of tiles in mapLayer collision
int Entity::CheckCollision(iPoint positionMap)
{
	uint typeTilePlayer = app->map->data.layers.At(2)->data->Get(positionMap.x, positionMap.y);
	uint firstgidLayerCollisions = app->map->data.tilesets.At(2)->data->firstgid;
	typeTilePlayer -= firstgidLayerCollisions;

	if (app->player->godMode == false) 
	{
		switch (typeTilePlayer)
		{
		case VICTORY:
			// Victory
			if(positionMap== app->map->WorldToMap( app->player->playerData.position))
			app->player->win = true;
			return VICTORY;
			break;

		case COLLISION:
			// Collision
			return COLLISION;
			break;

		case CHECK_POINT:
			// Checkpoint
			if (positionMap == app->map->WorldToMap(app->player->playerData.position)|| this->entityData->type== FIREBALL)
			app->player->ActiveCheckpoint(positionMap);
			return CHECK_POINT;
			break;

		default:
			return -1;
			break;
		}
	}

	return false;
}