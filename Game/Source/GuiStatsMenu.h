#ifndef __GUISTATSMENU_H__
#define __GUISTATSMENU_H__

#include "Point.h"
#include "SString.h"

#define BOOK_W 187
#define BOOK_H 169

#include "GuiSettings.h"
#include "Animation.h"

class Pages
{
public:
	int numPage = 1;
	int attack = 0;
	int defense = 0;
	int speed = 0;
	int health = 0;
	int maxHealth = 0;
	int mana = 0;
	int maxMana = 0;
	int exp = 0;
	int maxExp = 0;
};

class GuiStatsMenu
{
public:
	GuiStatsMenu(iPoint position, SceneControl* moduleObserver, SDL_Texture* textureAtlas);
	~GuiStatsMenu();

	bool Update(float dt);
	bool PostUpdate();

	void DrawTitleStats(int posX, int& posY);

	bool CleanUp();

	bool Event(GuiControl* control);

	void AbleDisableMenu();

	void MovePosition();

	bool GetActive() { return active; }

	void ChangePage();
	void InicializeStats();
	int CalculateExp(int level);
	void DrawBarLive();


	Animation* currentAnim = nullptr;
	bool introBook = false;

private:

	iPoint initialPos = { 0,0 };
	iPoint relativePosition = { 0,0 };

	Pages page;
	int maxNumPages = 4;
	char textStats[15] = { "\0" };
	char textDescription[400] = { "\0" };

	SDL_Rect rectBar = { 0,0,0,0 };
	SDL_Color color;
	SDL_Color green = { 0,187, 45 };
	SDL_Color yellow = { 229,190,1 };
	SDL_Color red = { 203,50,52 };

	SceneControl* observer = nullptr;

	SDL_Rect screenRect = { NULL };

	bool active = false;
	bool changingPage = false;
	int wRectBar = 320;
};

#endif // !__GUISTATSMENU_H__