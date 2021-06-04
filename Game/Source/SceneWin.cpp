#include "SceneWin.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneManager.h"
#include "GuiManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

#define LOGO_FADE_SPEED 7

SceneWin::SceneWin(SceneType type) : SceneControl(type)
{
	active = true;
	name.Create("sceneWin");

}

SceneWin::~SceneWin()
{
}

bool SceneWin::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool SceneWin::Start()
{

	// GUI: Initialize required controls for the screen
	int margin = 7;
	int padding = 110;
	int yPosition = 330 + margin;

	btnContinue = new GuiButton(2, { 935 + 237 / 2, 510,  85, 25 }, "Continue", RECTANGLE);
	btnContinue->active = false;
	btnContinue->SetObserver(this);
	app->guiManager->AddGuiButton(btnContinue);

	app->SetLastScene((Module*)this);

	app->audio->PlayMusic("Audio/Music/music_intro.ogg");

	bgIntro = app->tex->Load("Textures/title_background.png");
	logo = app->tex->Load("Textures/logo_title.png");
	cloud = app->tex->Load("Textures/GUI/cloud.png");
	oneiric = app->tex->Load("Textures/oneiric_title.png");
	exoria = app->tex->Load("Textures/exoria_title.png");

	fxStart = app->audio->LoadFx("Audio/Fx/start_button.wav");
	fxTittle = app->audio->LoadFx("Audio/Fx/tittle.wav");
	fxFlash = app->audio->LoadFx("Audio/Fx/sparkle.wav");

	app->audio->PlayFx(fxTittle);

	sBackCloudPos = { WINDOW_W / 2 - 420, WINDOW_H / 3 + 300 };
	bBackCloudPos = { WINDOW_W / 2 - 350, WINDOW_H / 3 - 100 };

	bCloudPos = { WINDOW_W / 2 + 150, WINDOW_H / 3 };
	bCloudPos2 = { WINDOW_W / 2 + 850, WINDOW_H / 3 - 100 };
	sCloudPos = { WINDOW_W / 2 + 500, WINDOW_H / 3 - 50 };
	sCloudPos2 = { WINDOW_W / 2, WINDOW_H / 3 + 250 };

	app->render->camera.x = app->render->camera.y = 0;

	// Easings inicialize variables
	currentIteration = 0;
	totalIterations = 100;
	initialPosition = 1280;
	deltaPosition = 1025;

	return true;
}

bool SceneWin::PreUpdate()
{
	return true;
}

bool SceneWin::Update(float dt)
{
	bool ret = true;

	CloudsUpdate();

	//Update Easings	
	if (state == 0)
	{
		positionExoria = EaseCircIn(currentIteration, initialPosition, -deltaPosition, totalIterations);
		positionOneiric = EaseCircIn(currentIteration, -895, deltaPosition, totalIterations);
	}

	if (currentIteration < totalIterations)
	{
		++currentIteration;
	}
	else
	{
		if (logoAlpha == 0) app->audio->PlayFx(fxFlash);
		if (state == 0) state = 1;
		if (state == 1)
		{
			flash = true;
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
	if (state == 3) angle += dt * 10;

	return ret;
}

void SceneWin::AbleButtons()
{
	btnContinue->active = true;
}

void SceneWin::CloudsUpdate()
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

bool SceneWin::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(bgIntro, app->render->camera.x, app->render->camera.y);

	app->render->DrawTexture(cloud, sBackCloudPos.x, sBackCloudPos.y);
	app->render->DrawTexture(cloud, bBackCloudPos.x, bBackCloudPos.y, 0, 2);

	app->render->DrawTexture(logo, 160, 33, 0, 1, 1, angle);
	app->render->DrawTexture(oneiric, positionOneiric, 145, 0, 1.3);
	app->render->DrawTexture(exoria, positionExoria, 325, 0, 1.3);

	CloudsDraw();

	if (state == 3)
	{
		app->render->DrawTextBox(935, 427, 237, 237, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	}

	if (flash)
	{
		app->render->DrawRectangle({ 0, 0, WINDOW_W, WINDOW_H }, 255, 255, 255, logoAlpha);
	}

	return ret;
}

void SceneWin::CloudsDraw()
{
	app->render->DrawTexture(cloud, bCloudPos.x, bCloudPos.y, 0, 2);
	app->render->DrawTexture(cloud, bCloudPos2.x, bCloudPos2.y, 0, 2);
	app->render->DrawTexture(cloud, sCloudPos.x, sCloudPos.y);
	app->render->DrawTexture(cloud, sCloudPos2.x, sCloudPos2.y);
}

bool SceneWin::CleanUp()
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

	app->audio->Unload1Fx(fxStart);
	app->audio->Unload1Fx(fxTittle);
	app->audio->Unload1Fx(fxFlash);

	app->guiManager->DeleteList();

	bgIntro = nullptr;
	logo = nullptr;
	cloud = nullptr;
	oneiric = nullptr;
	exoria = nullptr;
	active = false;

	return true;
}

bool SceneWin::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		if (control->id == 2)
		{
			TransitionToScene(SceneType::LEVEL1), app->sceneManager->lastLevel = 1;
			isContinue = true;
			app->player->loadStats = true;
		}
	}
	break;
	}
	return true;
}

bool SceneWin::LoadState(pugi::xml_node& data)
{
	lastLevel = data.child("level").attribute("lvl").as_int(0);
	return true;
}

bool SceneWin::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(lastLevel);

	return true;
}