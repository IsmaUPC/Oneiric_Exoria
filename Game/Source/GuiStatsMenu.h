#ifndef __GUISTATSMENU_H__
#define __GUISTATSMENU_H__

#include "SString.h"

#define BOOK_W 187
#define BOOK_H 169

#include "GuiSettings.h"
#include "Animation.h"
#include "GuiMenuMagic.h"
#include "ItemManager.h"

enum PageType
{
	STATS = 1,
	INVENTORY = 2
};

class  InventoryPage
{
	GameItem* currentItem = nullptr;
};

class StatsPage
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
	int level = 0;
	SDL_Rect avatar;
};

class GuiStatsMenu
{
public:
	GuiStatsMenu(iPoint position, SceneControl* moduleObserver, SDL_Texture* textureAtlas);
	~GuiStatsMenu();

	bool Update(float dt);
	void UpdateInventory();
	void UpdateStats();
	void CloseBook();
	bool PostUpdate();

	void DrawTitleStats(int posX, int& posY);
	void DrawBookMarks(int posX, int& posY);
	void DrawInventory(int posX, int& posY);

	bool CleanUp();

	bool Event(GuiControl* control);

	void AbleDisableMenu();

	void MovePosition();

	bool GetActive() { return active; }

	void ChangeStatCharacter();
	void InicializeStats();
	int CalculateExp(int level);
	void DrawBarLive();

	void ChangePages();

	Animation* currentAnim = nullptr;
	bool introBook = false;

private:

	iPoint initialPos = { 0,0 };
	iPoint relativePosition = { 0,0 };

	GuiMenuMagic* menuMagic = nullptr;

	StatsPage page;
	PageType pageType = INVENTORY;

	int maxNumPages = 4;
	char textStats[15] = { "\0" };
	char textName[7] = { "\0" };
	char textClass[10] = { "\0" };
	char textMagicRunes[10] = { "\0" };
	char textDescription[400] = { "\0" };
	char textItemName[30] = { "\0" };
	char itemQuantity[3] = { "\0" };

	SDL_Rect rectBar = { 0,0,0,0 };
	SDL_Color color;
	SDL_Color green = { 0,187, 45 };
	SDL_Color yellow = { 211,159,0 };
	SDL_Color red = { 203,50,52 };

	SceneControl* observer = nullptr;

	SDL_Rect screenRect = { NULL };

	bool active = false;
	bool changingPage = false;
	bool closingBook = false;
	int wRectBar = 320;

	float dt = 0;
	int velocityDraw = 1;

	float liveW = 0;
	float manaW = 0;
	float expW = 0;
};

#endif // !__GUISTATSMENU_H__