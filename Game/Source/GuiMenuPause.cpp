#include "App.h"
#include "GuiMenuPause.h"
#include "Audio.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

GuiMenuPause::GuiMenuPause(iPoint Position, SceneControl* moduleObserver, SDL_Texture* textureAtlas)
{
	screenRect = { 0, 0, WINDOW_W, WINDOW_H };

	initialPos = Position;
	
	btnResume = new GuiButton(1, { Position.x , Position.y + padding*0, 183, 91 }, "RESUME", RECTANGLE, textureAtlas);
	btnResume->SetObserver(moduleObserver);
	btnResume->active = false;
	app->guiManager->AddGuiButton(btnResume);

	btnSettings = new GuiButton(2, { Position.x, Position.y + padding * 1, 183, 91 }, "SETTINGS",RECTANGLE, textureAtlas);
	btnSettings->SetObserver(moduleObserver);
	btnSettings->active = false;
	app->guiManager->AddGuiButton(btnSettings);

	btnBackToTitle = new GuiButton(3, { Position.x, Position.y + padding * 2, 183, 91 }, "TITLE", RECTANGLE, textureAtlas);
	btnBackToTitle->SetObserver(moduleObserver);
	btnBackToTitle->active = false;
	app->guiManager->AddGuiButton(btnBackToTitle);

	btnExit = new GuiButton(4, { Position.x, Position.y + padding * 3, 88, 88 }, "", EXIT, textureAtlas);
	btnExit->SetObserver(moduleObserver);
	btnExit->active = false;
	app->guiManager->AddGuiButton(btnExit);

	menuSettings = new GuiSettings({ WINDOW_W / 2 + 240, Position.y - padding }, moduleObserver);

	observer = moduleObserver;

	active = false;
	activeSettings = false;	
}

GuiMenuPause::~GuiMenuPause()
{
}

bool GuiMenuPause::Update(float dt)
{
	bool ret = true;

	if (active)
	{
		if (activeSettings) 
		{
			menuSettings->Update(dt);
			if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b)
			{
				CloaseMenuSettings();
				btnResume->PressButtonSound();
			}
		}
		/*
		else if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b)
		{
			btnResume->PressButtonSound();
			app->sceneManager->SetPause(false);
			AbleDisableMenu();
		}*/
	}

	return ret;
}

bool GuiMenuPause::PostUpdate()
{

	if (active )
	{
		screenRect.x = -app->render->camera.x;
		screenRect.y = -app->render->camera.y;
		app->render->DrawRectangle(screenRect, 0, 0, 0, 200);
	}
	return true;
}

bool GuiMenuPause::CleanUp()
{
	active = false;
	activeSettings = false;

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
			AbleDisableMenu();
			activeSettings = false;
		}
		else if (control->id == 2)
		{
			activeSettings = true;
			AbleDisableSetting();

			btnResume->state = GuiControlState::DISABLED;
			btnSettings->state = GuiControlState::DISABLED;
			btnBackToTitle->state = GuiControlState::DISABLED;
			btnExit->state = GuiControlState::DISABLED;

			menuSettings->MovePosition();
			menuSettings->sldMusic->SetValue(app->audio->GetVolumeMusic());
			menuSettings->sldFx->SetValue(app->audio->GetVolumeFx());
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

void GuiMenuPause::AbleDisableMenu()
{
	active = !active;
	btnResume->active = active;
	btnSettings->active = active;
	btnBackToTitle->active = active;
	btnExit->active = active;
	
	btnResume->PressButtonSound();
	if (active == true)MovePosition();
}

void GuiMenuPause::AbleDisableSetting()
{
	menuSettings->AbleDisableSetting();
}

void GuiMenuPause::CloaseMenuSettings()
{
	activeSettings = false;
	btnResume->state = GuiControlState::NORMAL;
	btnSettings->state = GuiControlState::NORMAL;
	btnBackToTitle->state = GuiControlState::NORMAL;
	btnExit->state = GuiControlState::NORMAL;
	AbleDisableSetting();
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
