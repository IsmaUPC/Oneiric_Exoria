#ifndef __SCENELOSE_H__
#define __SCENELOSE_H__

#include "SceneControl.h"
#include "Animation.h"

struct SDL_Texture;

struct AnimationEnd
{
	iPoint position;
	Animation* currentAnimation;
	SDL_Texture* texture;
};

class SceneLose : public SceneControl
{
public:

	SceneLose();

	// Destructor
	virtual ~SceneLose();

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
	AnimationEnd animationEnd;
	Animation idleAnim;

	SDL_Texture* img;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	bool transition;
	bool keyPressed;

	Timer timer;
};

#endif // !__SCENELOSE_H__
