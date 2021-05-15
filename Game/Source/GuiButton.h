#ifndef __GUIBUTTON_H__
#define __GUIBUTTON_H__

#include "GuiControl.h"
#include "Audio.h"
#include "App.h"
#include "SString.h"

enum TypeButton {

	RECTANGLE,
	REMOVE,
	CREDITS,
	EXIT,
	BACK
};


class GuiButton : public GuiControl
{
public:

	GuiButton(uint32 id, SDL_Rect bounds, const char *text, TypeButton typeButton, SDL_Texture* texture = NULL);
	void ResizeButton(int* w, int* h);
	virtual ~GuiButton();

	bool Update(float dt);
	bool Draw();

	void DefinePositionAtlas();
	void PressButtonSound();
	int GetTexButtonW()
	{
		return rectTexW;
	}
	int GetTexButtonH()
	{
		return rectTexH;

	}

private:
	bool mouseIn = false;
	// Gui Button specific properties
	// Maybe some animation properties for state change?
	

	int rectTexW= 183;
	int rectTexH= 91;
	int squareTexW= 88;
	int squareTexH= 88;

	TypeButton typeButton;

	iPoint* rectAtlasPos = new iPoint(87, 99);
	iPoint* removeAtlasPos = new iPoint( 552,713 );
	iPoint* creditAtlasPos = new iPoint(550, 412);
	iPoint* exitAtlasPos = new iPoint(0, 1810);
	iPoint* backAtlasPos = new iPoint( 0, 1114);
	
	SDL_Rect rect = { 0,0,0,0 };

	bool textDisable = false;
	
	int margin = 10;
	int marginSquare = 9;
	float angle = 0;
	float positionY = 0;
};

#endif // __GUIBUTTON_H__
