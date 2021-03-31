#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "Module.h"
#include "SceneControl.h"
#include "GuiMenuPause.h"
#include "Window.h"

class GuiButton;

class Input;
class Render;
class Textures;

class SceneLogo;
class Scene;
class SceneIntro;
class SceneWin;
class SceneLose;
class SceneLevel2;

class SceneManager : public Module
{
public:

	SceneManager(Input* input, Render* render, Textures* tex);

	// Destructor
	virtual ~SceneManager();

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

	// Add Scenes
	void AddScene(SceneControl* scene, bool active);

	// Return Pause
	bool GetIsPause() { return pause; };
	void SetPause(bool statePause) { pause = statePause; };

	int GetGuiFont() { return guiFont; };

	// Called before quitting
	bool CleanUp();

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;

	bool GetViewRectangle() { return ViewRectangles; };

	GuiMenuPause* menu;

private:

	Input* input;
	Render* render;
	Textures* tex;
	
	SceneControl* current;
	SceneControl* next;

	// Required variables to manage screen transitions (fade-in, fade-out)
	bool onTransition;
	bool fadeOutCompleted;
	float transitionAlpha;
	bool pause = false;
	List<SceneControl*> scenes;

	bool ViewRectangles = false;

public:
	SceneLogo* sceneLogo;
	Scene* scene;
	SceneIntro* sceneIntro;
	SceneWin* sceneWin;
	SceneLose* sceneLose;
	SceneLevel2* sceneLevel2;

	SceneControl* sceneControl;

	SDL_Texture* btnTextureAtlas;

	uint btnSelected;
	uint btnPressed;
	uint btnDisabled;
	uint btnSlider;

	int lastLevel = 0;
	int guiFont;
};

#endif // __SCENEMANAGER_H__