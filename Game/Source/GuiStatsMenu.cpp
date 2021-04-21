#include "App.h"
#include "GuiStatsMenu.h"
#include "Audio.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

GuiStatsMenu::GuiStatsMenu(iPoint Position, SceneControl* moduleObserver, SDL_Texture* textureAtlas)
{
	screenRect = { 0, 0, WINDOW_W, WINDOW_H };

	initialPos = Position;

	observer = moduleObserver;

	active = false;
}

GuiStatsMenu::~GuiStatsMenu()
{
}

bool GuiStatsMenu::Update(float dt)
{
	bool ret = true;

	if (active)
	{
		//Update stuff
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			app->sceneManager->SetPause(false);
			AbleDisableMenu();
		}
	}

	return ret;
}

bool GuiStatsMenu::PostUpdate()
{

	if (active)
	{
		screenRect.x = -app->render->camera.x;
		screenRect.y = -app->render->camera.y;

		bookAnimRect = { 0, 0, BOOK_W, BOOK_H };
		app->render->DrawTexture(app->guiManager->bookMenu, screenRect.x, screenRect.y, &bookAnimRect);

	}

	return true;
}

bool GuiStatsMenu::CleanUp()
{
	active = false;

	return true;
}

bool GuiStatsMenu::Event(GuiControl* control)
{

	//Button stuff

	return true;
}

void GuiStatsMenu::AbleDisableMenu()
{
	active = !active;

	if (active == true)
	{
		MovePosition();
	}
}

void GuiStatsMenu::MovePosition()
{
	int x = -app->render->camera.x + initialPos.x;
	int y = -app->render->camera.y + initialPos.y;

}