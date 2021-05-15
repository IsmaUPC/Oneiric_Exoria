#ifndef __SCENECONTROL_H__
#define __SCENECONTROL_H__

#include "SString.h"
#include "App.h"

class Input;
class Render;
class Textures;

class GuiControl;

enum class SceneType
{
	LOGO,
	INTRO,
	WIN,
	LOSE,
	BATTLE,
	LEVEL1 = 5,
	LEVEL2 =6,
	LEVEL3 =7,
};

class Fx
{
public:
	uint fxName = 0;
	int channel = -1;
	iPoint position = { 0,0 };
	int maxDistance = 0;
};


class SceneControl
{
public:

	SceneControl() : active(true), loaded(false), transitionRequired(false) {}

	virtual bool Start()
	{
		return true;
	}

	virtual bool PreUpdate()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool PostUpdate()
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	void TransitionToScene(SceneType scene)
	{
		transitionRequired = true;
		nextScene = scene;
	}

	// Define multiple Gui Event methods
	virtual bool OnGuiMouseClickEvent(GuiControl* control)
	{
		return true;
	}
	// Load state game
	virtual bool LoadState(pugi::xml_node& data)
	{
		return true;
	}
	// Save state game
	virtual bool SaveState(pugi::xml_node& data)const
	{
		return true;
	}
	// Distance Fx
	int DistanceToListener(iPoint player, iPoint channel)
	{
		iPoint pos;
		pos.x = player.x - channel.x;
		pos.y = player.y - channel.y;

		return sqrt(pow(pos.x, 2) + pow(pos.y, 2));
	}
	// Angle Fx
	int AngleToListener(iPoint player, iPoint channel)
	{
		iPoint pos;
		pos.x = player.x - channel.x;
		pos.y = player.y - channel.y;

		return atan2(pos.y, pos.x);
	}
public:

	bool active = true;
	SString name;

	bool victory=false;
	bool lose=false;

	// Possible properties
	bool loaded = false;

	bool transitionRequired = false;
	SceneType nextScene;
	int lastLevel = 0;
	bool isContinue = false;

	//Fx list
	Fx fxList[10];
	int fxCount = 0;
};

#endif // __SCENECONTROL_H__