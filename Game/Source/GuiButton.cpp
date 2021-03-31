#include "GuiButton.h"
#include "SceneManager.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, const char* text, TypeButton typeButton, SDL_Texture* texture ) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->texture = texture;
	this->typeButton = typeButton;
	this->font = app->sceneManager->GetGuiFont();

}

GuiButton::~GuiButton()
{
}

bool GuiButton::Update(float dt)
{
	bool ret = true;

	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	// Check collision between mouse and button bounds
	if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
		(mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h)))
	{
		if (state != GuiControlState::DISABLED)
		{
			state = GuiControlState::FOCUSED;
			if (!mouseIn)app->audio->PlayFx(app->sceneManager->btnSelected), mouseIn = true;
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;

			}

			// If mouse button pressed -> Generate event!
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				app->audio->PlayFx(app->sceneManager->btnPressed);
				ret = NotifyObserver();
			}
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			app->audio->PlayFx(app->sceneManager->btnDisabled);
	}
	else if (state != GuiControlState::DISABLED) state = GuiControlState::NORMAL, mouseIn = false;


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
		app->render->DrawTexture(texture, bounds.x, bounds.y, &rect);

		textDisable = true;
		if (drawRectangles)app->render->DrawRectangle(bounds, 100, 100, 100, 190);
		break;
	case GuiControlState::NORMAL: 
		app->render->DrawTexture(texture, bounds.x, bounds.y, &rect);
		if (drawRectangles)app->render->DrawRectangle(bounds, 0, 255, 0, 190);
		break;
	case GuiControlState::FOCUSED: 
		rect.x+= 1*rect.w;
		app->render->DrawTexture(texture, bounds.x, bounds.y, &rect);
		if (drawRectangles)app->render->DrawRectangle(bounds, 255, 255, 0, 190);
		break;
	case GuiControlState::PRESSED:
		rect.x+= 2 * rect.w;
		app->render->DrawTexture(texture, bounds.x, bounds.y, &rect);
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
	app->fonts->BlitText(bounds.x + centerX, bounds.y + centerY, font, text.GetString(), textDisable);

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
	app->audio->PlayFx(app->sceneManager->btnPressed);
}
