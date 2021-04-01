#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"

struct PlayerData
{
	iPoint position;
	State state;
	MoveDirection direction;
	Animation* currentAnimation;
	float velocity ;
	uint lives = 0;
	uint respawns = 0;
	uint coins = 0;

	SDL_Texture* texture;
	static const int numPoints = 4;

	iPoint pointsCollision[numPoints] = { { 0,0 },{32 , 0},{ 32,44 },{0 ,44 } };

};


class Player : public Entity
{
public:

	Player();
	Player(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture);
	virtual ~Player();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	void SpeedAnimationCheck(float dt);

	void MoveBetweenCheckPoints();

	void CameraPlayer();

	void PlayerMoveAnimation();

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


	bool GetInCheckPoint() { return inCheckPoint; };
	bool GetCheckPointMove() { return checkpointMove; };

private:

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;

	void MoveToDirection();

	void DebugCP();

public:

	PlayerData playerData;
	bool godMode = false;
	iPoint* positionInitial= new iPoint(0,0);	
	bool win= false;

private:
	
	int levelScene;

	float velY = 0;
	float velX = 0;

	Animation* idleAnimR = new Animation();
	Animation* idleAnimL = new Animation();
	Animation* idleAnimUp = new Animation();
	Animation* idleAnimDown = new Animation();

	Animation* walkAnimR = new Animation();
	Animation* walkAnimL = new Animation();
	Animation* walkAnimUp = new Animation();
	Animation* walkAnimDown = new Animation();

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
};

#endif // _PLAYER_H_


