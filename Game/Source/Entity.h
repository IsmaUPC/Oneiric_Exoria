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
	BANDIT=1,
	FIGHTER=2,
	SAPLING=3,
	TRUNK=4,
	PERRY=5,
	ALENS=6,
	LIAM=7,
	NAN_=8,
	BIN=9,
	LICAN=10,
	EESAAC=11,
	HEADACHE=12,
	HUD=13,
	NPC=14
};


enum State {

	IDLE,
	WALK,
	RUN,
	RETURN,
	ATTACK,
	DEADING,
	DEAD,
	HIT,
};

enum MoveDirection {

	WALK_L,
	WALK_R,
	WALK_UP,
	WALK_DOWN
};
enum TypeCollision {

	VICTORY=1,
	COLLISION= 0, // Change to id tile of collision
	CHECK_POINT=2,
	AIR,
};

struct EntityData
{
	iPoint position;
	State state = IDLE;
	MoveDirection direction = WALK_R;
	Animation* currentAnimation = nullptr;
	TypeEntity type;
	SDL_Texture* texture;
	int level = 0;
	float velocity = 1;
	int numPoints = 0;
	iPoint* pointsCollision;

public:
	EntityData(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture) :
		position(pPosition), state(IDLE), direction(WALK_R), velocity(pVelocity),
		texture(pTexture), type(pTypeEntity)
	{};
	EntityData::EntityData() {};
};

class Entity : public Module
{
public:


	Entity(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture);
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

	EntityData entityData;
	Collisions collision;
};

#endif // __MODULE_H__