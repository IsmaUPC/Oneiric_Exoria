#ifndef __TRANSITIONMANAGER_H__
#define __TRANSITIONMANAGER_H__

#include "Module.h"
#include "easings.h"
#include "Textures.h"
#include "SDL\include\SDL_rect.h"

#define MAX_TRANSITIONS 3
#define MAX_RECTANGLES 4

class Render;
class Textures;
class SceneManager;


class TransitionManager : public Module 
{
public:
	//Constructor
	TransitionManager(Render* render, Textures* tex, SceneManager* sceneManager);

	//Destructor
	virtual ~TransitionManager();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// A frame count system to handle the fade time and ratio
	
	void Transition1(float dt);
	void InitParameters();
	void Reset();

	bool GetTransitionMid() { return midTransition; };

private:

	float currentIteration = 0.0f;
	float totalIterations = 0.0f;
	float initialPos = 0.0f;
	float deltaPos = 0.0f;

	SDL_Rect transit1[MAX_RECTANGLES] = { 0,0,0,0 };
	SDL_Rect dimensionLogo = { 0,0,0,0 };
	SDL_Rect rect = { 0,0,0,0 };

	Render* render = nullptr;
	Textures* tex = nullptr;
	SDL_Texture* logo = nullptr;

	bool activatorTransition = false;
	int randT = 0;
	bool doorRand = true;
	bool midTransition = false;
	bool endTransition = true;
	int state = 0;
	float timeCounter = 0;
	int logoAlpha = 0;

};

#endif // !__TRANSITIONMANAGER_H
