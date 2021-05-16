#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Module.h"
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

	UNKNOWN = 0,
	BANDIT = 1,
	FIGHTER = 2,
	SAPLING = 3,
	TRUNK = 4,
	PERRY = 5,
	ALENS = 6,
	LIAM = 7,
	NAN_ = 8,
	BIN = 9,
	LICAN = 10,
	EESAAC = 11,
	HEADACHE = 12,
	HUD = 13,
	NPC = 14,
	KENZIE_ = 15,
	KEILER_ = 16,
	ISRRA_ = 17,
	BRENDA_ = 18,
	BOX_ENTITY,
	HOLE_ENTITY,
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
	MOBILE,
};

enum MoveDirection {

	WALK_L,
	WALK_R,
	WALK_UP,
	WALK_DOWN
};
enum TypeCollision {

	VICTORY = 1,
	COLLISION = 0, // Change to id tile of collision
	DOWN_LADDER = 2,
	UP_LADDER = 3,
	DOWN_HALL = 4,
	UP_HALL = 5,
	HOLE = 6,
	BOX = 7,
	CHECK_POINT = 8, 
	AIR,
};
struct Stats
{
	int attack = 0;
	int defense = 0;
	int speed = 0;
	float health = 0;
	int maxHealth = 0;
	int mana = 0;
	int maxMana = 0;
	float exp = 0;
	bool defenseOn = false;
};


struct EntityData
{
	fPoint position = {0,0};
	iPoint centerPoint = { 0,0 };
	iPoint positionInitial = { 0,0 };
	State state = IDLE;
	MoveDirection direction = WALK_R;
	Animation* currentAnimation = nullptr;
	TypeEntity type = UNKNOWN;
	SDL_Texture* texture = nullptr;
	float velocity = 1;
	int numPoints = 4;
	iPoint* pointsCollision = new iPoint[numPoints];
	int level = 0;
	int id = 0;
	int channel = -1;

public:
	EntityData(TypeEntity pTypeEntity, fPoint pPosition, float pVelocity, SDL_Texture* pTexture) :
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
	bool CheckCollisionObstacle(iPoint positionMap);
	bool CheckCollisionBoxes(iPoint positionMap);
	bool CheckCollisionHoles(iPoint positionMap);

	int CalculateDistance(iPoint origin, iPoint destination);
	bool MoveBox();


public:

	bool isAlive= false;
	bool pendingToDelete = false;

	EntityData entityData;
	Collisions collision;
	Stats stats;
	bool move = true;

	ListItem<TeleportNode*>* node=nullptr;
	int posBox;
	int posHole;
};

#endif // __MODULE_H__