#include "GuiButton.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "Fonts.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text, TypeButton typeButton, SDL_Texture* texture ) : GuiControl(GuiControlType::BUTTON, id)
{
	this->texture = texture;
	this->typeButton = typeButton;
	this->font = app->sceneManager->GetGuiFont();
	this->bounds = bounds;
	TTF_SizeText(this->font, text, &this->bounds.w, &this->bounds.h);
	ResizeButton(&this->bounds.w, &this->bounds.h);
	this->text = text;
}

void GuiButton::ResizeButton(int* w, int* h)
{
	bounds.w = *w;
	bounds.h = *h;
	bounds.x -= 5;
	bounds.y -= 5;
	bounds.w += 10;
	bounds.h += 10;
	bounds.x -= bounds.w / 2;
}

GuiButton::~GuiButton()
{
}

bool GuiButton::Update(float dt)
{
	bool ret = true;
	GamePad& pad = app->input->pads[0];
	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	// Check collision between mouse and button bounds
	if (state == GuiControlState::FOCUSED)
	{
		if (state != GuiControlState::DISABLED)
		{
			if (!mouseIn)app->audio->PlayFx(app->guiManager->fxBtnSelected), mouseIn = true; // && !app->guiManager->missClick

			if (pad.a && !app->guiManager->press)
			{
				state = GuiControlState::PRESSED;
				padPreset = true;
			}

			if ((app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) && !app->guiManager->missClick)
			{
				//app->guiManager->press = true;
				state = GuiControlState::PRESSED;
			}
			// If mouse button pressed -> Generate event!
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP && !app->guiManager->missClick)
			{
				app->guiManager->missClick = true;
				app->audio->PlayFx(app->guiManager->fxBtnPressed);
				ret = NotifyObserver();
				app->guiManager->press = true;
				lastState == GuiControlState::FOCUSED;
			}
		}
	}

	if (state == GuiControlState::PRESSED && !app->guiManager->missClick)
	{
		// If mouse button pressed -> Generate event!
		if (!pad.a && padPreset){
			padPreset = false;
			app->audio->PlayFx(app->guiManager->fxBtnPressed);
			ret = NotifyObserver();
			app->guiManager->press = true;
			lastState == GuiControlState::FOCUSED;
		}
		if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_UP || app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
		{
			app->audio->PlayFx(app->guiManager->fxBtnPressed);
			ret = NotifyObserver();
			lastState == GuiControlState::FOCUSED;
		}
	}

	if(state == GuiControlState::NORMAL) mouseIn = false;

	if (state == GuiControlState::FOCUSED)
	{
		angle += dt*5;
		positionY = bounds.y + 4*cos(angle);
	}
	else
	{
		angle = 0;
		positionY = 0;
	}
		

	return ret;
}

bool GuiButton::Draw()
{
	DefinePositionAtlas();
	bool drawRectangles = app->sceneManager->GetViewRectangle();
	textDisable=false;

	// Draw the right button depending on state
	switch (state)
	{
	case GuiControlState::DISABLED: 
		rect.x+= 3 * rect.w;

		textDisable = true;
		if (drawRectangles)app->render->DrawRectangle(bounds, 100, 100, 100, 190);
		break;
	case GuiControlState::NORMAL: 
		if (drawRectangles)app->render->DrawRectangle(bounds, 0, 255, 0, 190);
		break;
	case GuiControlState::FOCUSED: 
		rect.x+= 1*rect.w;
		app->render->DrawTexture(app->guiManager->handCursor, bounds.x - 35, bounds.y + 5, &app->guiManager->handAnim->GetCurrentFrame());
		if (drawRectangles)app->render->DrawRectangle(bounds, 255, 255, 0, 190);
		break;
	case GuiControlState::PRESSED:
		rect.x+= 2 * rect.w;
		app->render->DrawTexture(app->guiManager->handCursor, bounds.x - 35, bounds.y + 5, &app->guiManager->handAnim->GetCurrentFrame());
		if (drawRectangles)app->render->DrawRectangle(bounds, 0, 255, 255, 190);
		break;
	case GuiControlState::SELECTED:
		if (drawRectangles)app->render->DrawRectangle(bounds, 255, 255, 0, 190);
		break;
	default:
		break;
	}

	int centerY,centerX;
	centerX = (bounds.w / 2) - (((float)(text.Length() / 2)+0.5f) * 14);
	// 48 = height image of font, whith 2 Raws, 48/2 = half a letter's height
	centerY = (bounds.h/2)-(48/4);
	
	if(app->sceneManager->onDialog) app->fonts->BlitText(bounds.x + 5, bounds.y + 5, 0, text.GetString(), { 60, 43, 13 });
	else if(positionY == 0 && state != GuiControlState::FOCUSED && state != GuiControlState::DISABLED) app->fonts->BlitText(bounds.x + 5, bounds.y + 5, 0, text.GetString(), {60, 43, 13}, app->guiManager->dt * 20);
	else if(state != GuiControlState::DISABLED) app->fonts->BlitText(bounds.x + 5, positionY + 5, 0, text.GetString(), { 199, 147, 55 });
	if(state == GuiControlState::PRESSED) app->fonts->BlitText(bounds.x + 5, bounds.y + 5, 0, text.GetString(), { 38, 79, 85 });
	if(state == GuiControlState::DISABLED) app->fonts->BlitText(bounds.x + 5, bounds.y + 5, 0, text.GetString(), { 88, 88, 88 });

	return true;
}

void GuiButton::DefinePositionAtlas()
{
	switch (typeButton)
	{
	case RECTANGLE:
		rect = { rectAtlasPos->x,rectAtlasPos->y,rectTexW + margin,rectTexH };
		break;
	case REMOVE:
		rect = { removeAtlasPos->x,removeAtlasPos->y,squareTexW + marginSquare,squareTexH };
		break;
	case CREDITS:
		rect = { creditAtlasPos->x,creditAtlasPos->y,squareTexW + marginSquare,squareTexH };
		break;
	case EXIT:
		rect = { exitAtlasPos->x,exitAtlasPos->y,squareTexW + marginSquare,squareTexH };
		break;
	case BACK:
		rect = { backAtlasPos->x,backAtlasPos->y,squareTexW + marginSquare,squareTexH };
		break;
	default:
		break;
	}
}

void GuiButton::PressButtonSound()
{
	app->audio->PlayFx(app->guiManager->fxBtnPressed);
}
