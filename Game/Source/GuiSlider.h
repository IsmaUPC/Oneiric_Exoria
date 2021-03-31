#ifndef __GUISLIDER_H__
#define __GUISLIDER_H__

#include "GuiControl.h"
#include "Audio.h"

#include "Point.h"
#include "SString.h"

class GuiSlider : public GuiControl
{
public:

	 GuiSlider(uint32 id, SDL_Rect bounds, const char* text, int min=0, int max=100, SDL_Texture* texture = NULL);
	 virtual ~GuiSlider();

	 bool Update(float dt);
	 bool Draw();

	 void SliderControl(int mouseX, int mouseY);

	 int GetValue() { return value; };

	 // Put the slider in the correct position respected the value && change the value
	 void SetValue(int newValue);

private:

	 // GuiSlider specific properties
	 // Maybe some animation properties for state change?
	 iPoint* rectAtlasPos = new iPoint(87, 99);
	 int margin = 10;
	 SDL_Rect button;
	 int rectTexW = 183;
	 int rectTexH = 91;


	 SDL_Rect slider;
	 SDL_Rect sliderImage;
	 int marginSliders = 4;


	 SDL_Rect sliderBarInput;
	 SDL_Rect sliderBarImage;


	 int value;
	 int lastValue;

	 int minValue;
	 int maxValue;

};

#endif // __GUISLIDER_H__
