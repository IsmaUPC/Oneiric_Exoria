#ifndef __GUISTATSMENU_H__
#define __GUISTATSMENU_H__

#include "Point.h"
#include "SString.h"

#define BOOK_W 748
#define BOOK_H 676

#include "GuiSettings.h"

class GuiStatsMenu
{
public:
	GuiStatsMenu(iPoint Position, SceneControl* moduleObserver, SDL_Texture* textureAtlas);
	~GuiStatsMenu();

	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp();

	bool Event(GuiControl* control);

	void AbleDisableMenu();

	void MovePosition();

	bool GetActive() { return active; }

private:

	iPoint initialPos = { 0,0 };

	SceneControl* observer = nullptr;

	SDL_Rect bookAnimRect;

	SDL_Rect screenRect = { NULL };

	bool active = false;
};

#endif // !__GUISTATSMENU_H__