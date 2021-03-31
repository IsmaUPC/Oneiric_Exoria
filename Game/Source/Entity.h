#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Module.h"
#include "Point.h"
#include "Animation.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "Map.h"
#include "Collisions.h"
#include "Defs.h"
#include "Log.h"


enum TypeEntity {

	UNKNOWN=0,
	PLAYER=1,
	GROUND_ENEMY=2,
	AIR_ENEMY=3,
	HUD=4,
	FIREBALL=5,
	COIN=6,
	LIVE=7,
};


enum State {

	IDLE,
	JUMP,
	WALK,
	RUN,
	RETURN,
	ATTACK,
	DEADING,
	DEAD,
	HIT,
};

enum MoveDirection {

	WALK_L=-1,
	WALK_R=1,
};
enum TypeCollision {

	VICTORY=0,
	COLLISION=1,
	CHECK_POINT=2,
	AIR,
};

struct EntityData
{
	iPoint position;
	State state;
	MoveDirection direction;
	Animation* currentAnimation = nullptr;
	float velocity;
	TypeEntity type;
	SDL_Texture* texture;
	int numPoints = 0;
	iPoint* pointsCollision;
	int fireBallState;
	uint deadFx;
	int dropScore;

public:
	EntityData(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture, int dropScore, uint deadFx) :
		position(pPosition), state(IDLE), direction(WALK_R), velocity(pVelocity),
		texture(pTexture), type(pTypeEntity),deadFx(deadFx), dropScore(dropScore)
	{};
	EntityData::EntityData() {};
};

class Entity : public Module
{
public:


	Entity(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture, int dropScore=0, uint deadFx=0);
	Entity();
	~Entity();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&);

	// Called before the first frame
	virtual bool Start();
	// Called each loop iteration
	virtual bool PreUpdate();

	// Called each loop iteration
	virtual bool Update(float dt);
	// Called each loop iteration
	virtual bool PostUpdate();
	// Called before quitting
	virtual bool CleanUp();

	// Virtual methods to Load state
	bool LoadState(pugi::xml_node&);
	// Virtual methods to Save state
	bool SaveState(pugi::xml_node&) const;
	// Transform fPoint to iPoint
	iPoint TransformFPoint(fPoint fpoint);
	iPoint MapToWorld(iPoint position);
	int CheckCollision(iPoint positionMap);
	
public:

	bool isAlive= false;
	bool pendingToDelete = false;

	EntityData* entityData;
	Collisions collision;
};

#endif // __MODULE_H__