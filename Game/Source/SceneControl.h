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
	LEVEL1,
	LEVEL2,
	WIN,
	LOSE
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

public:

	bool active = true;
	SString name;

	// Possible properties
	bool loaded = false;

	bool transitionRequired;
	SceneType nextScene;
	int lastLevel = 0;
	bool isContinue = false;
};

#endif // __SCENECONTROL_H__