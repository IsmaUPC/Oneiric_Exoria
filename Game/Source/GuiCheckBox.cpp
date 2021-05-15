#include "GuiCheckBox.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "Fonts.h"

GuiCheckBox::GuiCheckBox(uint32 id, SDL_Rect bounds, const char* text, bool initState, SDL_Texture* texture) : GuiControl(GuiControlType::CHECKBOX, id)
{
	this->bounds = bounds;
	this->texture = texture;
	this->text = text;


	button = { rectAtlasPos->x,rectAtlasPos->y,rectTexW + margin,rectTexH };

	checkBoxInput.x = bounds.x + bounds.w + (bounds.w / 4);
	checkBoxInput.y = bounds.y;

	checkBoxInput.w = squareTexW;
	checkBoxInput.h = squareTexH;

	checkBoxImage = { 0,0,squareTexW,squareTexH };


	this->font = app->sceneManager->GetGuiFont();

	checked = initState;

	ChangeImageChecked();
}

GuiCheckBox::~GuiCheckBox()
{

}

bool GuiCheckBox::Update(float dt)
{
	bool ret = true;
	checkBoxInput.x = bounds.x + bounds.w + (bounds.w / 4);
	checkBoxInput.y = bounds.y - 3;

	GamePad& pad = app->input->pads[0];
	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	// Check collision between mouse and button bounds
	if (state == GuiControlState::FOCUSED)
	{
		if (state != GuiControlState::DISABLED)
		{
			if (!mouseIn)app->audio->PlayFx(app->guiManager->fxBtnSelected), mouseIn = true;
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT || pad.a || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
			{
				state = GuiControlState::PRESSED;
			}

			// If mouse button pressed -> Generate event!
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP || pad.a || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
			{
				app->audio->PlayFx(app->guiManager->fxBtnPressed);
				checked = !checked;
				state = GuiControlState::NORMAL;
				ret = NotifyObserver();
			}
		}
	}
	if (state == GuiControlState::NORMAL) mouseIn = false;

	return ret;
}

bool GuiCheckBox::Draw()
{
	ChangeImageChecked();
	bool drawRectangles = app->sceneManager->GetViewRectangle();
	textDisable = false;

	// Draw the right button depending on state
	switch (state)
	{
	case GuiControlState::DISABLED:
		button.x += 3 * button.w;
		//app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		//checkBoxImage.x += 3 * (checkBoxImage.w+marginSquare);
		checkBoxImage = { 160,64,32,32 };
		app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage); 
		textDisable = true;

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 100, 100, 100, 190);
		break;
	case GuiControlState::NORMAL: 
		//app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		if (checked)
		{
			checkBoxImage = { 0, 64, 32, 32 };
			app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);
		}
		else
		{
			checkBoxImage = { 96, 64, 32, 32 };
			app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);
		}

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 0, 255, 0, 190);
		break;
	case GuiControlState::FOCUSED: 
		//app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		//checkBoxImage.x += 1 * (checkBoxImage.w + marginSquare);
		if (checked)
		{
			checkBoxImage = { 32, 64, 32, 32 };
			app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);
		}
		else
		{
			checkBoxImage = { 64, 64, 32, 32 };
			app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);
		}

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 255, 255, 0, 190);
		break;
	case GuiControlState::PRESSED:
		//app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		//checkBoxImage.x += 2 * (checkBoxImage.w + marginSquare);
		checkBoxImage = { 128, 64, 32, 32 };
		app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 0, 255, 255, 190);
		break;
	case GuiControlState::SELECTED:

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 255, 255, 0, 190);
		break;
	default:
		break;
	}

	int centradoY, centradoX;
	centradoX = (bounds.w / 2) - (((float)(text.Length() / 2) + 0.5f) * 14);
	// 48 = height image of font, whith 2 Raws, 48/2 = half a letter's height
	centradoY = (bounds.h / 2) - (48 / 4);
	app->fonts->BlitText(bounds.x, bounds.y, 0, text.GetString(), { 60, 43, 13 });


	return false;
}

void GuiCheckBox::ChangeImageChecked()
{
	if (checked)
	{
		checkBoxImage.x = trueAtlasPos->x;
		checkBoxImage.y = trueAtlasPos->y;
	}
	else
	{
		checkBoxImage.x = falseAtlasPos->x;
		checkBoxImage.y = falseAtlasPos->y;
	}
	button.x = rectAtlasPos->x;

}
