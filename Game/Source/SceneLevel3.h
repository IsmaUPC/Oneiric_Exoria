#ifndef __SCENE_LEVEL_3_H__
#define __SCENE_LEVEL_3_H__

#include "SceneControl.h"
#include "Point.h"
#include "Animation.h"
#include "GuiManager.h"
#include "PugiXml\src\pugixml.hpp"

struct SDL_Texture;

class SceneLevel3 : public SceneControl
{
public:

	SceneLevel3();

	// Destructor
	virtual ~SceneLevel3();

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

	void UpdateDialog();

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	int GetNumThisScene()
	{
		return numThisScene;
	};
	bool OnGuiMouseClickEvent(GuiControl* control);
	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;


private:

	int numThisScene = 2;

	void DebugKeys();

	bool debugCollisions = false;
	bool victory = false;
	bool lose = false;

	int moveBG0 = 0;
	int moveBG1 = 0;
	int moveBG2 = 0;
	int withBG = 0;
	int posX0 = 0;
	int posX1 = 0;
	int posX2 = 0;
	int xW = 0;
	int xSpeed = 0;

	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	float speedImg = 0;

	GuiButton* btn1 = nullptr;
	GuiButton* btn2 = nullptr;
	GuiButton* btn3 = nullptr;
};

#endif // __SCENELEVEL_3_H__

