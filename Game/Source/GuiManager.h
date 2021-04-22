#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"

#include "GuiControl.h"
#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiMenuPause.h"
#include "GuiStatsMenu.h"
#include "GuiSlider.h"
#include "Animation.h"

#include "List.h"

#define SPACEBOUNDBAR 0

class GuiManager : public Module
{
public:

	GuiManager(Input* input, Render* render);

	// Destructor
	virtual ~GuiManager();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	void DeleteList();

	void SelectControl();
	void ComprobeMouseOnControl(GamePad& pad);
	void ReAssignState(int i, GamePad& pad);

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type);

	void AddGuiButton(GuiButton* button);
	void AddGuiCheckBox(GuiCheckBox* checkBox);
	void AddGuiSlider(GuiSlider* slider);

	void DestroyGuiControl(GuiControl* entity);

	void CreatMenuPause(SceneControl* current);
	void CreateStatsMenu(SceneControl* current);
	GuiMenuPause* GetMenuPause() 
	{
		return menu;
	};
	GuiStatsMenu* GetStatsMenu()
	{
		return stats;
	};
	void SetMenuPause(GuiMenuPause* pMenu)
	{
		 menu= pMenu;
	};

public:

	List<GuiControl*> controls;
	List<GuiButton*> buttons;
	List<GuiCheckBox*> checkBoxs;
	List<GuiSlider*> sliders;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	uint btnSelected = 0;
	uint btnPressed = 0;
	uint btnDisabled = 0;
	uint btnSlider = 0;

	SDL_Texture* btnTextureAtlas = nullptr;
	SDL_Texture* moonCorner = nullptr;
	SDL_Texture* uiAtlas = nullptr;
	SDL_Texture* handCursor = nullptr;
	SDL_Texture* bookMenu = nullptr;
	Animation* handAnim = nullptr;

	Animation* openBookAnim = nullptr;
	Animation* idleBook = nullptr;
	Animation* rightBook = nullptr;
	Animation* leftBook = nullptr;

	bool press = false;

private:
	Input* input = nullptr;
	Render* render = nullptr;
	GuiMenuPause* menu = nullptr;
	GuiStatsMenu* stats = nullptr;
};
#endif // __GUIMANAGER_H__
