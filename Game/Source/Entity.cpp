#include "Entity.h"
#include "Player.h"
#include "EntityManager.h"
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

	if (typeTilePlayer == UP_LADDER)
	{
		int stop=0;
	}

	if (app->player->godMode == false) 
	{
		switch (typeTilePlayer)
		{
		case VICTORY:
			// Victory
			if(positionMap == app->map->WorldToMap( app->player->playerData.position))
			app->player->win = true;
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

			app->sceneManager->originTpNode = FindNodeTpInPlayer(typeTilePlayer);
			app->player->win = true;

			break;

		case CHECK_POINT:
			// Checkpoint
			//if (positionMap == app->map->WorldToMap(app->player->playerData.position))
			//app->player->ActiveCheckpoint(positionMap);
			return CHECK_POINT;
			break;

		default:
			return -1;
			break;
		}
	}

	return false;
}

// Move to TpNode Class
List<TeleportNode*>* Entity::FindTpNodeList( uint typeNode)
{
	List<TeleportNode*>* nodeTpList= nullptr;
	switch (typeNode)
	{
	case UP_LADDER:
		// Next floor
		nodeTpList = &app->map->tpNodeUpLadder;
		break;

	case DOWN_LADDER:
		// Prev floor
		nodeTpList = &app->map->tpNodeDownLadder;
		break;

	case UP_HALL:
		// Next hall
		nodeTpList = &app->map->tpNodeUpHall;
		break;

	case DOWN_HALL:
		// Prev hall
		nodeTpList = &app->map->tpNodeDownHall;
		break;
	}

	return nodeTpList;
}

// Move to TpNode Class
TeleportNode* Entity::FindNodeTpInPlayer(uint typeTilePlayer)
{
	ListItem<TeleportNode*>* node=	FindTpNodeList(typeTilePlayer)->start;
//	iPoint vecPlayer = app->map->WorldToMap(app->player->playerData.position);
	iPoint vecPlayer =app->player->playerData.position;
	iPoint listPointsCollision[4];
	int y = vecPlayer.y;
	int x = vecPlayer.x;

	for (int i = 0; i < app->player->playerData.numPoints; i++)
	{
		// Convert position player WorldToMap 
		listPointsCollision[i]= app->map->WorldToMap(x + app->player->playerData.pointsCollision[i].x, y + app->player->playerData.pointsCollision[i].y);
	
	}

	for (node; node; node = node->next)
	{
		if (node->data->position == listPointsCollision[0]||
			node->data->position == listPointsCollision[1] || 
			node->data->position == listPointsCollision[2] || 
			node->data->position == listPointsCollision[3] )
		{
			//app->player->win = true;
			//app->sceneManager->originTpNode = node->data;
			return  node->data;
			break;
		}
	}
	
	return nullptr;
}

// Move to TpNode Class
TeleportNode* Entity::FindNodeTpById(uint typeNode, int nodeId)
{
	TeleportNode* nodeById = FindTpNodeList(typeNode)->At(nodeId)->data;
	
	return (nodeById != NULL) ? nodeById : 0;

}
