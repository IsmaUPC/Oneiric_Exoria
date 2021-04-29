#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneIntro.h"
#include "SceneManager.h"
#include "GuiManager.h"	

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

#define LOGO_FADE_SPEED 7

SceneIntro::SceneIntro()
{
	active = true;
	name.Create("sceneIntro");

}

SceneIntro::~SceneIntro()
{
}

bool SceneIntro::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool SceneIntro::Start()
{

	// GUI: Initialize required controls for the screen
	int margin= 7;
	int padding = 110;
	int yPosition = 330 + margin;

	btnPlay = new GuiButton(1, { 935 + 237/2, 470,  0, 0 }, "Play", RECTANGLE);
	btnPlay->active = false;
	btnPlay->SetObserver(this);
	app->guiManager->AddGuiButton(btnPlay);

	btnContinue = new GuiButton(2, { 935 + 237/2, 510,  85, 25 }, "Continue", RECTANGLE);
	btnContinue->active = false;
	btnContinue->SetObserver(this);
	app->guiManager->AddGuiButton(btnContinue);

	/*btnRemove = new GuiButton(3, { WINDOW_W / 2 + 280 ,yPosition + (padding * 2), 88, 88 }, "", REMOVE, btnTextureAtlas);
	btnRemove->SetObserver(this);
	app->guiManager->AddGuiButton(btnRemove);*/

	/*btnCredits = new GuiButton(4, { 20 , (margin * 4),  88, 88 }, "", CREDITS, btnTextureAtlas);
	btnCredits->SetObserver(this);
	app->guiManager->AddGuiButton(btnCredits);*/

	btnSettings = new GuiButton(3, { 935 + 237/2, 550,  75, 25 }, "Settings", RECTANGLE);
	btnSettings->active = false;
	btnSettings->SetObserver(this);
	app->guiManager->AddGuiButton(btnSettings);

	btnExit = new GuiButton(4, { 935 + 237/2, 590,  35, 25 }, "Exit", EXIT);
	btnExit->active = false;
	btnExit->SetObserver(this);
	app->guiManager->AddGuiButton(btnExit);
	
	menuSettings = new GuiSettings({ 900, 70 }, this);

	app->SetLastScene((Module*)this);
	transition = false;

	menuSettings->MovePosition();

	app->audio->PlayMusic("Assets/Audio/Music/music_intro.ogg");

	bgIntro = app->tex->Load("Assets/Textures/title_background.png");
	logo = app->tex->Load("Assets/Textures/logo_title.png");
	cloud = app->tex->Load("Assets/Textures/GUI/cloud.png");
	oneiric = app->tex->Load("Assets/Textures/oneiric_title.png");
	exoria = app->tex->Load("Assets/Textures/exoria_title.png");

	startFx = app->audio->LoadFx("Assets/Audio/Fx/start_button.wav");
	exitFx = app->audio->LoadFx("Assets/Audio/Fx/exit.wav");
	tittleFx = app->audio->LoadFx("Assets/Audio/Fx/tittle.wav");
	flashFx = app->audio->LoadFx("Assets/Audio/Fx/sparkle.wav");

	app->audio->PlayFx(tittleFx);

	sBackCloudPos = { WINDOW_W / 2 - 420, WINDOW_H / 3 + 300 };
	bBackCloudPos = { WINDOW_W / 2 - 350, WINDOW_H / 3 - 100 };

	bCloudPos = { WINDOW_W /2 + 150, WINDOW_H / 3 };
	bCloudPos2 = { WINDOW_W / 2 + 850, WINDOW_H / 3 - 100 };
	sCloudPos = {WINDOW_W /2 + 500, WINDOW_H/3 - 50};
	sCloudPos2 = { WINDOW_W/2, WINDOW_H / 3 + 250 };

	app->render->camera.x = app->render->camera.y = 0;
	
	ComprobeState(2);

	if (lastLevel == 0)
	{
		btnContinue->state = GuiControlState::DISABLED;
		//btnRemove->state = GuiControlState::DISABLED;
	}
	app->sceneManager->SetPause(false);

	// Easings inicialize variables
	currentIteration = 0;
	totalIterations = 100;
	initialPosition = 1280;
	deltaPosition = 1025;
	
	return true;
}

bool SceneIntro::PreUpdate()
{
	return true;
}

