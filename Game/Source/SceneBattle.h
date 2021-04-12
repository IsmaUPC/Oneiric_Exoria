#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "SceneControl.h"
#include "GuiButton.h"
#include "Animation.h"

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
	void AddBattleMenu(SDL_Texture* btnTextureAtlas);
	void InicializeStats();
	void AddPartners();
	void AddEntities();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);
	int CalculateExp(int level);
	void AssignEntities();


private:

	SDL_Rect rec;
	SDL_Rect live;
	SDL_Color green;
	SDL_Color yellow;
	SDL_Color red;

	GuiButton* btnAttack;
	GuiButton* btnMagic;
	GuiButton* btnDefense;
	GuiButton* btnExit;

	SDL_Texture* img;
	SDL_Texture* texPalyers;

	Animation* idleKenzie;
	Animation* idleKeiler;
	Animation* idleIsrra;
	Animation* idleBrenda;

	List<Entity*> enemies;
	List<Entity*> partners;
	bool assigneDone = false;
};
#endif //__SCENE_BATTLE_H__