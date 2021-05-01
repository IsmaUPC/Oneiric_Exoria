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
	if (app->sceneManager->GetOnTransition()) activatorTransition = true;
	
	if (activatorTransition == true)
	{
		if (doorRand == true)
		{
			randT = rand() % MAX_TRANSITIONS;
			randT = 0;
			totalIterations = 50;
			initialPos = -app->render->camera.x - WINDOW_W;
			doorRand = false;
		}
		Transition1(dt);
		if (currentIteration < totalIterations)
		{
			currentIteration += dt * 60;
		}
	
	}

	return true;
}

bool TransitionManager::PostUpdate()
{
	if (activatorTransition == true)
	{
		render->DrawRectangle(transit1, 20, 20, 20, 255);
	}
	
	return true;
}

bool TransitionManager::CleanUp()
{
	return true;
}

void TransitionManager::Transition1(float dt)
{
	switch (randT)
	{
	case 0:
		switch (state)
		{
		case 0:
			transit1.x = EaseLinearInOut(currentIteration, initialPos, WINDOW_W, totalIterations);
			transit1.y = -app->render->camera.y;
			transit1.w = WINDOW_W;
			transit1.h = WINDOW_H;
						
			if (currentIteration >= totalIterations)
			{
				state = 1;
				initialPos = transit1.x;
			}
			break;
		case 1:
			timeCounter += dt;
			if (timeCounter >= 0.5f)
			{
				state = 2;
				midTransition = true;
				currentIteration = 0;				
			}
			break;
		case 2:
			transit1.x = EaseLinearInOut(currentIteration, initialPos, WINDOW_W, totalIterations);
			transit1.y = -app->render->camera.y;
			transit1.w = WINDOW_W;
			transit1.h = WINDOW_H;
			if (currentIteration >= totalIterations) Reset();

			break;
		default:
			break;
		}
		
		break;
	default:
		break;
	}	
}

void TransitionManager::Reset() 
{
	currentIteration = 0;
	doorRand = true;
	activatorTransition = false;
	midTransition = false;
	state = 0;
}