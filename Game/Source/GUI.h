#ifndef __GUI_H__
#define __GUI_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"

class GUI : public Entity
{
public:

	GUI();
	GUI(Entity* entity, SDL_Texture* pTexture);

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

	iPoint point0 = { 0,0 };

	bool inCheckPoint = false;

	SDL_Texture* playerUi = nullptr;
	char textLive[8] = { "\0" };

	bool activeFPS = false;
	char fps[3] = { "\0" };
	// Time
	Timer timer;
	char timeText[10] = { "\0" };
	char scoreText[12] = { "\0" };
	uint miliseconds = 0;
	uint miliseconds2 = 0;
	uint seconds = 0;
	uint minuts = 0;
	Timer auxTimePause;
	bool stopTime = false;
};

#endif // __GUI_H__