#include "ModuleFadeToBlack.h"

#include "App.h"
#include "Scene.h"
#include "SceneLevel2.h"
#include "Render.h"

#include "SDL/include/SDL_render.h"
#include "Log.h"

ModuleFadeToBlack::ModuleFadeToBlack()
{
	name.Create("fade");

}

ModuleFadeToBlack::~ModuleFadeToBlack()
{


}

bool ModuleFadeToBlack::Start()
{
	LOG("Preparing Fade Screen");
	screenRect = { 0, 0, WINDOW_W, WINDOW_H };
	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool ModuleFadeToBlack::PreUpdate()
{
	return true;
}

bool ModuleFadeToBlack::Update(float dt)
{
	// Exit this function if we are not performing a fade
	if (currentStep == FadeStep::NONE) return true;

	if (currentStep == FadeStep::TO_BLACK)
	{
		++frameCount;
		if (frameCount >= maxFadeFrames)
		{
			currentStep = FadeStep::FROM_BLACK;
			if (moduleToDisable->active == true)
			{
				moduleToDisable->CleanUp();
			}
			if (moduleToEnable->active == false) 
{
				moduleToEnable->Init();
				moduleToEnable->Start();
			}
		}
	}
	else
	{
		--frameCount;
		if (frameCount <= 0)
		{
			currentStep = FadeStep::NONE;
		}
	}
	
	return true;
}

bool ModuleFadeToBlack::PostUpdate()
{
	// Exit this function if we are not performing a fade
	if (currentStep == FadeStep::NONE) return true;

	float fadeRatio = (float)frameCount / (float)maxFadeFrames;

	// Render the black square with alpha on the screen
	SDL_SetRenderDrawColor(app->render->renderer, 0, 0, 0, (Uint8)(fadeRatio * 255.0f));
	screenRect.x = app->render->camera.x;
	screenRect.y = app->render->camera.y;
	SDL_RenderFillRect(app->render->renderer, &screenRect);
	
	return true;
}

bool ModuleFadeToBlack::CleanUp()
{
	moduleToDisable = nullptr;
	moduleToEnable = nullptr;
	lastLevel = nullptr;

	return true;
}

bool ModuleFadeToBlack::FadeToBlack(Module* toDisable, Module* toEnable, float frames)
{
	bool ret = true;
	moduleToDisable = toDisable;
	moduleToEnable = toEnable;

	frameCount = 0;
	if (currentStep == FadeStep::NONE) 
	{
		currentStep = FadeStep::TO_BLACK;
		maxFadeFrames = frames;	
		ret = true;
	}


	return ret;
}

