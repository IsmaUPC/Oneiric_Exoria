#ifndef __SCENE_H__
#define __SCENE_H__

#include "SceneControl.h"
#include "Animation.h"
#include "Point.h"

#include "PugiXml\src\pugixml.hpp"

struct SDL_Texture;

struct AnimationFather
{
	iPoint position;
	Animation* currentAnimation;
	SDL_Texture* texture;
};

class Scene : public SceneControl
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	void SetDebugCollaider();
	bool* GetDebugCollaider() { return &debugCollisions; }
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
		return numThisScene;
	}

	bool OnGuiMouseClickEvent(GuiControl* control);


	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;
private:

	int numThisScene;

	void Parallax();
	void DebugKeys();

	bool debugCollisions = false;
	bool victory = false;
	bool lose = false;
	AnimationFather animationFather;
	Animation* idleAnim= new Animation();
	SDL_Texture* img;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	float speedImg=0;

};

#endif // __SCENE_H__