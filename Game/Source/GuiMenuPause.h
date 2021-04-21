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
	bool PostUpdate();

	bool CleanUp();

	bool Event(GuiControl* control);

	void AbleDisableMenu();
	void AbleDisableSetting();

	void CloaseMenuSettings();

	void MovePosition();

	bool GetActive() { return active; }

private:
	int padding = 40;
	GuiButton* btnResume = nullptr;
	GuiButton* btnSettings = nullptr;
	GuiButton* btnBackToTitle = nullptr;
	GuiButton* btnSave = nullptr;
	GuiButton* btnExit = nullptr;

	GuiSettings* menuSettings = nullptr;

	iPoint initialPos = { 0,0 };

	SceneControl* observer = nullptr;

	SDL_Rect screenRect = { NULL };

	bool active = false;
	bool activeSettings = false;
};

#endif // !__GUIMENUPAUSE_H__

