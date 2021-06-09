#include "Entity.h"
#include "Player.h"
#include "EntityManager.h"
#include "TpNodeManager.h"
#include "TeleportNode.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

Entity::Entity(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture) : Module()
{
	name.Create("Entity");
	isAlive = true;
}

Entity::Entity() : Module()
{
	name.Create("Entity");
	isAlive = true;
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
int Entity::CalculateDistance(iPoint origin, iPoint destination)
{
	return abs(origin.x - destination.x) + abs(origin.y - destination.y);
}
// Comprove position player in array of tiles in mapLayer collision
int Entity::CheckCollision(iPoint positionMap)
{
	uint typeTilePlayer = app->map->data.layers.At(7)->data->Get(positionMap.x, positionMap.y);
	uint firstgidLayerCollisions = app->map->data.tilesets.At(0)->data->firstgid;
	typeTilePlayer -= firstgidLayerCollisions;

	if (app->player->godMode == false && app->player->changeScene != true)
	{
		switch (typeTilePlayer)
		{
		case VICTORY:
			// Victory
			if(positionMap == app->map->WorldToMap( app->player->playerData.position))
			app->player->winGame = true;
			app->sceneManager->ending = true;
			return VICTORY;
			break;

		case COLLISION:
			// Collision
			return COLLISION;
			break;
		
		case UP_LADDER:
		case DOWN_LADDER:
		case UP_HALL:
		case DOWN_HALL:
			// Fill node origin in Scene manager
			app->sceneManager->originTpNode = app->sceneManager->tpManager->FindNodeTpInPlayer(typeTilePlayer);
			app->player->changeScene = true;
			break;

		case CHECK_POINT:
			// Checkpoint
			//if (positionMap == app->map->WorldToMap(app->player->playerData.position))
			//app->player->ActiveCheckpoint(positionMap);
			return CHECK_POINT;
			break;

		case HOLE:
			return HOLE;
			break;
		case BOX:
			return BOX;
		break;

		default:
			return AIR;
			break;
		}
	}

	return false;
}
bool Entity::CheckCollisionObstacle(iPoint positionMap)
{

	for (int i = 0; i < app->entityManager->boxes.Count(); i++)
		if (positionMap == app->entityManager->boxes.At(i)->data->tilePosition)
			return true;

	for (int i = 0; i < app->entityManager->holes.Count(); i++)
		if (positionMap == app->entityManager->holes.At(i)->data->tilePosition)
			return true;
	
	return false;
}
bool Entity::CheckCollisionBoxes(iPoint positionMap)
{
	for (int i = 0; i < app->entityManager->boxes.Count(); i++)
		if (positionMap == app->entityManager->boxes.At(i)->data->tilePosition)
		{
			posBox = i;
			return true;
		}

	return false;
}bool Entity::CheckCollisionHoles(iPoint positionMap)
{
	for (int i = 0; i < app->entityManager->holes.Count(); i++)
		if (positionMap == app->entityManager->holes.At(i)->data->tilePosition)
		{
			posHole = i;
			return true;
		}
	return false;
}
// TODO move to Obstacle Object
bool Entity::MoveBox()
{
	fPoint tmp= app->entityManager->boxes.At(posBox)->data->entityData.position;
	fPoint tmp2= tmp;
	fPoint aux;
	 switch (app->player->playerData.direction)
	 {
	 case WALK_L:

		 //app->entityManager->boxes.At(posBox)->data->entityData.position.x-=32;
		  tmp2.x-=32;

		 break;
	 case WALK_R:
		 //app->entityManager->boxes.At(posBox)->data->entityData.position.x += 32;
		 tmp2.x += 32;

		 break;
	 case WALK_UP:
		 //app->entityManager->boxes.At(posBox)->data->entityData.position.y -= 32;
		 tmp2.y -= 32;

		 break;
	 case WALK_DOWN:
		 //app->entityManager->boxes.At(posBox)->data->entityData.position.y += 32;
		 tmp2.y += 32;

		 break;
	 default:
		 break;
	 }
	// aux = app->entityManager->boxes.At(posBox)->data->entityData.position;
	 aux = app->entityManager->boxes.At(posBox)->data->entityData.position;
	 //iPoint check = app->map->WorldToMap(iPoint(aux.x,aux.y));
	 iPoint check = app->map->WorldToMap(iPoint(tmp2.x,tmp2.y));
	 

	 if ((CheckCollision(check) == AIR || CheckCollision(check) == HOLE 
		 || CheckCollision(check) == BOX) && 
		 !CheckCollisionBoxes(check)) {

		 app->entityManager->boxes.At(posBox)->data->entityData.position = tmp2;
		 aux = fPoint(app->entityManager->boxes.At(posBox)->data->entityData.position);

		 app->entityManager->boxes.At(posBox)->data->tilePosition = app->map->WorldToMap(tmp2.x, tmp2.y);

		 if (CheckCollisionHoles(app->entityManager->boxes.At(posBox)->data->tilePosition))
		 {
			 app->entityManager->boxes.At(posBox)->data->pendingToDelete = true;
			 app->entityManager->boxes.At(posBox)->data->entityData.state = DEAD;
			 app->entityManager->boxes.Del(app->entityManager->boxes.At(posBox));
			 app->entityManager->holes.At(posHole)->data->pendingToDelete = true;
			 app->entityManager->holes.At(posHole)->data->entityData.state = DEAD;
			 app->entityManager->holes.Del(app->entityManager->holes.At(posHole));

		 }

	 }
	 else
	 {
		 tmp2 = tmp;

	 }

	return false;
}