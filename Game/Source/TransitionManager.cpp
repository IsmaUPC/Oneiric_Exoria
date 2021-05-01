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
	if (app->sceneManager->GetOnTransition() && endTransition) 
	{
		activatorTransition = true;
		endTransition = false;
	}

	if (activatorTransition == true)
	{
		if (doorRand == true)
		{
			randT = rand() % MAX_TRANSITIONS;
			randT = 0;
			InitParameters();
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
		for (int i = 0; i < MAX_RECTANGLES; i++)
		{
			if (transit1[i].w != 0 || transit1[i].h != 0)render->DrawRectangle(transit1[i], 255, 0, 0, 255);
		}
		
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
			transit1[0].x = EaseLinearIn(currentIteration, initialPos, WINDOW_W, totalIterations);
									
			if (currentIteration >= totalIterations)
			{
				state = 1;
				initialPos += WINDOW_W;
				transit1[0].x = initialPos;
			}
			break;
		case 1:
			timeCounter += dt;
			if (timeCounter >= 1.0f)
			{
				state = 2;
				midTransition = true;
				currentIteration = 0;				
			}
			break;
		case 2:
			transit1[0].x = EaseLinearOut(currentIteration, initialPos, WINDOW_W, totalIterations);
			transit1[0].y = -app->render->camera.y;
			if (currentIteration >= totalIterations) Reset();
			break;

		default:
			break;
		}		
		break;

	case 1:
		switch (state)
		{
		case 0:
			transit1[0].w = EaseBounceIn(currentIteration, initialPos, WINDOW_W/2, totalIterations);
			transit1[0].h = EaseBounceIn(currentIteration, initialPos, WINDOW_H / 2, totalIterations);

			transit1[1].w = EaseBounceIn(currentIteration, initialPos, -WINDOW_W / 2, totalIterations);
			transit1[1].h = EaseBounceIn(currentIteration, initialPos, WINDOW_H / 2, totalIterations);

			transit1[2].w = EaseBounceIn(currentIteration, initialPos, -WINDOW_W / 2, totalIterations);
			transit1[2].h = EaseBounceIn(currentIteration, initialPos, -WINDOW_H / 2, totalIterations);

			transit1[3].w = EaseBounceIn(currentIteration, initialPos, WINDOW_W / 2, totalIterations);
			transit1[3].h = EaseBounceIn(currentIteration, initialPos, -WINDOW_H / 2, totalIterations);

			if (currentIteration >= totalIterations)
			{
				state = 1;
			}
			break;
		case 1:
			timeCounter += dt;
			if (timeCounter >= 1.0f)
			{
				state = 2;
				midTransition = true;
				currentIteration = 0;
			}
			break;
		case 2:
			transit1[0].x = -app->render->camera.x;
			transit1[0].y = -app->render->camera.y;

			transit1[1].x = -app->render->camera.x + WINDOW_W;
			transit1[1].y = -app->render->camera.y;

			transit1[2].x = -app->render->camera.x + WINDOW_W;
			transit1[2].y = -app->render->camera.y + WINDOW_H;

			transit1[3].x = -app->render->camera.x;
			transit1[3].y = -app->render->camera.y + WINDOW_H;

			transit1[0].w = EaseBounceOut(currentIteration, initialPos, -WINDOW_W / 2, totalIterations);
			transit1[0].h = EaseBounceOut(currentIteration, initialPos, -WINDOW_H / 2, totalIterations);

			transit1[1].w = EaseBounceOut(currentIteration, initialPos, WINDOW_W / 2, totalIterations);
			transit1[1].h = EaseBounceOut(currentIteration, initialPos, -WINDOW_H / 2, totalIterations);

			transit1[2].w = EaseBounceOut(currentIteration, initialPos, WINDOW_W / 2, totalIterations);
			transit1[2].h = EaseBounceOut(currentIteration, initialPos, WINDOW_H / 2, totalIterations);

			transit1[3].w = EaseBounceOut(currentIteration, initialPos, -WINDOW_W / 2, totalIterations);
			transit1[3].h = EaseBounceOut(currentIteration, initialPos, WINDOW_H / 2, totalIterations);
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

void TransitionManager::InitParameters()
{
	switch (randT)
	{
	case 0:
		totalIterations = 60;
		initialPos = -app->render->camera.x - WINDOW_W;
		transit1[0].y = -app->render->camera.y;
		transit1[0].w = WINDOW_W;
		transit1[0].h = WINDOW_H;
		break;
	case 1:
		totalIterations = 60;
		initialPos = -app->render->camera.x;

		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x + WINDOW_W;
		transit1[1].y = -app->render->camera.y;

		transit1[2].x = -app->render->camera.x + WINDOW_W;
		transit1[2].y = -app->render->camera.y + WINDOW_H;

		transit1[3].x = -app->render->camera.x;
		transit1[3].y = -app->render->camera.y + WINDOW_H;

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
	endTransition = true;
	state = 0;
	timeCounter = 0;
	for (int i = 0; i < MAX_RECTANGLES; i++)
	{
		transit1[i].h = 0;
		transit1[i].w = 0;
	}
}