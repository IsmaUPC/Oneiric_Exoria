#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneLose.h"
#include "SceneManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

SceneLose::SceneLose()
{
	active = true;
	name.Create("sceneLose");
}

SceneLose::~SceneLose()
{
}

bool SceneLose::Awake()
{
	LOG("Loading SceneLose");
	bool ret = true;
	return ret;
}

bool SceneLose::Start()
{
	app->SetLastScene((Module*)this);
	transition = false;

	app->audio->PlayMusic("Assets/Audio/Music/music_lost.ogg");
	img = app->tex->Load("Assets/Textures/end_screen.png");
	animationEnd.texture = app->tex->Load("Assets/Textures/dino_sprites_dead.png");
	animationEnd.position = { 480 , 345 };
	idleAnim.loop = true;
	idleAnim.speed = 0.008f;

	for (int i = 0; i < 9; i++)
		idleAnim.PushBack({ 336 * i,0, 336, 336 });

	animationEnd.currentAnimation = &idleAnim;

	SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;

	timer.Start();
	return true;
}

bool SceneLose::PreUpdate()
{
	return true;
}

bool SceneLose::Update(float dt)
{
	animationEnd.currentAnimation->Update();
	idleAnim.speed = (dt*100 )*0.08f;

	return true;
}

bool SceneLose::PostUpdate()
{
	bool ret = true;
	SDL_Rect rectIntro;
	rectIntro = animationEnd.currentAnimation->GetCurrentFrame();

	if (!transition && timer.ReadSec() > CCOOLDOWNSCENE)
	{
		transition = true;
		if (app->sceneManager->lastLevel == 1) TransitionToScene(SceneType::LEVEL1);
		if (app->sceneManager->lastLevel == 2) TransitionToScene(SceneType::LEVEL2);
		return true;
	}

	app->render->DrawTexture(img, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(animationEnd.texture, animationEnd.position.x, animationEnd.position.y, &rectIntro);

	return ret;
}

bool SceneLose::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");

	Mix_HaltMusic();
	app->tex->UnLoad(img);
	app->tex->UnLoad(animationEnd.texture);
	img = nullptr;
	animationEnd.texture = nullptr;
	active = false;
	return true;
}
