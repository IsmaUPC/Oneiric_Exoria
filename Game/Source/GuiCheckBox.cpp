#include "GuiCheckBox.h"
#include "SceneManager.h"

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
	checkBoxInput.y = bounds.y;

	int mouseX, mouseY;
	app->input->GetMousePosition(mouseX, mouseY);

	// Check collision between mouse and button bounds
	if ((mouseX > checkBoxInput.x) && (mouseX < (checkBoxInput.x + checkBoxInput.w)) &&
		(mouseY > checkBoxInput.y) && (mouseY < (checkBoxInput.y + checkBoxInput.h)))
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
				checked = !checked;
				ret = NotifyObserver();
				app->audio->PlayFx(app->sceneManager->btnPressed);
			}
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
			app->audio->PlayFx(app->sceneManager->btnDisabled);
	}
	else if (state != GuiControlState::DISABLED) state = GuiControlState::NORMAL, mouseIn = false;

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
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		checkBoxImage.x += 3 * (checkBoxImage.w+marginSquare);
		app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);
		textDisable = true;

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 100, 100, 100, 190);
		break;
	case GuiControlState::NORMAL: 
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 0, 255, 0, 190);
		break;
	case GuiControlState::FOCUSED: 
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		checkBoxImage.x += 1 * (checkBoxImage.w + marginSquare);
		app->render->DrawTexture(texture, checkBoxInput.x, checkBoxInput.y, &checkBoxImage);

		if (drawRectangles)app->render->DrawRectangle(checkBoxInput, 255, 255, 0, 190);
		break;
	case GuiControlState::PRESSED:
		app->render->DrawTexture(texture, bounds.x, bounds.y, &button);

		checkBoxImage.x += 2 * (checkBoxImage.w + marginSquare);
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
	app->fonts->BlitText(bounds.x + centradoX, bounds.y + centradoY, font, text.GetString(), textDisable);


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
