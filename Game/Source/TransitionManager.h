#ifndef __TRANSITIONMANAGER_H__
#define __TRANSITIONMANAGER_H__

#include "Module.h"
#include "easings.h"
#include "SDL\include\SDL_rect.h"

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

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// A frame count system to handle the fade time and ratio
	
	void ActivateTransition();
	void Transition1();

private:
	
	float framesCounter = 0.0f;
	float maxFadeFrames = 0.0f;
	float deltaTime = 0.0f;
	SDL_Rect transit1;
	Render* render = nullptr;
	Textures* tex = nullptr;
	bool activatorTransition = false;

};

#endif // !__TRANSITIONMANAGER_H
