#ifndef __GUISETTINGS_H__
#define __GUISETTINGS_H__

#include "Point.h"
#include "SString.h"

#include "GuiButton.h"
#include "GuiCheckBox.h"
#include "GuiSlider.h"


class GuiSettings
{
public:

	GuiSettings(iPoint Position, SceneControl* moduleObserver);
	
	~GuiSettings();

	bool Update(float dt);
	bool Draw();

	bool CleanUp();

	void AbleDisableSetting();
	bool GetActiveSettings() { return active; };

	void MovePosition();


	GuiSlider* sldMusic = nullptr;
	GuiSlider* sldFx = nullptr;
	GuiCheckBox* chBxFullScreen = nullptr;
	GuiCheckBox* chBxVSync = nullptr;

	GuiButton* btnBack = nullptr;

	bool active = false;

private:
	int padding = 60;

	iPoint initialPos = { 0,0};

};
#endif // __GUISETTINGS_H__



