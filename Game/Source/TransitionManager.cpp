#include "TransitionManager.h"
#include "SceneManager.h"

#include "Render.h"

#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

#define LOGO_FADE_SPEED 7

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
bool TransitionManager::Start()
{
	logo = app->tex->Load("Textures/logo_title.png");
	SDL_QueryTexture(logo, NULL, NULL, &dimensionLogo.w, &dimensionLogo.h);

	return true;
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
			if(app->sceneManager->ending == true) randT = MAX_TRANSITIONS;
			InitParameters();
			doorRand = false;
		}
		Transition1(dt);
		if (currentIteration < totalIterations)
		{
			currentIteration++;
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
			if (transit1[i].w != 0 || transit1[i].h != 0)render->DrawRectangle(transit1[i], 30, 30, 30, 255);
		}	
		
		switch (randT)
		{
		case 0:
			rect = { 0, 0, dimensionLogo.w /2,dimensionLogo.h};
			app->render->DrawTexture(logo, transit1[0].x + transit1[0].w - dimensionLogo.w / 2, transit1[0].y + WINDOW_H /2 - dimensionLogo.h/2, &rect);
			rect = { dimensionLogo.w / 2, 0, dimensionLogo.w /2,dimensionLogo.h };
			app->render->DrawTexture(logo, transit1[1].x + transit1[1].w, transit1[1].y + WINDOW_H / 2 - dimensionLogo.h / 2, &rect);
			break;
		case 1:
			rect = { 0, 0, dimensionLogo.w / 2, dimensionLogo.h / 2 };
			app->render->DrawTexture(logo, transit1[0].x + transit1[0].w - dimensionLogo.w / 2, transit1[0].y + transit1[0].h - dimensionLogo.h / 2, &rect);

			rect = { dimensionLogo.w / 2, 0, dimensionLogo.w / 2, dimensionLogo.h / 2 };
			app->render->DrawTexture(logo, transit1[1].x + transit1[1].w, transit1[1].y + transit1[1].h - dimensionLogo.h / 2, &rect);

			rect = { dimensionLogo.w / 2, dimensionLogo.h / 2, dimensionLogo.w / 2, dimensionLogo.h / 2 };
			app->render->DrawTexture(logo, transit1[2].x + transit1[2].w, transit1[2].y + transit1[2].h, &rect);

			rect = { 0, dimensionLogo.h / 2, dimensionLogo.w / 2, dimensionLogo.h / 2 };
			app->render->DrawTexture(logo, transit1[3].x + transit1[3].w - dimensionLogo.w / 2, transit1[3].y + transit1[3].h, &rect);

			break;
		case 2:
			rect = { 0, 0, dimensionLogo.w,dimensionLogo.h/2 };
			app->render->DrawTexture(logo, transit1[0].x + WINDOW_W / 2 - dimensionLogo.w / 2, transit1[0].y + transit1[0].h - dimensionLogo.h / 2, &rect);
			rect = { 0, dimensionLogo.h / 2, dimensionLogo.w,dimensionLogo.h / 2 };
			app->render->DrawTexture(logo, transit1[1].x + WINDOW_W / 2 - dimensionLogo.w / 2, transit1[1].y + transit1[1].h, &rect);
			break;
		case 3:
			app->render->DrawRectangle({ -app->render->camera.x, -app->render->camera.y, WINDOW_W, WINDOW_H }, 255, 255, 255, logoAlpha);
			break;
		default:
			break;
		}
	}
	
	return true;
}

