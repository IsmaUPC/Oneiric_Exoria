#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneWin.h"
#include "SceneManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

SceneWin::SceneWin()
{
	name.Create("sceneWin");
	active = true;
}

SceneWin::~SceneWin()
{
}

bool SceneWin::Awake()
{
	LOG("Loading SceneWin");
	bool ret = true;
	return ret;
}

bool SceneWin::Start()
{
	app->SetLastScene((Module*)this);
	transition = false;

	app->audio->PlayMusic("Assets/Audio/Music/music_victory.ogg");
	img = app->tex->Load("Assets/Textures/scene_win.png");
	animationSon.texture = app->tex->Load("Assets/Textures/dino_sprites.png");
	animationSon.position = { 340 , 372 };
	idleAnimSon.loop = true;
	idleAnimSon.speed = 0.01;

	for (int i = 0; i < 4; i++)
		idleAnimSon.PushBack({ 336 * i,0, 336, 336 });

	animationSon.currentAnimation = &idleAnimSon;

	animationFather.texture = app->tex->Load("Assets/Textures/dino_orange_big.png");
	animationFather.position = { 540 , 180 };
	idleAnimFather.loop = true;
	idleAnimFather.speed = 0.01;

	for (int i = 0; i < 4; i++)
		idleAnimFather.PushBack({ 558 * i,0, 558, 558 });

	animationFather.currentAnimation = &idleAnimFather;

	SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;

	timer.Start();
	return true;
}

bool SceneWin::PreUpdate()
{
	return true;
}

bool SceneWin::Update(float dt)
{
	animationSon.currentAnimation->Update();
	animationFather.currentAnimation->Update();
	
		idleAnimSon.speed = (dt * 100) * 0.08f;
		idleAnimFather.speed = (dt * 100) * 0.08f;
	
	return true;
}

bool SceneWin::PostUpdate()
{
	bool ret = true;
	SDL_Rect rectSon;
	rectSon = animationSon.currentAnimation->GetCurrentFrame();
	SDL_Rect rectFather;
	rectFather = animationFather.currentAnimation->GetCurrentFrame();

	if (!transition && timer.ReadSec() >= CCOOLDOWNSCENE)
	{
		transition = true;
		if (app->sceneManager->lastLevel == 1) TransitionToScene(SceneType::LEVEL2);
		if (app->sceneManager->lastLevel == 2) TransitionToScene(SceneType::INTRO);
		return true;
	}

	app->render->DrawTexture(img, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(animationSon.texture, animationSon.position.x, animationSon.position.y, &rectSon);
	app->render->DrawTextureFlip(animationFather.texture, animationFather.position.x, animationFather.position.y, &rectFather);

	return ret;
}

bool SceneWin::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(img);
	app->tex->UnLoad(animationSon.texture);
	app->tex->UnLoad(animationFather.texture);
	img = nullptr;
	active = false;
	return true;
}
