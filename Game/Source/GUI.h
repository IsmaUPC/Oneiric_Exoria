#ifndef __GUI_H__
#define __GUI_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"

class GUI : public Entity
{
public:

	GUI();
	GUI(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture);

	virtual ~GUI();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	void Chronometer();

	bool CleanUp();

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;

public:

private:

	// Positions vars
	int headSeparation = 10;
	int headPositionX = 20;
	int headPositionY = 20;

	int headW = 0;

	uint* respawn = 0;
	uint* fireBallState = 0;

	iPoint point0 = { 0,0 };
	SDL_Texture* headTex;
	SDL_Texture* arrowTex;
	SDL_Texture* fireBallTex;

	Animation* headAnim;
	Animation* arrowAnim;
	Animation* buttonEAnim;
	Animation* fireBallOnAnim;
	Animation* fireBallOffAnim;

	bool inCheckPoint;

	// Coin
	SDL_Texture* imgCoin;
	Animation* coinHudAnim;

	// Text
	uint* coins = 0;
	int hudFont = 0;
	char coinText[10] = { "\0" };

	bool activeFPS;
	// Time
	Timer timer;
	char timeText[10] = { "\0" };
	char scoreText[12] = { "\0" };
	uint miliseconds=0;
	uint miliseconds2 = 0;
	uint seconds = 0;
	uint minuts=0;
	Timer auxTimePause;
	bool stopTime = false;
};

#endif // __GUI_H__