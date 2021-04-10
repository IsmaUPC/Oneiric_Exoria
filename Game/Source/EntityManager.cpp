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

	// Load Fx

	// Load Textures
	texCoin = app->tex->Load("Assets/Textures/coin_square.png");
	texLive = app->tex->Load("Assets/Textures/lives.png");
	texHead = app->tex->Load("Assets/Textures/GUI/dino_head.png");

	// Animations


	return true;
}

// Called each loop iteration
bool EntityManager::PreUpdate()
{
	CheckDespawnEntities();

	for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next)
		entiti->data->PreUpdate();

	return true;
}

bool EntityManager::Update(float dt)
{
	if (!app->sceneManager->GetIsPause())
	{
		SpeedAnimationCheck(dt);

		CheckSpawnEntities();

		for (ListItem<Entity*>* entity = entities.start; entity; entity = entity->next) 
			entity->data->Update(dt);
	}

	return true;
}

bool EntityManager::PostUpdate()
{
	for (ListItem<Entity*>* entiti = entities.start; entiti; entiti = entiti->next)
		entiti->data->PostUpdate();
	
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

	// Unload Fx

	// Unload Tx
	app->tex->UnLoad(texCoin);
	app->tex->UnLoad(texHead);
	app->tex->UnLoad(texLive);
	// Unload Animations

	// Clear list
	entities.Clear();

	score = 0;
	timeSave = 0;
	active = false;

	return ret;
}

void EntityManager::CheckSpawnEntities()
{
	if (spawnQueue.Count() != 0)
	{
		iPoint a;
		SDL_Rect b = app->render->camera;
		for (ListItem<Entity*>* spawnEntity = spawnQueue.start; spawnEntity; spawnEntity = spawnEntity->next)
		{
			a = spawnEntity->data->entityData.position;
			if ((a.x > b.x - SPAWN_MARGIN && a.x < b.x + b.w + SPAWN_MARGIN)
				&& (a.y > b.y - SPAWN_MARGIN && a.y < b.y + b.h + SPAWN_MARGIN))
				SpawnEntity(spawnEntity->data);
		}
	}
}
void EntityManager::CheckDespawnEntities()
{
	if (entities.Count() != 0)
	{
		iPoint a;
		SDL_Rect b = app->render->camera;
		for (ListItem<Entity*>* despawnEntity = entities.start; despawnEntity; despawnEntity = despawnEntity->next)
		{
			a = despawnEntity->data->entityData.position;
			if (!((a.x > b.x - SPAWN_MARGIN && a.x < b.x + b.w + SPAWN_MARGIN)
				&& (a.y > b.y - SPAWN_MARGIN && a.y < b.y + b.h + SPAWN_MARGIN)))
				DespawnEntity(despawnEntity->data);
		}
	}
}

bool EntityManager::AddEntity(TypeEntity pType, int pX, int pY, int level)
{
	Entity* b = new Entity;
	iPoint positionSpawn = { pX, pY };
	b->entityData.type = pType;
	b->entityData.position = positionSpawn;
	b->entityData.level = level;

	spawnQueue.Add(b);

	return true;
}

void EntityManager::SpawnEntity(Entity* info)
{
	switch (info->entityData.type)
	{
	case BANDIT:
	case FIGHTER:
	case SAPLING:
	case TRUNK:
	case PERRY:
	case ALENS:
	case LIAM:
	case NAN_:
	case BIN:
	case LICAN:
	case EESAAC:
	case HEADACHE:
		entities.Add(new Enemy(info, texCoin)); // Change texture for atlasEnemy
		break;

	case HUD:
		entities.Add(new GUI(info, texHead));
		break;

		/*case NPC:
			entities.Add(new GUI(info, texHead));
			break;*/
	}

	entities.end->data->Start();
	DeleteSpawnEntity(info);
}

void EntityManager::DespawnEntity(Entity* spawnEntity)
{
	spawnQueue.Add(spawnEntity);
	DeleteEntity(spawnEntity);
}

void EntityManager::DeleteEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity)
		{
			// TODO 6: Notify the audio manager that a channel can be released 
			//app->audio->DeleteChannel(item->data->channel);
			entities.Del(item);
			break;
		}
	}
}
void EntityManager::DeleteSpawnEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = spawnQueue.start; item != NULL; item = item->next)
	{
		if (item->data == entity)
		{
			// TODO 6: Notify the audio manager that a channel can be released 
			//app->audio->DeleteChannel(item->data->channel);
			spawnQueue.Del(item);
			break;
		}
	}
}

void EntityManager::SpeedAnimationCheck(float dt)
{
	idleAnim->speed = (dt * 5);
	walkAnim->speed = (dt * 9);
	deadAnim->speed = (dt * 5);
	isDetectedAnim->speed = (dt * 9);
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
			if (entiti->data->entityData.type == TypeEntity::HUD)entiti->data->LoadState(entityManagerNode);
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
			entitiesNode.append_child("entity").append_attribute("type").set_value(entiti->data->entityData.type);
			entitiesNode.last_child().append_attribute("x").set_value(entiti->data->entityData.position.x);
			entitiesNode.last_child().append_attribute("y").set_value(entiti->data->entityData.position.y);
			entitiesNode.last_child().append_attribute("state").set_value(entiti->data->entityData.state);
			if (entiti->data->entityData.type == TypeEntity::HUD)
				entiti->data->SaveState(entityManagerNode);
		}
	}
	else
	{
		for (entiti; entiti; entiti = entiti->next)
		{
			if (entiti->data->entityData.type == TypeEntity::HUD)
				entiti->data->SaveState(entityManagerNode);
			break;
		}
	}
	return true;
}
