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
	
	btnResume = new GuiButton(1, { Position.x, Position.y + padding*0, 183, 91 }, "Resume", RECTANGLE, textureAtlas);
	btnResume->SetObserver(moduleObserver);
	btnResume->active = false;
	app->guiManager->AddGuiButton(btnResume);

	btnSettings = new GuiButton(2, { Position.x, Position.y + padding * 1, 183, 91 }, "Settings",RECTANGLE, textureAtlas);
	btnSettings->SetObserver(moduleObserver);
	btnSettings->active = false;
	app->guiManager->AddGuiButton(btnSettings);

	btnBackToTitle = new GuiButton(3, { Position.x, Position.y + padding * 2, 183, 91 }, "Title", RECTANGLE, textureAtlas);
	btnBackToTitle->SetObserver(moduleObserver);
	btnBackToTitle->active = false;
	app->guiManager->AddGuiButton(btnBackToTitle);

	btnSave = new GuiButton(4, { Position.x, Position.y + padding * 3, 183, 91 }, "Save", RECTANGLE, textureAtlas);
	btnSave->SetObserver(moduleObserver);
	btnSave->active = false;
	app->guiManager->AddGuiButton(btnSave);

	btnExit = new GuiButton(5, { Position.x, Position.y + padding * 4, 88, 88 }, "Exit", EXIT, textureAtlas);
	btnExit->SetObserver(moduleObserver);
	btnExit->active = false;
	app->guiManager->AddGuiButton(btnExit);

	menuSettings = new GuiSettings({ WINDOW_W / 2 + 140, Position.y - 50}, moduleObserver);

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

	if (active)
	{
		screenRect.x = -app->render->camera.x;
		screenRect.y = -app->render->camera.y;
		app->render->DrawRectangle(screenRect, 0, 0, 0, 200);
		app->render->DrawTextBox(-app->render->camera.x + initialPos.x, -app->render->camera.y + initialPos.y, 237, 237, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	}

	if (menuSettings->active)
	{
		app->render->DrawRectangle({ -app->render->camera.x + initialPos.x, -app->render->camera.y + initialPos.y, 237, 237 }, 0, 0, 0, 100);
		menuSettings->Draw();
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
			if (!app->audio->GetHasBeenModificated())
				app->audio->SetVolumeMusic(app->sceneManager->GetCurrentVolume());
			else app->audio->SetHasBeenModificated(false);
		}
		else if (control->id == 2)
		{
			activeSettings = true;
			AbleDisableSetting();

			btnResume->state = GuiControlState::DISABLED;
			btnSettings->state = GuiControlState::DISABLED;
			btnBackToTitle->state = GuiControlState::DISABLED;
			btnSave->state = GuiControlState::DISABLED;
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
			app->SaveGameRequest();
		}
		else if (control->id == 5)
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
			app->audio->SetHasBeenModificated(true);

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
	btnSave->active = active;
	btnExit->active = active;

	btnResume->state = GuiControlState::NORMAL;
	
	btnResume->PressButtonSound();
	if (active == true)
	{
		MovePosition();
	}
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
	btnSave->state = GuiControlState::NORMAL;
	btnExit->state = GuiControlState::NORMAL;
	menuSettings->btnBack->state = GuiControlState::NORMAL;
	AbleDisableSetting();
	app->SaveConfigRequested();
}

void GuiMenuPause::MovePosition()
{
	int x = -app->render->camera.x + initialPos.x;
	int y = -app->render->camera.y + initialPos.y +30;

	btnResume->bounds.x = x + 237/2 - btnResume->bounds.w/2;
	btnResume->bounds.y = y;

	btnSettings->bounds.x = x + 237 / 2 - btnSettings->bounds.w / 2;
	btnSettings->bounds.y = y + padding * 1;

	btnBackToTitle->bounds.x = x + 237 / 2 - btnBackToTitle->bounds.w / 2;
	btnBackToTitle->bounds.y = y + padding * 2;
	
	btnSave->bounds.x = x + 237 / 2 - btnSave->bounds.w / 2;
	btnSave->bounds.y = y + padding * 3;
		
	btnExit->bounds.x = x + 237 / 2 - btnExit->bounds.w / 2;
	btnExit->bounds.y = y + padding * 4;
		
}
