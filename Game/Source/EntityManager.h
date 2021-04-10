#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "Entity.h"
#include "Player.h"
#include "GUI.h"
#include "Enemy.h"
#include "Coins.h"
#include "Lives.h"


class EntityManager : public Module
{
public:

	EntityManager();
	~EntityManager();

	// Called before render is available
	 bool Awake(pugi::xml_node&);

	// Called before the first frame
	 bool Start();
	// Called each loop iteration
	 bool PreUpdate();

	// Called each loop iteration
	 bool Update(float dt);
	 
	// Called each loop iteration
	 bool PostUpdate();
	// Called before quitting
	 bool CleanUp();

	// Virtual method to Load state
	bool LoadState(pugi::xml_node&);
	// Virtual method to Save state
	bool SaveState(pugi::xml_node&) const;

	bool AddEntity(TypeEntity pType, int pX, int pY, int level = 0);

	void DeleteEntity(Entity* entity);
	void DeleteHUD();
	void DeleteSpawnEntity(Entity* entity);

	void SpawnEntity(Entity* spawnEntity);
	void DespawnEntity(Entity* spawnEntity);

	void CheckSpawnEntities();
	void CheckDespawnEntities();

	void SpeedAnimationCheck(float dt);


public:
	uint timeSave = 0;
	List<Entity*> entities;
	List<Entity*> spawnQueue;
	iPoint mapDimensions = { 0,0 };
	int score = 0;

	uint chickenFx;
	uint batFx;
	uint liveFx;

	// Textures
	SDL_Texture* texCoin;
	SDL_Texture* texLive;
	SDL_Texture* texChicken;
	SDL_Texture* texBat;
	SDL_Texture* texHead;

	// Animations
	Animation* idleAnim = new Animation();
	Animation* isDetectedAnim = new Animation();
	Animation* walkAnim = new Animation();
	Animation* deadAnim = new Animation();
};

#endif // __MODULE_H__