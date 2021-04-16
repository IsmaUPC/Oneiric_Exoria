#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "SceneControl.h"
#include "GuiButton.h"
#include "Animation.h"
#include "GuiMenuMagic.h"

#include "PugiXml\src\pugixml.hpp"
#include <string>

struct Magic
{
	int id = 0;
	int level = 0;
	SString name = "";
	int damage = 0;
	int mana = 0;
	SString description = "";
};

enum FaseAttack
{
	SELECT_ACTION,
	SELECT_ENEMI,
	DO_ACITON,
	END_ACTION,
	ENEMY_ATTACK
};

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
	void LoadAnimations();
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

	void BubbleSort();
	void DisplaceToLeft();

private:

	SDL_Rect rec;
	SDL_Rect live;
	SDL_Rect face;

	SDL_Color green;
	SDL_Color yellow;
	SDL_Color red;
	SDL_Color blue;
	SDL_Color orange;

	GuiButton* btnAttack;
	GuiButton* btnMagic;
	GuiButton* btnDefense;
	GuiButton* btnExit;
	GuiButton* btnContinue;

	SDL_Texture* img;
	SDL_Texture* texPalyers;
	SDL_Texture* texEnemies;

	List<Animation*> spritesBarTurn;
	List<Animation*> animationsPlayer;
	List<Animation*> animationsEnemies;

	List<Entity*> enemies;
	List<Entity*> partners;
	Entity* turnSort;

	//GuiMenuMagic* menuMagic;
	GuiMenuMagic* menuMagic;
	
	Magic* magicInUse;
	bool activeMenuMagic;

	int turn = 0;
	int tam = 0;
	int numArrows = 0;
	float dt = 0;
	float offset = 38;

	int faseAction;
	int enemiSelected;

	bool moveBarTurn = false;
	bool assigneDone = false;
	bool win = false;
};
#endif //__SCENE_BATTLE_H__