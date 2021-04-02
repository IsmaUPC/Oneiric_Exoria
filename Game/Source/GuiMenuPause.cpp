#include "App.h"
#include "GuiMenuPause.h"
#include "Audio.h"
#include "SceneManager.h"
#include "SceneControl.h"

#include "Defs.h"
#include "Log.h"

GuiMenuPause::GuiMenuPause(iPoint Position, SceneControl* moduleObserver, SDL_Texture* textureAtlas)
{
	screenRect = { 0, 0, WINDOW_W, WINDOW_H };

	initialPos = Position;
	
	btnResume = new GuiButton(1, { Position.x , Position.y + padding*0, 183, 91 }, "RESUME", RECTANGLE, textureAtlas);
	btnResume->SetObserver(moduleObserver);

	btnSettings = new GuiButton(2, { Position.x, Position.y + padding * 1, 183, 91 }, "SETTINGS",RECTANGLE, textureAtlas);
	btnSettings->SetObserver(moduleObserver);

	btnBackToTitle = new GuiButton(3, { Position.x, Position.y + padding * 2, 183, 91 }, "TITLE", RECTANGLE, textureAtlas);
	btnBackToTitle->SetObserver(moduleObserver);

	btnExit = new GuiButton(4, { Position.x, Position.y + padding * 3, 88, 88 }, "", EXIT, textureAtlas);
	btnExit->SetObserver(moduleObserver);

	menuSettings = new GuiSettings({ WINDOW_W / 2 + 240, Position.y - padding }, moduleObserver);

	observer = moduleObserver;

	active = false;
	activeMenu = false;	
}

GuiMenuPause::~GuiMenuPause()
{
}

bool GuiMenuPause::Update(float dt)
{
	bool ret = true;

	if (active)
	{
		btnResume->Update(dt);
		btnSettings->Update(dt);
		btnBackToTitle->Update(dt);
		ret = btnExit->Update(dt);
		if (activeMenu) 
		{
			menuSettings->Update(dt);
			if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			{
				CloaseMenuSettings();
				btnResume->PressButtonSound();
			}
		}
		else
		{
			if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			{
				btnResume->PressButtonSound();
				app->sceneManager->SetPause(false);
				active = false;
				activeMenu = false;
			}
		}
	}

	if (app->sceneManager->GetIsPause())
	{
		if (active) return ret;
		btnResume->PressButtonSound();
		MovePosition();
		active = true;
	}
	return ret;
}

bool GuiMenuPause::Draw()
{
	if (active)
	{
		screenRect.x = -app->render->camera.x;
		screenRect.y = -app->render->camera.y;
		app->render->DrawRectangle(screenRect, 0, 0, 0, 200);

		btnResume->Draw();
		btnSettings->Draw();
		btnBackToTitle->Draw();
		btnExit->Draw();
		if (activeMenu) menuSettings->Draw();
	}
	return true;
}

bool GuiMenuPause::CleanUp()
{
	active = false;

	delete btnResume;
	delete btnSettings;
	delete btnBackToTitle;
	delete btnExit;
	delete menuSettings;

	activeMenu = false;
	return true;
}

bool GuiMenuPause::Event(GuiControl* control)
{

	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		if (control->id == 1) 
		{
			app->sceneManager->SetPause(false);
			active = false;
			activeMenu = false;
		}
		else if (control->id == 2)
		{
			activeMenu = true;
			btnResume->state = GuiControlState::DISABLED;
			btnSettings->state = GuiControlState::DISABLED;
			btnBackToTitle->state = GuiControlState::DISABLED;
			btnExit->state = GuiControlState::DISABLED;
			menuSettings->MovePosition();
			menuSettings->sldMusic->SetValue(app->audio->GetVolumeMusic());
			menuSettings->sldFx->SetValue(app->audio->GetVolumeFx());
			menuSettings->AbleDisableSetting();
		}
		else if (control->id == 3)
		{
			observer->TransitionToScene(SceneType::INTRO);
		}
		else if (control->id == 4)
		{
			return false;
		}
		else if (control->id == 10)
		{
			CloaseMenuSettings();
		}

	}
	case GuiControlType::SLIDER:
	{
		if (control->id == 11)
		{
			int menuMusic;
			menuMusic = menuSettings->sldMusic->GetValue();
			LOG("%d", menuMusic);
			app->audio->SetVolumeMusic(menuMusic);

		}
		if (control->id == 12)
		{
			int menuFx;
			menuFx = menuSettings->sldFx->GetValue();
			LOG("%d", menuFx);
			app->audio->SetVolumeFx(menuFx);
		}
	}
	case GuiControlType::CHECKBOX:
	{
		if (control->id == 13)
		{
			app->fullScreen = menuSettings->chBxFullScreen->GetValue();
			app->win->FullScreen(app->fullScreen);
			app->render->FullScreen();
		}
		if (control->id == 14)
		{
			bool menuVSync;
			menuVSync = menuSettings->chBxVSync->GetValue();
			LOG("%d", menuVSync);
		}
	}
	default: break;
	}
	return true;
}

void GuiMenuPause::CloaseMenuSettings()
{
	activeMenu = false;
	btnResume->state = GuiControlState::NORMAL;
	btnSettings->state = GuiControlState::NORMAL;
	btnBackToTitle->state = GuiControlState::NORMAL;
	btnExit->state = GuiControlState::NORMAL;
	menuSettings->AbleDisableSetting();
	app->SaveConfigRequested();
}

void GuiMenuPause::MovePosition()
{
	int x = -app->render->camera.x + initialPos.x;
	int y = -app->render->camera.y + initialPos.y;

	btnResume->bounds.x = x;
	btnResume->bounds.y = y;

	btnSettings->bounds.x = x;
	btnSettings->bounds.y = y + padding * 1;

	btnBackToTitle->bounds.x = x;
	btnBackToTitle->bounds.y = y + padding * 2;
		
	btnExit->bounds.x = x;
	btnExit->bounds.y = y + padding * 3;
		
}
