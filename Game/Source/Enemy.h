#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"
#include "Audio.h"

class Enemy : public Entity
{
public:
	Enemy();
	Enemy(Entity* entity, SDL_Texture* tex);

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
	int CalculateDistance(iPoint origin, iPoint destination);

private:

	int range = 150;
	int rangeMax = 200;
	bool isDetected = false;
	bool returning = false;
	int radiusCollision = 0;
	
	iPoint destination;
	fPoint tmp;

	// We store the created path here
	DynArray<iPoint>* lastPath;
};
#endif // _ENEMY_H_
