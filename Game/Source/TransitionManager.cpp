#include "TransitionManager.h"
#include "SceneManager.h"

#include "Render.h"
#include "Textures.h"
#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

TransitionManager::TransitionManager(Render* render, Textures* tex, SceneManager* sceneManager) : Module()
{
	name.Create("transition_manager");


	this->render = render;
	this->tex = tex;
}

// Destructor
TransitionManager::~TransitionManager(){}

// Called before render is available
bool TransitionManager::Awake()
{
	LOG("Loading Transition manager");
	bool ret = true;

	return ret;
}

bool TransitionManager::PreUpdate()
{
	return true;
}

bool TransitionManager::Update(float dt)
{
	activatorTransition = app->sceneManager->GetOnTransition();
	this->deltaTime=dt;
	return true;
}

bool TransitionManager::PostUpdate()
{
	if (activatorTransition == true)
	{
		framesCounter++;
		Transition1();
	}
	else
	{
		framesCounter = 0;
	}
	return true;
}


bool TransitionManager::CleanUp()
{
	return true;
}


void TransitionManager::Transition1()
{
	render->DrawRectangle({ transit1 }, 20, 20, 20, 255);
	transit1.x = -app->render->camera.x;
	transit1.y = -app->render->camera.y;
	transit1.h = WINDOW_H;
	transit1.w = EaseLinearInOut(framesCounter, -app->render->camera.x, WINDOW_W, 24);
}