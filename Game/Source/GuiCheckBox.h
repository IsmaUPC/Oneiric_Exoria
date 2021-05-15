#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__

#include "GuiControl.h"
#include "Audio.h"

#include "SString.h"

class GuiCheckBox : public GuiControl
{
public:

	GuiCheckBox(uint32 id, SDL_Rect bounds, const char *text, bool initState = false, SDL_Texture* texture = NULL);
	virtual ~GuiCheckBox();

	bool Update(float dt);
	bool Draw();

	bool GetValue() { return checked; };
	void SetValue(bool state) { checked = state; };
	SDL_Rect GetCheckBoxInput() { return checkBoxInput; };

private:

	void ChangeImageChecked();

	// GuiCheckBox specific properties
	iPoint* rectAtlasPos = new iPoint(87, 99);
	int margin = 10;

	SDL_Rect button = {0,0,0,0};
	int rectTexW = 183;
	int rectTexH = 91;

	SDL_Rect checkBoxInput = { 0,0,0,0 };
	SDL_Rect checkBoxImage = { 0,0,0,0 };

	iPoint* falseAtlasPos = new iPoint(227, 201);
	iPoint* trueAtlasPos = new iPoint(552, 1407);
	int squareTexW = 32;
	int squareTexH = 32;
	int marginSquare = 9;

	bool textDisable = false;

	bool mouseIn = false;
	bool checked = false;
};

#endif // __GUICHECKBOX_H__
