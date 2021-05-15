#include "TpNodeManager.h"
#include "Entity.h"

TpNodeManager::TpNodeManager()
{
}

TpNodeManager::~TpNodeManager()
{
}

bool TpNodeManager::Awake()
{
	return false;
}

bool TpNodeManager::Start()
{
	return false;
}

bool TpNodeManager::PreUpdate()
{
	return false;
}

bool TpNodeManager::Update(float dt)
{
	return false;
}

bool TpNodeManager::PostUpdate()
{
	return false;
}

bool TpNodeManager::CleanUp()
{
	return false;
}


// Move to TpNode Class
List<TeleportNode*>* TpNodeManager::FindTpNodeList(uint typeNode)
{
	List<TeleportNode*>* nodeTpList = nullptr;
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

	return (nodeTpList != NULL) ? nodeTpList : 0;
}

// Move to TpNode Class
TeleportNode* TpNodeManager::FindNodeTpInPlayer(uint typeTilePlayer)
{
	ListItem<TeleportNode*>* node = FindTpNodeList(typeTilePlayer)->start;
	//	iPoint vecPlayer = app->map->WorldToMap(app->player->playerData.position);
	iPoint vecPlayer = app->player->playerData.position;
	iPoint listPointsCollision[4];
	int y = vecPlayer.y;
	int x = vecPlayer.x;

	for (int i = 0; i < app->player->playerData.numPoints; i++)
	{
		// Convert position player WorldToMap 
		listPointsCollision[i] = app->map->WorldToMap(x + app->player->playerData.pointsCollision[i].x, y + app->player->playerData.pointsCollision[i].y);

	}

	for (node; node; node = node->next)
	{
		if (node->data->position == listPointsCollision[0] ||
			node->data->position == listPointsCollision[1] ||
			node->data->position == listPointsCollision[2] ||
			node->data->position == listPointsCollision[3])
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
TeleportNode* TpNodeManager::FindNodeTpById(uint typeNode, int nodeId)
{

	TeleportNode* nodeById = FindTpNodeList(typeNode)->At(nodeId)->data;

	return (nodeById != NULL) ? nodeById : 0;

}

void TpNodeManager::SpawnPlayerTpNode(TeleportNode* originTpNode)
{
	//Move to TpNode Class
	  //Spawn Player in Tp Position
	{
		if (originTpNode != NULL)
		{
			int idNode = originTpNode->idNode;
			uint typeNode = originTpNode->typeTpNode;//select next node

			//if the type of node is even, it means that it is of type down, if it is odd otherwise, 
			//to decide the next one it is added or subtracted depending on its origin
			(typeNode % 2 == 0) ? typeNode += 1 : typeNode -= 1;

			iPoint pos = FindNodeTpById(typeNode, idNode)->position;

			if (typeNode % 2 == 0)
			{
				pos.y -= 2;
				app->player->playerData.direction = WALK_UP;
			}
			else
			{
				pos.y += 2;
				app->player->playerData.direction = WALK_DOWN;
			}

			pos = app->map->MapToWorld(pos);

			//app->player->positionInitial = new iPoint(pos.x, pos.y);
			app->player->playerData.position = pos;

			app->audio->PlayFx(app->player->fxStairs);
		}
		originTpNode=nullptr;
	}
}