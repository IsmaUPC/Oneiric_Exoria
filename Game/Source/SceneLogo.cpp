#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "SceneLogo.h"
#include "Audio.h"

#include "Defs.h"
#include "Log.h"

#define LOGO_FADE_SPEED 500.0f

SceneLogo::SceneLogo()
{
	active = true;
	name.Create("sceneLogo");
}

SceneLogo::~SceneLogo()
{
}

bool SceneLogo::Awake()
{
	LOG("Loading SceneLogo");
	bool ret = true;
	return ret;
}

bool SceneLogo::Start()
{
	app->SetLastScene((Module*)this);

	img = app->tex->Load("Assets/Textures/scene_logo.png");
	introFx = app->audio->LoadFx("Assets/Audio/Music/logo.wav");
	app->audio->PlayFx(introFx);


	SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	logo = {imgX,imgY,imgW,imgH};

	state = 0;
	timeCounter = 0.0f;
	logoAlpha = 0.0f;

	// Background color
	color.r = 20; color.g = 20; color.b = 20;
	
	return true;
}

bool SceneLogo::PreUpdate()
{
	return true;
}

bool SceneLogo::Update(float dt)
{

	if (state == 0)
	{
		state = 1;
	}
	else if (state == 1)
	{
		logoAlpha += (LOGO_FADE_SPEED * dt);

		if (logoAlpha > 255.0f)
		{
			logoAlpha = 255.0f;
			state = 2;
		}
	}
	else if (state == 2)
	{
		// Waiting for 3 seconds
		timeCounter += dt;
		if (timeCounter >= 4.5f) state = 3;
	}
	else if (state == 3)
	{
		if(logoAlpha!=0)logoAlpha -= (LOGO_FADE_SPEED * dt);

		if (logoAlpha <= 0.0f)
		{
			logoAlpha = 0.0f;
			TransitionToScene(SceneType::INTRO);
		}
	}

	return true;
}

bool SceneLogo::PostUpdate()
{
	app->render->SetBackgroundColor(color);

	bool ret = true;
	if (img != NULL)
	{
		SDL_SetTextureAlphaMod(img, logoAlpha);
		app->render->DrawTexture(img, 0, 0);
	}

	return ret;
}

bool SceneLogo::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	app->tex->UnLoad(img);
	//app->audio->Unload1Fx(introFx);
	img = nullptr;
	active = false;
	return true;
}

