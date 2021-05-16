#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "Entity.h"
#include "Player.h"
#include "GUI.h"
#include "Enemy.h"
#include "Coins.h"
#include "Lives.h"
#include "ObstacleObjects.h"



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

	 void ClearList(bool& ret);

	// Virtual method to Load state
	bool LoadState(pugi::xml_node&);
	// Virtual method to Save state
	bool SaveState(pugi::xml_node&) const;

	bool AddEntity(TypeEntity pType, int pX, int pY, int id, int level = 0, bool move = true, State state = IDLE);
	bool AddEntity(TypeEntity pType, iPoint pos);

	Entity* FindNPC(int id);

	void DeleteEntity(Entity* entity);
	void DeleteHUD();
	void DeleteSpawnEntity(Entity* entity);

	void SpawnEntity(Entity* spawnEntity);
	void DespawnEntity(Entity* spawnEntity);

	void CheckSpawnEntities();
	void CheckDespawnEntities();

	void SpeedAnimationCheck(float dt);
	void SetCurrentEntity(Enemy* entity) { current = entity; };
	Entity* GetCurrentEntity() { return current; };


public:
	uint timeSave = 0;
	List<Entity*> entities;
	List<Entity*> spawnQueue;
	List<Entity*> partners;
	iPoint mapDimensions = { 0,0 };
	int score = 0;

	// Animations
	List<Animation*> animations;
	// Animation exclamation when enemy detected you, with Pokemon
	//Animation* isDetectedAnim = new Animation();
		
	//Dungeon Objects
	ObstacleObjects* auxObstacle= nullptr;
	List<ObstacleObjects*> boxes;
	List<ObstacleObjects*> holes;

	Entity* entityHUD = nullptr;
	bool drawCloud = false;

private:
	// Textures
	SDL_Texture* texEnemies = nullptr;
	SDL_Texture* texObstacles = nullptr;
	Enemy* current = nullptr;

};

#endif // __ENTITY_MANAGER_H__