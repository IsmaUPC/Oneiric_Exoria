#ifndef __FIREBALL_H__
#define __FIREBALL_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"

enum StateShoot {

	CAN_SHOOT,
	SHOOT,
	WAIT,
};

class FireBall : public Entity
{
public:
	FireBall();
	FireBall(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture);

	virtual ~FireBall();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void SetPosition(iPoint pos);

	void BackToPos0();

	void Shoot();


	uint stateShoot;
	uint lastState;
private:

	int direc;

	iPoint startPos;
	iPoint explosionPos;
	Animation* fireBallAnim = new Animation();
	Animation* explosionAnim = new Animation();

	SDL_Texture* explsionTex;

	int texW, texH;
	int cooldown;

	bool startexplosion;

	uint hitFx;
	uint shootFx;

	Timer frameTime;
	Timer auxTimePause;
	bool stopTime = false;
};

#endif // __FIREBALL__H__
