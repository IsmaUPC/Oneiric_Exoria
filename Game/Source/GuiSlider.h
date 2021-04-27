#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "GuiControl.h"
#include "Audio.h"

#include "Point.h"
#include "SString.h"

#define MIX_MAX_VOLUME 128

class GuiSlider : public GuiControl
{
public:

	 GuiSlider(uint32 id, SDL_Rect bounds, const char* text, int min=0, int max= MIX_MAX_VOLUME, SDL_Texture* texture = NULL);
	 virtual ~GuiSlider();

	 bool Update(float dt);
	 bool Draw();

	 void SliderControl(int mouseX, int mouseY,GamePad pad);

	 int GetValue() { return value; };
	 SDL_Rect GetSlider() { return slider; };
	 SDL_Rect GetsliderBarInput() { return sliderBarInput; };

	 // Put the slider in the correct position respected the value && change the value
	 void SetValue(int newValue);

private:

	 // GuiSlider specific properties
	 // Maybe some animation properties for state change?
	 iPoint* rectAtlasPos = new iPoint(87, 99);
	 int margin = 10;
	 SDL_Rect button = {0,0,0,0};
	 int rectTexW = 183;
	 int rectTexH = 91;
	 bool mouseIn = false;

	 SDL_Rect slider = { 0,0,0,0 };
	 SDL_Rect sliderImage = { 0,0,0,0 };
	 int marginSliders = 4;


	 SDL_Rect sliderBarInput = { 0,0,0,0 };
	 SDL_Rect sliderBarImage = { 0,0,0,0 };


	 int value = 0;
	 int lastValue = 0;

	 int minValue = 0;
	 int maxValue = 0;
};

#endif // __GUISLIDER_H__
