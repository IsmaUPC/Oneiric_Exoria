#ifndef __GUICHECKBOX_H__
#define __GUICHECKBOX_H__

#include "GuiControl.h"
#include "Audio.h"

#include "Point.h"
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

private:

	void ChangeImageChecked();

	// GuiCheckBox specific properties
	iPoint* rectAtlasPos = new iPoint(87, 99);
	int margin = 10;

	SDL_Rect button;
	int rectTexW = 183;
	int rectTexH = 91;

	SDL_Rect checkBoxInput;
	SDL_Rect checkBoxImage;

	iPoint* falseAtlasPos = new iPoint(227, 201);
	iPoint* trueAtlasPos = new iPoint(552, 1407);
	int squareTexW = 88;
	int squareTexH = 88;
	int marginSquare = 9;

	bool textDisable = false;

	bool mouseIn = false;
	bool checked;
};

#endif // __GUICHECKBOX_H__
