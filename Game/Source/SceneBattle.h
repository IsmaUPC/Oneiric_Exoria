#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "SceneControl.h"
#include "GuiButton.h"
#include "Animation.h"
#include "PugiXml\src\pugixml.hpp"
#include <string>

struct SDL_Texture;

struct Magic
{
	int ID=0;
	int level =0;
	SString name = "";
	int damage=0;
	int mana=0;
	SString description = "";
};

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
	void DrawBarLives();
	void DrawTurnBar();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);
	int CalculateExp(int level);
	void AssignEntities();
	void SpeedAnimationCheck(float dt);

	bool loadMagics(const char*);

	void BubbleSort();
	void DisplaceToLeft();

private:

	SDL_Rect rec;
	SDL_Rect live;
	SDL_Rect face;

	SDL_Color green;
	SDL_Color yellow;
	SDL_Color red;
	SDL_Color violet;
	SDL_Color orange;

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
	List<Animation*> spritesBarTurn;

	List<Entity*> enemies;
	List<Entity*> partners;
	Entity* turnSort;

	pugi::xml_document magicDoc;
	List<Magic*> magics;

	int turn = 0;
	int tam = 0;
	bool assigneDone = false;
};
#endif //__SCENE_BATTLE_H__