bool SceneIntro::Update(float dt)
{
	bool ret = true;
	if ((app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b) && (menuSettings->GetActiveSettings()))
	{
		menuSettings->btnBack->PressButtonSound();
		CloaseMenuSettings();
	}

	if (menuSettings->active)
	{
		menuSettings->Update(dt);
	}

	CloudsUpdate();

	//Update Easings
	//positionOneiric = 110;
	//positionExoria = 275;
	positionExoria = EaseCircIn(currentIteration, initialPosition, -deltaPosition, totalIterations);
	positionOneiric = EaseCircIn(currentIteration, -895, deltaPosition, totalIterations);

	if (currentIteration < totalIterations)
	{
		++currentIteration;
	}
	else
	{
		if (logoAlpha == 0) app->audio->PlayFx(flashFx);
		if (state == 1) flash = true;
		if (state == 1)
		{
			logoAlpha += (LOGO_FADE_SPEED);

			if (logoAlpha > 255.0f)
			{
				logoAlpha = 255.0f;
				state = 2;
			}
		}
		else if (state == 2)
		{
			timeCounter += dt;
			if (timeCounter >= 0.5f)
			{
				AbleButtons();
				state = 3;
			}
		}
		else if (state == 3)
		{
			if (logoAlpha != 0)logoAlpha -= (LOGO_FADE_SPEED);

			if (logoAlpha <= 0.0f)
			{
				flash = false;
			}
		}
	}
	if (state == 3) angle += dt*10;

	return ret;
}

void SceneIntro::CloudsUpdate()
{
	bCloudPos.x -= 0.5f;
	bCloudPos2.x -= 0.5f;
	sCloudPos.x -= 0.75f;
	sCloudPos2.x -= 0.75f;
	sBackCloudPos.x -= 0.75f;
	bBackCloudPos.x -= 0.5f;
	if (bCloudPos.x + 585 < 0)
	{
		bCloudPos.x = WINDOW_W;
	}
	if (bCloudPos2.x + 585 < 0)
	{
		bCloudPos2.x = WINDOW_W;
	}
	if (sCloudPos.x + 292 < 0)
	{
		sCloudPos.x = WINDOW_W;
	}
	if (sCloudPos2.x + 292 < 0)
	{
		sCloudPos2.x = WINDOW_W;
	}
	if (sBackCloudPos.x + 292 < 0)
	{
		sBackCloudPos.x = WINDOW_W;
	}
	if (bBackCloudPos.x + 585 < 0)
	{
		bBackCloudPos.x = WINDOW_W;
	}
}

bool SceneIntro::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(bgIntro, app->render->camera.x, app->render->camera.y);

	app->render->DrawTexture(cloud, sBackCloudPos.x, sBackCloudPos.y);
	app->render->DrawTexture(cloud, bBackCloudPos.x, bBackCloudPos.y,0,2);

	app->render->DrawTexture(logo, 160, 33,0,1,1,angle);
	app->render->DrawTexture(oneiric, positionOneiric, 145,0,1.3);
	app->render->DrawTexture(exoria, positionExoria, 325,0,1.3);

	CloudsDraw();

	if (state == 3)
	{
		app->render->DrawTextBox(935, 427, 237, 237, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);

		if (menuSettings->active)
		{
			app->render->DrawRectangle({ 935, 427, 237, 237 }, 0, 0, 0, 100);
			menuSettings->Draw();
		}
	}
	
	if (flash)
	{
		app->render->DrawRectangle({ 0, 0, WINDOW_W, WINDOW_H }, 255, 255, 255, logoAlpha);
	}

	return ret;
}

void SceneIntro::CloudsDraw()
{
	app->render->DrawTexture(cloud, bCloudPos.x, bCloudPos.y,0,2);
	app->render->DrawTexture(cloud, bCloudPos2.x, bCloudPos2.y, 0, 2);
	app->render->DrawTexture(cloud, sCloudPos.x, sCloudPos.y);
	app->render->DrawTexture(cloud, sCloudPos2.x, sCloudPos2.y);
}

bool SceneIntro::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(bgIntro);
	app->tex->UnLoad(logo);
	app->tex->UnLoad(cloud);
	app->tex->UnLoad(oneiric);
	app->tex->UnLoad(exoria);

	app->audio->Unload1Fx(startFx);
	app->audio->Unload1Fx(exitFx);
	app->audio->Unload1Fx(tittleFx);
	app->audio->Unload1Fx(flashFx);

	app->guiManager->DeleteList();

	delete menuSettings;
	menuSettings = nullptr;

	bgIntro = nullptr;
	logo = nullptr;
	cloud = nullptr;
	oneiric = nullptr;
	exoria = nullptr;
	active = false;

	return true;
}

