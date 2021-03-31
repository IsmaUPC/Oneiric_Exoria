#include "EntityManager.h"
#include "SceneManager.h"
#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entity_manager");

}

// Destructor
EntityManager::~EntityManager()
{}

// Called before EntityManager is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	bool ret = true;


	return ret;
}

// Called before the first frame
bool EntityManager::Start()
{
	LOG("EntityManager start");
	active = true;

	chickenFx = app->audio->LoadFx("Assets/Audio/Fx/chicken.wav");
	batFx = app->audio->LoadFx("Assets/Audio/Fx/bat.wav");
	liveFx = app->audio->LoadFx("Assets/Audio/Fx/lives.wav");
	texCoin = app->tex->Load("Assets/Textures/coin_square.png");
	texLive = app->tex->Load("Assets/Textures/lives.png");
	texChicken = app->tex->Load("Assets/Textures/enemy_walk.png");
	texBat  =app->tex->Load("Assets/Textures/enemy_fly.png");
	texHead = app->tex->Load("Assets/Textures/GUI/dino_head.png");


	// back background
	return true;
}

// Called each loop iteration
bool EntityManager::PreUpdate()
{
	for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next)
		if(entiti->data->pendingToDelete)entities.Del(entiti);
	

	for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next) entiti->data->PreUpdate();

	return true;
}

bool EntityManager::Update(float dt)
{
	if (!app->sceneManager->GetIsPause())
	{
		HandleEntitiesSpawn();
		for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next) entiti->data->Update(dt);
		HandleEntitiesDespawn();
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next) entiti->data->PostUpdate();
	if(app->sceneManager->GetIsPause())(app->sceneManager->menu->Draw());
	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	LOG("Destroying EntityManager");

	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.start;

	while (item != NULL && ret == true)
	{
		if (item->data->active == true)
			ret = item->data->CleanUp();

		item = item->next;
	}

	app->audio->Unload1Fx(chickenFx);
	app->audio->Unload1Fx(batFx);
	app->audio->Unload1Fx(liveFx);
	app->tex->UnLoad(texCoin);
	app->tex->UnLoad(texLive);
	app->tex->UnLoad(texChicken);
	app->tex->UnLoad(texBat);
	app->tex->UnLoad(texHead);

	entities.Clear();
	score = 0;
	timeSave = 0;
	active = false;
	return ret;
}


bool EntityManager::AddEntity(TypeEntity pType, int pX, int pY)
{
	iPoint positionSpawn = app->entity->MapToWorld(iPoint(pX,pY));
	if (spawnQueue.Add(new EntitySpawnPoint(pType, positionSpawn.x, positionSpawn.y))) return true;
	else return false;
}
bool EntityManager::AddEntity(TypeEntity pType, int pX, int pY,int num)
{
	if (spawnQueue.Add(new EntitySpawnPoint(pType, pX, pY))) return true;
	else return false;
}
void EntityManager::HandleEntitiesSpawn()
{
	for (ListItem<EntitySpawnPoint*>* spawnEntiti = spawnQueue.start; spawnEntiti; spawnEntiti = spawnEntiti->next)
	{
		if (spawnEntiti->data->type != TypeEntity::UNKNOWN)
		{
			LOG("Spawning enemy at x:%d  y: %d", spawnEntiti->data->x * SCREEN_SIZE, spawnEntiti->data->x * SCREEN_SIZE);

			SpawnEnemy(*spawnEntiti->data);
			spawnEntiti->data->type = TypeEntity::UNKNOWN; // Removing the newly spawned enemy from the queue
		}
	}
}

void EntityManager::SpawnEnemy(const EntitySpawnPoint& info)
{
	Entity* iterateEntity;
	SDL_Texture* tex=nullptr;
	// Find an empty slot in the enemies array
	ListItem<Entity*>* entiti= entities.start;
	switch (info.type)
	{
	case TypeEntity::PLAYER:
		entities.Add(new Player(info.type,{ info.x,info.y }, 1, tex));
		entities.end->data->Start();
		break;

	case TypeEntity::GROUND_ENEMY:
		entities.Add(new Enemy(info.type, { info.x,info.y }, 1, texChicken,100,chickenFx));
		entities.end->data->Start();
		break;

	case TypeEntity::AIR_ENEMY:
		entities.Add(new Enemy(info.type, { info.x,info.y }, 1, texBat,150,batFx));
		entities.end->data->Start();
		break;
	case TypeEntity::HUD:
		entities.Add(new GUI(info.type, { info.x,info.y }, 1, texHead));
		entities.end->data->Start();
		break;
	case TypeEntity::FIREBALL:
		entities.Add(new FireBall(info.type, { info.x,info.y }, 1, tex));
		entities.end->data->Start();
		break;
	case TypeEntity::COIN:
		entities.Add(new Coins(info.type, { info.x,info.y }, 1, texCoin));
		entities.end->data->Start();
		break;
	case TypeEntity::LIVE:
		entities.Add(new Lives(info.type, { info.x,info.y }, 1, texLive,(int)50,liveFx));
		entities.end->data->Start();
	break;
	}
}

void EntityManager::HandleEntitiesDespawn()
{

}

bool EntityManager::LoadState(pugi::xml_node& entityManagerNode)
{
	bool ret = true;
	pugi::xml_node entitiesNode = entityManagerNode.child("entities").first_child();
	pugi::xml_node aux;
	score = entityManagerNode.child("score").attribute("value").as_int(0);
	if (entitiesNode != NULL)
	{
		for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next)
		{
			if (entiti->data->entityData->type == TypeEntity::HUD)entiti->data->LoadState(entityManagerNode);
			entiti->data->CleanUp();
			entities.Clear();
		}

		entityManagerNode.next_sibling();
		while (entitiesNode)
		{
			AddEntity((TypeEntity)entitiesNode.attribute("type").as_int(), entitiesNode.attribute("x").as_int(), entitiesNode.attribute("y").as_int(), 0);
			entitiesNode = entitiesNode.next_sibling();
		}
	}
	
	return ret;
}
bool EntityManager::SaveState(pugi::xml_node& entityManagerNode) const
{
	if(!app->removeGame)entityManagerNode.child("score").attribute("value").set_value(score);
	else entityManagerNode.child("score").attribute("value").set_value(0);

	entityManagerNode.remove_child("entities");
	entityManagerNode.append_child("entities").set_value(0);
	ListItem<Entity*>* entiti= entities.start;
	if (!app->removeGame)
	{
		pugi::xml_node entitiesNode = entityManagerNode.child("entities");
		pugi::xml_node entity_node = entitiesNode;

		for (entiti; entiti; entiti = entiti->next)
		{
			entitiesNode.append_child("entity").append_attribute("type").set_value(entiti->data->entityData->type);
			entitiesNode.last_child().append_attribute("x").set_value(entiti->data->entityData->position.x);
			entitiesNode.last_child().append_attribute("y").set_value(entiti->data->entityData->position.y);
			entitiesNode.last_child().append_attribute("state").set_value(entiti->data->entityData->state);
			if (entiti->data->entityData->type == TypeEntity::HUD)
				entiti->data->SaveState(entityManagerNode);
		}
	}
	else
	{
		for (entiti; entiti; entiti = entiti->next)
		{
			if (entiti->data->entityData->type == TypeEntity::HUD)
				entiti->data->SaveState(entityManagerNode);
			break;
		}
	}
	return true;
}
