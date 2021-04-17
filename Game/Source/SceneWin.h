#ifndef __SCENEWIN_H__
#define __SCENEWIN_H__

#include "SceneControl.h"
#include "Animation.h"

struct SDL_Texture;

struct AnimationWin
{
	iPoint position = { 0,0 };
	Animation* currentAnimation = nullptr;
	SDL_Texture* texture = nullptr;
};

class SceneWin :public SceneControl
{
public:

	SceneWin();

	// Destructor
	virtual ~SceneWin();

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

	int GetNumThisScene()
	{
		return 0;
	};


private:
	AnimationWin animationSon;
	Animation idleAnimSon;
	AnimationWin animationFather;
	Animation idleAnimFather;

	SDL_Texture* img = nullptr;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	bool transition = false;

	Timer timer;
};

#endif // !__SCENEWIN_H__