bool SceneIntro::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		
		if (control->id == 1)
		{
			app->audio->PlayFx(startFx);
			app->removeGame = false;
			TransitionToScene(SceneType::LEVEL1);
			app->sceneManager->lastLevel = 1;
			isContinue = false;
		}
		else if (control->id == 2 && lastLevel != 0)
		{
			if (lastLevel == 1)TransitionToScene(SceneType::LEVEL1), app->sceneManager->lastLevel = 1;
			if (lastLevel == 2)TransitionToScene(SceneType::LEVEL2), app->sceneManager->lastLevel = 2;
			if (lastLevel == 3)TransitionToScene(SceneType::LEVEL3), app->sceneManager->lastLevel = 3;
			isContinue = true;
			app->player->loadStats = true;
		}
		else if (control->id == 3)
		{
			/*app->removeGame = true;
			app->SaveGameRequest();
			lastLevel = 0;
			btnContinue->state = GuiControlState::DISABLED;
			btnRemove->state = GuiControlState::DISABLED;*/
			btnPlay->state = GuiControlState::DISABLED;
			btnContinue->state = GuiControlState::DISABLED;
			//btnRemove->state = GuiControlState::DISABLED;
			btnSettings->state = GuiControlState::DISABLED;
			//btnCredits->state = GuiControlState::DISABLED;
			btnExit->state = GuiControlState::DISABLED;

			menuSettings->MovePosition();
			menuSettings->sldMusic->SetValue(app->audio->GetVolumeMusic());
			menuSettings->sldFx->SetValue(app->audio->GetVolumeFx());
			menuSettings->AbleDisableSetting();
		}
		else if (control->id == 4)
		{
			//TransitionToScene(SceneType::LOGO);
			app->audio->PlayFx(exitFx);
			return false;
		}
		else if (control->id == 5)
		{
			//return false;
		}
		else if (control->id == 6)
		{
		/*	btnPlay->state = GuiControlState::DISABLED;
			btnContinue->state = GuiControlState::DISABLED;
			btnRemove->state = GuiControlState::DISABLED;
			btnSettings->state = GuiControlState::DISABLED;
			btnCredits->state = GuiControlState::DISABLED;
			btnExit->state = GuiControlState::DISABLED;

			menuSettings->MovePosition();
			menuSettings->sldMusic->SetValue(app->audio->GetVolumeMusic());
			menuSettings->sldFx->SetValue(app->audio->GetVolumeFx());
			menuSettings->AbleDisableSetting();*/
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
void SceneIntro::AbleButtons()
{
	btnPlay->active = true;
	btnContinue->active = true;
	btnSettings->active = true;
	btnExit->active = true;
}

void SceneIntro::CloaseMenuSettings()
{
	btnPlay->state = GuiControlState::NORMAL;
	btnSettings->state = GuiControlState::NORMAL;
	//btnCredits->state = GuiControlState::NORMAL;
	btnExit->state = GuiControlState::NORMAL;
	if (lastLevel != 0)
	{
		btnContinue->state = GuiControlState::NORMAL;
		//btnRemove->state = GuiControlState::NORMAL;
	}
	menuSettings->btnBack->state = GuiControlState::NORMAL;
	menuSettings->AbleDisableSetting();
	app->SaveConfigRequested();
}

bool SceneIntro::LoadState(pugi::xml_node& data)
{
	lastLevel = data.child("level").attribute("lvl").as_int(0);
	if (app->removeGame)lastLevel = 0;
	return true;
}

bool SceneIntro::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(lastLevel);

	return true;
}

void SceneIntro::ComprobeState(int id)
{
	bool ret = true;
	pugi::xml_parse_result result = sceneFile.load_file("save_game.xml");

	// Check result for loading errors
	if (result == NULL)
	{
		LOG("Could not load map xml file savegame.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		sceneStateFile = sceneFile.first_child();
		sceneStateFile = sceneStateFile.child("scene_manager");
		if(id==2)LoadState(sceneStateFile);
	}
	sceneFile.reset();
}