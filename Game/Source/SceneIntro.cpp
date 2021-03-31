#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneIntro.h"
#include "SceneManager.h"

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
	SDL_Texture* btnTextureAtlas = app->sceneManager->btnTextureAtlas;
	btnSettingsTex = app->tex->Load("Assets/Textures/GUI/setting_button.png");
	btnExitTex = app->tex->Load("Assets/Textures/GUI/exit_button.png");

	// GUI: Initialize required controls for the screen
	int margin= 7;
	int padding = 98;
	int yPosition = 330 + (margin * 1);

	btnPlay = new GuiButton(1, { WINDOW_W / 2 - 200 / 2,yPosition + (padding * 0),  183, 91 }, "PLAY", RECTANGLE, btnTextureAtlas);
	btnPlay->SetObserver(this);

	btnContinue = new GuiButton(2, { WINDOW_W / 2 - 200 / 2, yPosition + (padding * 1),  183, 91 }, "CONTINUE", RECTANGLE, btnTextureAtlas);
	btnContinue->SetObserver(this);

	btnRemove = new GuiButton(3, { WINDOW_W / 2 + 90 ,yPosition + (padding * 1), 88, 88 }, "", REMOVE, btnTextureAtlas);
	btnRemove->SetObserver(this);

	btnSettings = new GuiButton(4, { WINDOW_W / 2 - 200 / 2, yPosition + (padding * 2), 183, 91 }, "SETTINGS", RECTANGLE, btnTextureAtlas);
	btnSettings->SetObserver(this);

	btnCredits = new GuiButton(5, { WINDOW_W-( WINDOW_W /9) , (margin * 4),  88, 88 }, "", CREDITS, btnTextureAtlas);
	btnCredits->SetObserver(this);

	btnExit = new GuiButton(6, { WINDOW_W / 2 - 200 ,yPosition + (padding * 2),  88, 88 }, "", EXIT, btnTextureAtlas);
	btnExit->SetObserver(this);
	
	menuSettings = new GuiSettings({ WINDOW_W / 2 + 240, yPosition - (padding * 2) }, this);

	app->SetLastScene((Module*)this);
	transition = false;

	menuSettings->MovePosition();

	app->audio->PlayMusic("Assets/Audio/Music/music_intro.ogg");
	bgIntro = app->tex->Load("Assets/Textures/title_screen.png");
	animationIntro.texture = app->tex->Load("Assets/Textures/dino_sprites.png");
	animationIntro.position = { 180 , 363 };
	idleAnim->loop = true;
	idleAnim->speed = 0.005f;

	for (int i = 0; i < 4; i++)
		idleAnim->PushBack({ 336 * i,0, 336, 336 });

	animationIntro.currentAnimation = idleAnim;

	SDL_QueryTexture(bgIntro, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	
	ComprobeState(2);

	if (lastLevel == 0)
	{
		btnContinue->state = GuiControlState::DISABLED;
		btnRemove->state = GuiControlState::DISABLED;
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
	animationIntro.currentAnimation->Update();
	
	idleAnim->speed = (dt * 100) * 0.05f;

	if ((app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) && (menuSettings->GetActiveSettings()))
	{
		menuSettings->btnBack->PressButtonSound();
		CloaseMenuSettings();
	}


	btnPlay->Update(dt);
	btnContinue->Update(dt);
	btnRemove->Update(dt);
	btnSettings->Update(dt);
	btnCredits->Update(dt);
	ret=btnExit->Update(dt);
	menuSettings->Update(dt);

	return ret;
}

bool SceneIntro::PostUpdate()
{
	bool ret = true;
	SDL_Rect rectIntro;
	rectIntro = animationIntro.currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(bgIntro, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(animationIntro.texture, animationIntro.position.x, animationIntro.position.y, &rectIntro);
	
	btnPlay->Draw();
	btnContinue->Draw();
	btnRemove->Draw();
	btnSettings->Draw();
	btnCredits->Draw();
	btnExit->Draw();

	menuSettings->Draw();

	return ret;
}

bool SceneIntro::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(btnSettingsTex);
	app->tex->UnLoad(btnExitTex);
	app->tex->UnLoad(bgIntro);
	app->tex->UnLoad(animationIntro.texture);

	menuSettings->CleanUp();

	delete btnPlay;
	delete btnContinue;
	delete btnRemove;
	delete btnSettings;
	delete btnCredits;
	delete btnExit;
	
	btnPlay = NULL;
	btnContinue = NULL;
	btnRemove = NULL;
	btnSettings = NULL;
	btnCredits = NULL;
	btnExit = NULL;

	bgIntro = nullptr;
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
			app->removeGame = true;
			app->SaveGameRequest();
			lastLevel = 0;
			btnContinue->state = GuiControlState::DISABLED;
			btnRemove->state = GuiControlState::DISABLED;
		}
		else if (control->id == 4)
		{
			btnPlay->state = GuiControlState::DISABLED;
			btnContinue->state = GuiControlState::DISABLED;
			btnSettings->state = GuiControlState::DISABLED;
			btnCredits->state = GuiControlState::DISABLED;
			btnExit->state = GuiControlState::DISABLED;
			btnRemove->state = GuiControlState::DISABLED;

			menuSettings->MovePosition();
			menuSettings->sldMusic->SetValue(app->audio->GetVolumeMusic());
			menuSettings->sldFx->SetValue(app->audio->GetVolumeFx());
			menuSettings->AbleDisableSetting();

		}
		else if (control->id == 5)
		{
			TransitionToScene(SceneType::LOGO);
		}
		else if (control->id == 6)
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

void SceneIntro::CloaseMenuSettings()
{
	btnPlay->state = GuiControlState::NORMAL;
	btnSettings->state = GuiControlState::NORMAL;
	btnCredits->state = GuiControlState::NORMAL;
	btnExit->state = GuiControlState::NORMAL;
	if (lastLevel != 0)
	{
		btnContinue->state = GuiControlState::NORMAL;
		btnRemove->state = GuiControlState::NORMAL;
	}
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


