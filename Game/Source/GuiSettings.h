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

	void AbleDisableSetting() { active = !active; };
	bool GetActiveSettings() { return active; };

	void MovePosition();


	GuiSlider* sldMusic;
	GuiSlider* sldFx;
	GuiCheckBox* chBxFullScreen;
	GuiCheckBox* chBxVSync;

	GuiButton* btnBack;

private:
	int padding = 98;

	iPoint initialPos;

	bool active;

};
#endif // __GUISETTINGS_H__



