#ifndef __GUIMENUPAUSE_H__
#define __GUIMENUPAUSE_H__

#include "SString.h"
#include "GuiSettings.h"

class GuiMenuPause
{
public:
	GuiMenuPause(iPoint Position,SceneControl* moduleObserver, SDL_Texture* textureAtlas);
	~GuiMenuPause();

	bool Update(float dt);
	void UpdateSpawnPosition();
	void AssignRandomAnimation();
	bool PostUpdate();

	bool CleanUp();

	bool Event(GuiControl* control);

	void AbleDisableMenu();
	void DisableButtons();
	void AbleDisableSetting();

	void CloaseMenuSettings();

	void MovePosition();

	bool GetActive() { return active; }
	void SetActive(bool active_) { active = active_; }

	GuiSettings* GetMenuSettings() { return menuSettings; }

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
	bool activePause = false;
	bool doorRand = true;
	int randT = 0;
	float offsetSpawnX = 0;
	float offsetSpawnY = 0;

	// Easings variables
	int currentIteration = 0;
	int totalIterations = 80;
	int spawnPos = 0;
};

#endif // !__GUIMENUPAUSE_H__

