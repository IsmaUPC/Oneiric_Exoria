#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"
#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiMenuPause.h"
#include "GuiSlider.h"

#include "List.h"

#define SPACEBOUNDBAR 10

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
	GuiMenuPause* GetMenuPause() 
	{
		return menu;
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

	uint btnSelected;
	uint btnPressed;
	uint btnDisabled;
	uint btnSlider;

	SDL_Texture* btnTextureAtlas;

	bool press;

private:
	Input* input;
	Render* render;
	GuiMenuPause* menu;

};
#endif // __GUIMANAGER_H__
