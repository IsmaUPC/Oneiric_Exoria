#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "ItemManager.h"

struct PlayerData : public Entity
{
	iPoint position = { 0,0 };
	iPoint centerPoint = { 0,0 };
	State state = IDLE;
	MoveDirection direction = WALK_DOWN;
	Animation* currentAnimation = nullptr;
	float velocity = 1;
	int level = 1;
	int exp = 0;
	int health = 0;
	int mana = 0;
	uint respawns = 0;

	SDL_Texture* texture = nullptr;
	static const int numPoints = 4;

	iPoint pointsCollision[numPoints] =  { { 2,30 },{ 28, 30 },{ 28,50 },{ 2, 50 } };
};
enum TypePartner
{
	KEILER,
	ISRRA,
	BRENDA
};
struct Partner : public Entity
{
	iPoint position = {0,0};
	State state = IDLE;
	MoveDirection direction = WALK_R;
	Animation* currentAnimation = nullptr;
	SDL_Texture* texture = nullptr;
	TypePartner type;
	int level = 1;
	int exp = 0;
	int health = 0;
	int mana = 0;
	int breadcrumb = 0;
};


class Player : public Entity
{
public:

	Player();
	Player(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture);
	virtual ~Player();

	bool Awake(pugi::xml_node&);

	bool Start();

	void LoadTexCharacters();

	void LoadPartners();

	void RePositionPartners();

	bool PreUpdate();

	bool Update(float dt);

	void UpdatePointCheker();

	void SpeedAnimationCheck(float dt);

	void MoveBetweenCheckPoints();

	void CameraPlayer(float dt);

	void PlayerMoveAnimation(State state, MoveDirection direction, Animation* &currentAnimation);

	void PlayerControls(float dt);

	void MovePlayer(MoveDirection playerDirection, float dt);

	bool PostUpdate();

	bool CleanUp();

	bool CollisionPlayer(iPoint nextPosition);
	bool CheckGameOver(int level);

	void SetHit();

	void LivePlus() { playerData.respawns++; };

	void ActiveCheckpoint(iPoint positionMapPlayer);

	iPoint IPointMapToWorld(iPoint ipoint);

	Partner* GetPartners() { return partners; };
	int GetNumPartners() { return numPartners; };
	bool GetInCheckPoint() { return inCheckPoint; };
	bool GetCheckPointMove() { return checkpointMove; };

	bool UpdatePlayerStats(Entity* entity, TypeEntity type);

	bool SaveLevel(pugi::xml_node& data)const;
private:

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;

	void MoveToDirection(MoveDirection direction, iPoint &position);

	void DebugCP();

	void AddBreadcrumb();
	void DeleteBreadcrumb(iPoint* pos);

	void PartnerDirection(int index);
	void NextBreadcrumb(int index);
	void MovePartners();
	void OffsetPartners();

public:

	ItemManager* itemManager = nullptr;
	pugi::xml_document	itemsFile;

	List<GameItem*> inventory;

	PlayerData playerData;
	int radiusCollision = 0;
	bool godMode = false;
	iPoint* positionInitial= new iPoint(0,0);	
	bool win = false;
	bool loadStats = false;
	bool play = false;
	bool load = false;
	fPoint lerpCamera = { 0,0 };

	uint fxStairs = -1;

private:
	
	int levelScene = 0;
	float vel = 0;

	// Partners
	int numPartners = 3;
	Partner partners[3];
	MoveDirection lastDirection;
	List<iPoint*> path;
	bool playerCollision = true;
	int diagonal = 0;
	
	uint fxBookOpen = -1;

	Animation* idleAnimR = nullptr;
	Animation* idleAnimL = nullptr;
	Animation* idleAnimUp = nullptr;
	Animation* idleAnimDown = nullptr;

	Animation* bookAnim = nullptr;

	Animation* walkAnimR = nullptr;
	Animation* walkAnimL = nullptr;
	Animation* walkAnimUp = nullptr;
	Animation* walkAnimDown = nullptr;

	pugi::xml_document playerFile;
	SString folder;
	iPoint tmp = {0,0};

	// CheckPoint's vars
	bool inCheckPoint = false;
	List<iPoint> checkPoints;
	bool debugCheckPoints = false;

	int lastCP = 0;
	bool checkpointMove = false;
	bool endUpdate = false;

	State lastState;
	MoveDirection hitDirection;
	List<SDL_Texture*> texPartners;

	iPoint pointCheker = { 0,0 };

};

#endif // _PLAYER_H_
