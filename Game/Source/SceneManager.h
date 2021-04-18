#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "Module.h"
#include "SceneControl.h"
#include "Window.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include "TeleportNode.h"

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
class SceneBattle;
class TransitionManager;

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

	TTF_Font* GetGuiFont() { return guiFont; };

	// Called before quitting
	bool CleanUp();

	// Load state game
	bool LoadState(pugi::xml_node& data);
	// Save state game
	bool SaveState(pugi::xml_node& data)const;

	bool GetViewRectangle() { return ViewRectangles; };

	int GetEnemeyDetected() { return enemyDetected; }
	void SetEnemeyDetected(bool ret) { enemyDetected = ret; }

	SceneControl* GetCurrentScene() { return current; };
	bool GetOnTransition() { return onTransition; };

	bool GetWinBattle() { return win; };
	void SetWinBattle(bool ret) { win = ret; };
	bool GetLoseBattle() { return lose; };
	void SetLoseBattle(bool ret) { lose = ret; };

private:

	Input* input = nullptr;
	Render* render = nullptr;
	Textures* tex = nullptr;

	uint transitionFx;

	SceneControl* current = nullptr;
	SceneControl* next = nullptr;
	TransitionManager* transition = nullptr;

	// Required variables to manage screen transitions (fade-in, fade-out)
	bool onTransition =false;
	bool fadeOutCompleted = false;
	float transitionAlpha = 0.0f;
	bool pause = false;

	bool enemyDetected = false;

	List<SceneControl*> scenes;
	bool ViewRectangles = false;
	bool win = false;
	bool lose = false;

public:
	SceneLogo* sceneLogo = nullptr;
	Scene* scene = nullptr;
	SceneIntro* sceneIntro = nullptr;
	SceneWin* sceneWin = nullptr;
	SceneLose* sceneLose = nullptr;
	SceneLevel2* sceneLevel2 = nullptr;
	SceneBattle* sceneBattle = nullptr;

	SceneControl* sceneControl = nullptr;

	//Origin player tp
	TeleportNode* originTpNode = nullptr;

	int lastLevel = 0;
	TTF_Font* guiFont = nullptr;
	TTF_Font* titleFont = nullptr;
};

#endif // __SCENEMANAGER_H__