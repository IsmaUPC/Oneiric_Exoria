#include "GuiSlider.h"
#include "Log.h"
#include "SceneManager.h"

#define SPACEBOUNDBAR 10

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds,const char* text = "SLIDER", int min, int max, SDL_Texture* texture) : GuiControl(GuiControlType::SLIDER, id)
{
	this->bounds = bounds;
	this->texture = texture;
	this->text = text;

	if (min > max)
	{
		this->minValue = max;
		this->maxValue = min;
		LOG("The minimum value is higher than the maximum, they have been changed to avoid errors");
	}
	else
	{
		this->minValue = min;
		this->maxValue = max;
	}

	sliderBarInput.x = bounds.x + bounds.w + SPACEBOUNDBAR;
	sliderBarInput.w = 183;
	sliderBarInput.h = 20;
	sliderBarInput.y = bounds.y + (bounds.w / 2) - (sliderBarInput.h / 2);

	sliderBarImage = { 497,2623,183,20 };
	sliderImage = {696,2632,46,46};

	button = { rectAtlasPos->x,rectAtlasPos->y,rectTexW + margin,rectTexH };

	slider.w = sliderImage.w;
	slider.h = sliderImage.h;
	SetValue(minValue);

	this->font = app->sceneManager->GetGuiFont();
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update(float dt)
{
	sliderBarInput.x = bounds.x + bounds.w + SPACEBOUNDBAR;
	sliderBarInput.y = bounds.y + (bounds.h/2) - (sliderBarInput.h / 2);

	SetValue(value);

	if (state != GuiControlState::DISABLED)
	{
		int mouseX, mouseY;
		app->input->GetMousePosition(mouseX, mouseY);

		// Check collision between mouse and button bounds
		if (((mouseX > sliderBarInput.x) && (mouseX < (sliderBarInput.x + sliderBarInput.w)) &&
			(mouseY > sliderBarInput.y) && (mouseY < (sliderBarInput.y + sliderBarInput.h))) || 
			((mouseX > slider.x) && (mouseX < (slider.x + slider.w)) &&
			(mouseY > slider.y) && (mouseY < (slider.y + slider.h))))
		{
			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;
			}

			if (state == GuiControlState::PRESSED)
			{
				SliderControl(mouseX, mouseY);
				NotifyObserver();
			}
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				if (text == "FX")
					app->audio->PlayFx(app->sceneManager->btnSlider);

			}
		}
		else state = GuiControlState::NORMAL;
	}

	return false;
}

bool GuiSlider::Draw()
{
	// Draw SliderBar
	app->render->DrawTexture(texture, sliderBarInput.x, sliderBarInput.y, &sliderBarImage);
	bool drawRectangles = app->sceneManager->GetViewRectangle();

	// Draw the right button depending on state
	switch (state)
	{
	case GuiControlState::DISABLED:
		button.x += 3 * button.w;
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		sliderImage.x += 3 * (sliderImage.w + marginSliders);
		app->render->DrawTexture(texture, slider.x, slider.y, &sliderImage);

		if (drawRectangles)app->render->DrawRectangle(sliderBarInput, 100, 100, 100, 190);
		if (drawRectangles)app->render->DrawRectangle(slider, 100, 100, 100, 190);

		break;
	case GuiControlState::NORMAL:
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		app->render->DrawTexture(texture, slider.x, slider.y, &sliderImage);

		if (drawRectangles)app->render->DrawRectangle(sliderBarInput, 0, 255, 0, 190);
		if (drawRectangles)app->render->DrawRectangle(slider, 0, 255, 0, 190);

		break;
	case GuiControlState::FOCUSED:
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		sliderImage.x += 1 * (sliderImage.w + marginSliders);
		app->render->DrawTexture(texture, slider.x, slider.y, &sliderImage);

		if (drawRectangles)app->render->DrawRectangle(sliderBarInput, 255, 255, 0, 190);
		if (drawRectangles)app->render->DrawRectangle(slider, 255, 255, 0, 190);

		break;
	case GuiControlState::PRESSED:
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		sliderImage.x += 2 * (sliderImage.w + marginSliders);
		app->render->DrawTexture(texture, slider.x, slider.y, &sliderImage);

		if (drawRectangles)app->render->DrawRectangle(sliderBarInput, 0, 255, 255, 190);
		if (drawRectangles)app->render->DrawRectangle(slider, 0, 255, 255, 190);

		break;
	case GuiControlState::SELECTED:
		if (drawRectangles)app->render->DrawRectangle(sliderBarInput, 255, 255, 0, 190);
		if (drawRectangles)app->render->DrawRectangle(slider, 255, 255, 0, 190);

		break;
	default:
		break;
	}
	button.x = rectAtlasPos->x;
	sliderImage.x = 696;

	int centradoY, centradoX;
	// 14 = a letter's width
	centradoX = (bounds.w / 2) - (((float)(text.Length() / 2) + 0.5f) * 14);	
	// 48 = height image of font, whith 2 Raws, 48/2 = half a letter's height
	centradoY = (bounds.h / 2) - (48 / 4);
	app->fonts->BlitText(bounds.x + centradoX, bounds.y + centradoY, font, text.GetString());

	return false;
}

void GuiSlider::SliderControl(int mouseX, int mouseY)
{

	slider.x = mouseX - (slider.w / 2);

	value = ((maxValue - minValue) * (mouseX - (float)(sliderBarInput.x + slider.w / 2))) / (float)(sliderBarInput.w - slider.w) + minValue;

	// Limits
	if (slider.x < sliderBarInput.x)
	{
		slider.x = sliderBarInput.x;
		value = minValue;
	}
	if ((slider.x + slider.w) > (sliderBarInput.x + sliderBarInput.w))
	{
		slider.x = (sliderBarInput.x + sliderBarInput.w) - slider.w;
		value = maxValue;
	}

}

void GuiSlider::SetValue(int newValue)
{
	this->value = newValue;
	float X, h, h1, h2, h3, i;
	h1 = (((sliderBarInput.w - slider.w) + minValue) * (maxValue - minValue));
	h2 = maxValue - minValue;
	h3 = (sliderBarInput.w - slider.w) + minValue;

	h = (h1 * h2) / h3;
	i = (((sliderBarInput.w - slider.w) + minValue) * (maxValue - minValue)) * value;

	X = (i / h) + (sliderBarInput.x + (slider.w / 2));
	slider.x = (int)X-23;
	slider.y = sliderBarInput.y+(sliderBarInput.h/2) - (slider.h/2);
}
