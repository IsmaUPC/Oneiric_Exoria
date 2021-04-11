#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "SceneControl.h"
struct SDL_Texture;

class SceneBattle : public SceneControl
{
public:
	SceneBattle();

	// Destructor
	virtual ~SceneBattle();

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
	SDL_Texture* img;
};
#endif //__SCENE_BATTLE_H__