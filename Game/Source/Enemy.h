#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"
#include "Audio.h"

class Enemy : public Entity
{
public:
	Enemy();
	Enemy(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture, int dropScore, uint deadFx);

	virtual ~Enemy();

	bool Awake(pugi::xml_node&);

	bool Start();

	void CheckCollisions();

	bool PreUpdate();

	void CheckCollisionEnemyToPlayer();
	void CheckCollisionEnemyToFireBall();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

private:

	bool Radar(iPoint origin);
	void CreatePathEnemy(iPoint origin, iPoint destination);
	bool CheckAllPoints(iPoint& mapPositionDestination, TypeCollision typeCollision);
	int GetCurrentPositionInPath(iPoint mapPositionEnemy);
	void MoveEnemy(iPoint nextAuxPositionEenemy, iPoint mapPositionEnemy, TypeEntity type);
	void MoveEnemyNULL(iPoint mapPositionEnemy);
	int CalculateDistance(iPoint origin, iPoint destination);
	void SpeedAnimationCheck(float dt);


private:
	Animation* idleAnim = new Animation();
	Animation* isDetectedAnim = new Animation();
	Animation* walkAnim = new Animation();
	Animation* deadAnim = new Animation();

	int range = 500;
	bool isDetected = false;
	bool returning;
	// Timer
	Timer* checkDestination = new Timer();
	iPoint destination;
	iPoint positionInitial;
	// We store the created path here
	DynArray<iPoint>* lastPath;

	uint chickenFx;
	uint batFx;
	
};
#endif // _ENEMY_H_
