#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"
#include "Audio.h"

class Enemy : public Entity
{
public:
	Enemy();
	Enemy(Entity* entity, SDL_Texture* tex = nullptr);

	virtual ~Enemy();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool CheckCollisionEnemy(fPoint nextPosition);

	bool PreUpdate();

	void CheckCollisionEnemyToPlayer();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

private:

	bool Radar(iPoint origin, int range);
	void CreatePathEnemy(iPoint origin, iPoint destination);
	int GetCurrentPositionInPath(iPoint mapPositionEnemy);
	void MoveEnemy();

private:

	int range = 150;
	int rangeMax = 200;
	bool isDetected = false;
	bool returning = false;
	int radiusCollision = 0;

	iPoint destination = { 0,0 };
	fPoint tmp = { 0.0f,0.0f };

	// We store the created path here
	DynArray<iPoint>* lastPath = nullptr;
};
#endif // _ENEMY_H_