bool TransitionManager::CleanUp()
{
	app->tex->UnLoad(logo);
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
			transit1[0].w = EaseBounceIn(currentIteration, 0, WINDOW_W / 2, totalIterations);
			transit1[1].w = EaseBounceIn(currentIteration, 0, -WINDOW_W / 2, totalIterations);
			if (currentIteration >= totalIterations) state = 1;

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

			transit1[0].w = EaseBounceOut(currentIteration, WINDOW_W / 2, -WINDOW_W / 2, totalIterations);
			transit1[1].w = EaseBounceOut(currentIteration, -WINDOW_W / 2, WINDOW_W / 2, totalIterations);
			if (currentIteration >= totalIterations) Reset();
			break;

		default:
			break;
		}		
		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x + WINDOW_W;
		transit1[1].y = -app->render->camera.y;
		break;

	case 1:
		switch (state)
		{
		case 0:
			transit1[0].w = EaseBounceIn(currentIteration, 0, WINDOW_W/2, totalIterations);
			transit1[0].h = EaseBounceIn(currentIteration, 0, WINDOW_H / 2, totalIterations);
			
			transit1[1].w = EaseBounceIn(currentIteration, 0, -WINDOW_W / 2, totalIterations);
			transit1[1].h = EaseBounceIn(currentIteration, 0, WINDOW_H / 2, totalIterations);

			transit1[2].w = EaseBounceIn(currentIteration, 0, -WINDOW_W / 2, totalIterations);
			transit1[2].h = EaseBounceIn(currentIteration, 0, -WINDOW_H / 2, totalIterations);

			transit1[3].w = EaseBounceIn(currentIteration, 0, WINDOW_W / 2, totalIterations);
			transit1[3].h = EaseBounceIn(currentIteration, 0, -WINDOW_H / 2, totalIterations);

			if (currentIteration >= totalIterations) state = 1;
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
			
			transit1[0].w = EaseBounceOut(currentIteration, WINDOW_W / 2, -WINDOW_W / 2, totalIterations);
			transit1[0].h = EaseBounceOut(currentIteration, WINDOW_H / 2, -WINDOW_H / 2, totalIterations);

			transit1[1].w = EaseBounceOut(currentIteration, -WINDOW_W / 2, WINDOW_W / 2, totalIterations);
			transit1[1].h = EaseBounceOut(currentIteration, WINDOW_H / 2, -WINDOW_H / 2, totalIterations);

			transit1[2].w = EaseBounceOut(currentIteration, -WINDOW_W / 2, WINDOW_W / 2, totalIterations);
			transit1[2].h = EaseBounceOut(currentIteration, -WINDOW_H / 2, WINDOW_H / 2, totalIterations);

			transit1[3].w = EaseBounceOut(currentIteration, WINDOW_W / 2, -WINDOW_W / 2, totalIterations);
			transit1[3].h = EaseBounceOut(currentIteration, -WINDOW_H / 2, WINDOW_H / 2, totalIterations);
			if (currentIteration >= totalIterations) Reset();

			break;
		default:
			break;
		}
		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x + WINDOW_W;
		transit1[1].y = -app->render->camera.y;

		transit1[2].x = -app->render->camera.x + WINDOW_W;
		transit1[2].y = -app->render->camera.y + WINDOW_H;

		transit1[3].x = -app->render->camera.x;
		transit1[3].y = -app->render->camera.y + WINDOW_H;
		break;
	case 2:
		switch (state)
		{
		case 0:
			transit1[0].h = EaseBounceIn(currentIteration, 0, WINDOW_H / 2, totalIterations);
			transit1[1].h = EaseBounceIn(currentIteration, 0, -WINDOW_H / 2, totalIterations);
			if (currentIteration >= totalIterations) state = 1;

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
			
			transit1[0].h = EaseBounceOut(currentIteration, WINDOW_H / 2, -WINDOW_H / 2, totalIterations);
			transit1[1].h = EaseBounceOut(currentIteration, -WINDOW_H / 2, WINDOW_H / 2, totalIterations);
			if (currentIteration >= totalIterations) Reset();

			break;
		default:
			break;
		}
		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x;
		transit1[1].y = -app->render->camera.y + WINDOW_H;
		break;
	case 3:
		switch (state)
		{
		case 0:
			logoAlpha += (LOGO_FADE_SPEED);

			if (logoAlpha > 255.0f)
			{
				logoAlpha = 255.0f;
				state = 1;
			}
			break;
		case 1:
			timeCounter += dt;
			if (timeCounter >= 0.5f)
			{
				state = 2;
				midTransition = true;
			}
			break;
		case 2:
			if (logoAlpha > 0)logoAlpha -= (LOGO_FADE_SPEED);
			else
			{
				Reset();
				app->sceneManager->ending = false;
			}
		default:
			break;
		}
	default:
		break;
	}	
}

void TransitionManager::InitParameters()
{
	switch (randT)
	{
	case 0:
		totalIterations = 80;
		initialPos = -app->render->camera.x;
		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x + WINDOW_W;
		transit1[1].y = -app->render->camera.y;

		transit1[0].h = WINDOW_H;
		transit1[1].h = WINDOW_H;
		break;
	case 1:
		totalIterations = 80;
		initialPos = -app->render->camera.x;

		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x + WINDOW_W;
		transit1[1].y = -app->render->camera.y;

		transit1[2].x = -app->render->camera.x + WINDOW_W;
		transit1[2].y = -app->render->camera.y + WINDOW_H;

		transit1[3].x = -app->render->camera.x;
		transit1[3].y = -app->render->camera.y + WINDOW_H;
		break;
	case 2:
		totalIterations = 80;
		initialPos = -app->render->camera.x;
		transit1[0].x = -app->render->camera.x;
		transit1[0].y = -app->render->camera.y;

		transit1[1].x = -app->render->camera.x;
		transit1[1].y = -app->render->camera.y + WINDOW_H;

		transit1[0].w = WINDOW_W;
		transit1[1].w = WINDOW_W;
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
	endTransition = true;
	state = 0;
	timeCounter = 0;
	logoAlpha = 0;
	for (int i = 0; i < MAX_RECTANGLES; i++)
	{
		transit1[i].h = 0;
		transit1[i].w = 0;
	}
}