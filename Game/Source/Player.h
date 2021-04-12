#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"

struct PlayerData
{
	iPoint position;
	iPoint centerPoint;
	State state;
	MoveDirection direction;
	Animation* currentAnimation;
	float velocity ;
	int level = 1;
	uint lives = 0;
	uint respawns = 0;
	uint coins = 0;

	SDL_Texture* texture;
	static const int numPoints = 4;

	//iPoint pointsCollision[numPoints] = { { 0, 0 },{ 32, 0 },{ 32,44 },{ 0, 44 } };
	iPoint pointsCollision[numPoints] =  { { 2,30 },{ 28, 30 },{ 28,50 },{ 2, 50 } };

};
enum TypePartner
{
	KEILER,
	ISRRA,
	BRENDA
};
struct Partner 
{
	iPoint position;
	State state;
	MoveDirection direction;
	Animation* currentAnimation;
	SDL_Texture* texture;
	TypePartner type;
	int level = 1;
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

	bool PreUpdate();

	bool Update(float dt);

	void SpeedAnimationCheck(float dt);

	void MoveBetweenCheckPoints();

	void CameraPlayer();

	void PlayerMoveAnimation(State state, MoveDirection direction, Animation* &currentAnimation);

	void PlayerControls(float dt);

	void MovePlayer(MoveDirection playerDirection, float dt);

	bool PostUpdate();

	bool CleanUp();

	bool CollisionPlayer(iPoint nextPosition);
	bool CheckGameOver(int level);

	void SetHit();

	void CoinPlus() { playerData.coins++; };
	void LivePlus() { playerData.respawns++; };

	void ActiveCheckpoint(iPoint positionMapPlayer);

	iPoint IPointMapToWorld(iPoint ipoint);

	Partner* GetPartners() { return partners; };
	int GetNumPartners() { return numPartners; };
	bool GetInCheckPoint() { return inCheckPoint; };
	bool GetCheckPointMove() { return checkpointMove; };

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

	PlayerData playerData;
	int radiusCollision;
	bool godMode = false;
	iPoint* positionInitial= new iPoint(0,0);	
	bool win= false;

private:
	
	int levelScene;
	float vel = 0;

	// Partners
	int numPartners = 3;
	Partner partners[3];
	MoveDirection lastDirection;
	List<iPoint*> path;
	bool playerCollision = true;
	int diagonal = 0;


	Animation* idleAnimR;
	Animation* idleAnimL;
	Animation* idleAnimUp;
	Animation* idleAnimDown;

	Animation* walkAnimR;
	Animation* walkAnimL;
	Animation* walkAnimUp;
	Animation* walkAnimDown;

	pugi::xml_document playerFile;
	SString folder;
	iPoint tmp;

	uint bonfireFx;
	uint damageFx;

	// CheckPoint's vars
	bool inCheckPoint;
	List<iPoint> checkPoints;
	bool debugCheckPoints;

	int lastCP;
	bool checkpointMove;
	bool endUpdate;

	State lastState;
	MoveDirection hitDirection;


	SDL_Texture* texBrenda;
	SDL_Texture* texIsrra;
	SDL_Texture* texKenzie;
	SDL_Texture* texKeiler;
	List<SDL_Texture*> texPartners;

};

#endif // _PLAYER_H_


