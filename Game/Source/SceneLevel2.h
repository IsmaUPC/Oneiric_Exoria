#ifndef __SCENE_LEVEL_2_H__
#define __SCENE_LEVEL_2_H__

#include "SceneControl.h"
#include "Point.h"
#include "Animation.h"
#include "PugiXml\src\pugixml.hpp"

struct SDL_Texture;

struct AnimationFather2
{
	iPoint position;
	Animation* currentAnimation;
	SDL_Texture* texture;
};

class SceneLevel2 : public SceneControl
{
public:

	SceneLevel2();

	// Destructor
	virtual ~SceneLevel2();

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
		return 0;
	};
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
	AnimationFather2 animationFather;
	Animation idleAnim;
	SDL_Texture* img;
	int moveBG0;
	int moveBG1;
	int moveBG2;
	int withBG;
	int posX0;
	int posX1;
	int posX2;
	int xW;
	int xSpeed;

	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	float speedImg = 0;
};

#endif // __SCENELEVEL2_H__

