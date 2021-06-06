#include "App.h"
#include "GuiMenuPause.h"
#include "Audio.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiManager.h"
#include "DialogSystem.h"
#include "Fonts.h"

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

	menuSettings = new GuiSettings({ WINDOW_W / 2 + 140, Position.y - 82}, moduleObserver);

	observer = moduleObserver;

	active = false;
	activeSettings = false;	
	activePause = false;	
}

GuiMenuPause::~GuiMenuPause()
{
}

bool GuiMenuPause::Update(float dt)
{
	bool ret = true;

	if (active)
	{
		if (doorRand == true)
		{
			AssignRandomAnimation();
			currentIteration = 0;
			offsetSpawnY = 0;
			offsetSpawnX = 0;
		}		

		if (currentIteration >= totalIterations && !activePause) active = !active, AbleDisableMenu(), activePause = true;
		if (currentIteration < totalIterations)
		{
			UpdateSpawnPosition();
			currentIteration++;
		}

		if (activeSettings)
		{
			menuSettings->Update(dt);
			if ((app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b) && !app->guiManager->press && !app->guiManager->missClick)
			{
				app->guiManager->press = true;
				app->guiManager->missClick = true;
				menuSettings->DesactiveSettingMenu();
			}
		}
		else if ((app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b) && !app->guiManager->press && !app->guiManager->missClick && currentIteration >= totalIterations)
		{
			app->guiManager->press = true;
			app->guiManager->missClick = true;
			btnResume->PressButtonSound();
			DisableButtons();
			currentIteration = 0;
			spawnPos = 0;

			if (!app->audio->GetHasBeenModificated())
				app->audio->SetVolumeMusic(app->sceneManager->GetCurrentVolume());
			else app->audio->SetHasBeenModificated(false);
		}
		if (currentIteration >= totalIterations && spawnPos == 0)
		{
			app->sceneManager->SetPause(false);
			active = !active;
			currentIteration = 0;
			activePause = false;
			doorRand = true;
		}
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
		app->render->DrawTextBox(-app->render->camera.x + initialPos.x + offsetSpawnX, -app->render->camera.y + initialPos.y + offsetSpawnY, 237, 237, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
		
		SDL_Rect buttonRect = { 0,0,16,16 };
		app->render->DrawTexture(app->guiManager->uiButtonHelp, -app->render->camera.x + 32, -app->render->camera.y + WINDOW_H - 64, &buttonRect, 2);
		app->fonts->BlitText(-app->render->camera.x + 70, -app->render->camera.y + WINDOW_H - 62, 0, "Accept", { 255, 255, 255 });
		buttonRect = { 0,48,16,16 };
		app->render->DrawTexture(app->guiManager->uiButtonHelp, -app->render->camera.x + 140, -app->render->camera.y + WINDOW_H - 64, &buttonRect, 2);
		app->fonts->BlitText(-app->render->camera.x + 178, -app->render->camera.y + WINDOW_H - 62, 0, "Back", { 255, 255, 255 });
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
	activePause = false;

	RELEASE(btnResume);
	RELEASE(btnSettings);
	RELEASE(btnSave);
	RELEASE(btnExit);
	RELEASE(btnBackToTitle);
	menuSettings->CleanUp();
	RELEASE(menuSettings);

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
			btnResume->PressButtonSound();
			DisableButtons();
			currentIteration = 0;
			spawnPos = 0;

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
			menuSettings->DesactiveSettingMenu();
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
			if (menuVSync)
			{
				SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
				LOG("Using vsync");
			}
			else
			{
				SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
				LOG("Not using vsync");
			}
		}
		if (control->id == 15)
		{
			app->dialogueSystem->dialogSpeed = 1;
			menuSettings->chBxTextSpeed0->SetValue(0);
			menuSettings->chBxTextSpeed2->SetValue(0);
		}
		if (control->id == 16)
		{
			app->dialogueSystem->dialogSpeed = 2;
			menuSettings->chBxTextSpeed1->SetValue(0);
			menuSettings->chBxTextSpeed2->SetValue(0);
		}
		if (control->id == 17)
		{
			app->dialogueSystem->dialogSpeed = 0;
			menuSettings->chBxTextSpeed0->SetValue(0);
			menuSettings->chBxTextSpeed1->SetValue(0);
		}
	}
	default: break;
	}
	return true;
}

void GuiMenuPause::AssignRandomAnimation()
{
	randT = rand() % 4;
	switch (randT)
	{
	case 0:
		spawnPos = -WINDOW_H / 2;
		break;
	case 1:
		spawnPos = WINDOW_H / 2;
		break;
	case 2:
		spawnPos = -WINDOW_W / 2;
		break;
	case 3:
		spawnPos = WINDOW_W / 2;
		break;
	default:
		break;
	}
	doorRand = false;
}

void GuiMenuPause::UpdateSpawnPosition()
{
	if (spawnPos != 0)
	{
		switch (randT)
		{
		case 0:
			offsetSpawnY = EaseBackOut(currentIteration, spawnPos, WINDOW_H / 2, totalIterations);
			break;
		case 1:
			offsetSpawnY = EaseBackOut(currentIteration, spawnPos, -WINDOW_H / 2, totalIterations);
			break;
		case 2:
			offsetSpawnX = EaseBackOut(currentIteration, spawnPos, WINDOW_W / 2, totalIterations);
			break;
		case 3:
			offsetSpawnX = EaseBackOut(currentIteration, spawnPos, -WINDOW_W / 2, totalIterations);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (randT)
		{
		case 0:
			offsetSpawnY = EaseCubicIn(currentIteration, spawnPos, WINDOW_H / 2 + 120, totalIterations - 20);
			break;
		case 1:
			offsetSpawnY = EaseCubicIn(currentIteration, spawnPos, -WINDOW_H / 2 - 120, totalIterations - 20);
			break;
		case 2:
			offsetSpawnX = EaseCubicIn(currentIteration, spawnPos, WINDOW_W / 2 + 120, totalIterations - 20);
			break;
		case 3:
			offsetSpawnX = EaseCubicIn(currentIteration, spawnPos, -WINDOW_W / 2 - 120, totalIterations - 20);
			break;
		default:
			break;
		}
	}
	
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
void GuiMenuPause::DisableButtons()
{
	btnResume->active = false;
	btnSettings->active = false;
	btnBackToTitle->active = false;
	btnSave->active = false;
	btnExit->active = false;
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
