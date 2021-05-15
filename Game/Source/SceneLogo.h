#ifndef __SCENELOGO_H__
#define __SCENELOGO_H__

#include "SceneControl.h"
#include "Animation.h"
struct SDL_Texture;

class SceneLogo : public SceneControl
{
public:

	SceneLogo();

	// Destructor
	virtual ~SceneLogo();

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


private:

	SDL_Rect logoRect;
	SDL_Texture* logo = nullptr;
	SDL_Texture* cracks = nullptr;
	SDL_Texture* license = nullptr;
	SDL_Texture* names = nullptr;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	uint fxIntro = -1;

	SDL_Color color;		// Tint color

	int state;
	bool cracksOn = false;
	float scale = 0;
	float timeCounter = 0;
	float logoAlpha = 0;	
};

#endif // __SCENELOGO_H__