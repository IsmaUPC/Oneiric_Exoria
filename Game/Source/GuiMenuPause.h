#ifndef __GUIMENUPAUSE_H__
#define __GUIMENUPAUSE_H__

#include "Point.h"
#include "SString.h"

#include "GuiSettings.h"

class GuiMenuPause
{
public:
	GuiMenuPause(iPoint Position,SceneControl* moduleObserver, SDL_Texture* textureAtlas);
	~GuiMenuPause();

	bool Update(float dt);
	bool Draw();

	bool CleanUp();

	bool Event(GuiControl* control);

	void AbleDisableSetting() { active = !active; };
	void AbleDisableMenu() { activeMenu = !activeMenu; };

	void CloaseMenuSettings();

	void MovePosition();

private:
	int padding = 98;
	GuiButton* btnResume;
	GuiButton* btnSettings;
	GuiButton* btnBackToTitle;
	GuiButton* btnExit;

	GuiSettings* menuSettings;

	iPoint initialPos;

	SceneControl* observer;

	SDL_Rect screenRect = { NULL };

	bool active;
	bool activeMenu;

};

#endif // !__GUIMENUPAUSE_H__

