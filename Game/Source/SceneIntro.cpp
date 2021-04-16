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

	btnPlay = new GuiButton(1, { 1053 - 25, 470,  0, 0 }, "Play", RECTANGLE);
	btnPlay->SetObserver(this);
	app->guiManager->AddGuiButton(btnPlay);

	btnContinue = new GuiButton(2, { 1053 - 85 / 2 , 510,  85, 25 }, "Continue", RECTANGLE);
	btnContinue->SetObserver(this);
	app->guiManager->AddGuiButton(btnContinue);

	/*btnRemove = new GuiButton(3, { WINDOW_W / 2 + 280 ,yPosition + (padding * 2), 88, 88 }, "", REMOVE, btnTextureAtlas);
	btnRemove->SetObserver(this);
	app->guiManager->AddGuiButton(btnRemove);*/

	/*btnCredits = new GuiButton(4, { 20 , (margin * 4),  88, 88 }, "", CREDITS, btnTextureAtlas);
	btnCredits->SetObserver(this);
	app->guiManager->AddGuiButton(btnCredits);*/

	btnSettings = new GuiButton(3, { 1053 - 75/2, 550,  75, 25 }, "Settings", RECTANGLE);
	btnSettings->SetObserver(this);
	app->guiManager->AddGuiButton(btnSettings);

	btnExit = new GuiButton(4, { 1053 - 35/2 , 590,  35, 25 }, "Exit", EXIT);
	btnExit->SetObserver(this);
	app->guiManager->AddGuiButton(btnExit);
	
	menuSettings = new GuiSettings({ 900, 100 }, this);

	app->SetLastScene((Module*)this);
	transition = false;

	menuSettings->MovePosition();

	app->audio->PlayMusic("Assets/Audio/Music/music_intro.ogg");
	bgIntro = app->tex->Load("Assets/Textures/title_background.png");
	logoIntro = app->tex->Load("Assets/Textures/title_logo.png");

	SDL_QueryTexture(logoIntro, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	
	ComprobeState(2);

	if (lastLevel == 0)
	{
		btnContinue->state = GuiControlState::DISABLED;
		//btnRemove->state = GuiControlState::DISABLED;
	}
	app->sceneManager->SetPause(false);
	
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

	return ret;
}

bool SceneIntro::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(bgIntro, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(logoIntro, 108, 33);

	app->render->DrawTextBox(935, 427, 237, 237, { 24, 61, 172 }, { 97, 159, 207 }, { 0, 33, 78 }, app->guiManager->moonCorner, 200);

	if (menuSettings->active)
	{
		menuSettings->Draw();
	}

	return ret;
}

bool SceneIntro::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(bgIntro);
	app->tex->UnLoad(logoIntro);

	app->guiManager->DeleteList();

	delete menuSettings;
	menuSettings = nullptr;

	bgIntro = nullptr;
	logoIntro = nullptr;
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
			app->removeGame = false;
			TransitionToScene(SceneType::LEVEL1);
			app->sceneManager->lastLevel = 1;
			isContinue = false;
		}
		else if (control->id == 2 && lastLevel != 0)
		{
			if (lastLevel == 1)TransitionToScene(SceneType::LEVEL1), app->sceneManager->lastLevel = 1;
			if (lastLevel == 2)TransitionToScene(SceneType::LEVEL2), app->sceneManager->lastLevel = 2;
			isContinue = true;
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


