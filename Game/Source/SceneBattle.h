#ifndef __SCENE_BATTLE_H__
#define __SCENE_BATTLE_H__

#include "SceneControl.h"
#include "GuiButton.h"
#include "Animation.h"
#include "GuiMenuMagic.h"
#include "EntityManager.h"

#include "PugiXml\src\pugixml.hpp"
#include <string>

struct Magic
{
	int id = 0;
	int type = 0;
	int level = 0;
	SString name = "";
	int damage = 0;
	int mana = 0;
	SString description = "";
	int element = 0;
};

enum FaseAttack
{
	SELECT_ACTION,
	SELECT_ENEMY,
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

	void Transition(float dt_);

	void UpdateAnimationEnemies();
	void BattleSystem();
	void CheckWinLose();

	// Called before all Updates
	bool PostUpdate();

	void IconEnemySelected();
	void DrawAllBattlesElements();
	void DrawBarLives();
	void DrawTurnBar();

	void DrawSceneWin();
	void GenerateItems();
	void DrawSceneLose();
	void DrawBarExperience();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);
	int CalculateExp(int level);
	void AssignEntities();
	void SpeedAnimationCheck(float dt);

	// Turn bar
	void BubbleSort();
	void DisplaceToLeft();

	void AbleDisableButtons();
	void AbleButtons();
	
	void SaveStateLose();
	void SaveState(TypeEntity pType, int i);
	void ContinueGame();

private:

	void HealTeam();

	void UseAMagic();

	SDL_Rect rec = { 0,0,0,0 };
	SDL_Rect live = { 0,0,0,0 };
	SDL_Rect tex = { 0,0,0,0 };
	SDL_Rect face = { 0,0,0,0 };

	SDL_Color green = { 0,187, 45 };
	SDL_Color yellow = { 229,190,1 };
	SDL_Color red = { 203,50,52 };
	SDL_Color blue = {37, 40,80};
	SDL_Color cyan = { 0, 183,255 };
	SDL_Color orange = {255,136,18};
	SDL_Color white = { 255,255,255 };
	SDL_Color black = { 0,0,0 };

	GuiButton* btnAttack = nullptr;
	GuiButton* btnMagic = nullptr;
	GuiButton* btnDefense = nullptr;
	GuiButton* btnExit = nullptr;
	GuiButton* btnContinue = nullptr;

	SDL_Texture* img = nullptr;
	SDL_Texture* texPlayers = nullptr;
	SDL_Texture* texEnemies = nullptr;
	SDL_Texture* enemySelect = nullptr;

	List<Animation*> spritesBarTurn;
	List<Animation*> animationsPlayer;
	List<Animation*> animationsHitPlayer;
	List<Animation*> animationsEnemies;
	List<Animation*> animationsHitEnemies;
	List<Animation*> animationsAttackEnemies;

	List<Entity*> enemies;
	List<Entity*> partners;
	Entity* turnSort = nullptr;

	uint fxWin = -1;
	uint fxLose = -1;
	uint fxAttack = -1;
	uint fxDefense = -1;
	uint fxExit = -1;
	uint fxFireball = -1;
	uint fxShadow = -1;
	uint fxHealing = -1;
	uint fxBanditDies = -1;
	uint fxFighterDies = -1;
	uint fxSaplingDies = -1;
	uint fxAllyDies = -1;
	uint fxCancelButton = -1;

	GuiMenuMagic* menuMagic = nullptr;
	pugi::xml_document saveFile;
	
	Magic* magicInUse = nullptr;
	bool activeMenuMagic =false;

	int itemsId[2] = {0,0};
	int itemMulti = 1;

	int turn = 0;
	int tam = 0;
	int numArrows = 0;
	float dt = 0;
	float offset = 38;
	char textVictory[9] = { "VICTORY!" };
	char textLoose[28] = { "YOU LOOSE" };
	char textLive[8] = { "\0" };
	char textLevel[7] = { "\0" };
	char textItems[24] = { "\0" };
	char textExperience[14] = { "\0" };

	int faseAction = 0;
	int enemySelected = 0;
	int ally=0;
	bool hit=0;
	float newHealth = 0;
	float newHealthTeam[4] = { 0 };

	int reduceLieveVelocity = 8;

	bool moveBarTurn = false;
	bool assigneDone = false;
	bool win = false;
	bool lose = false;
	bool god = false;
	int totalExp = 0;
	float currentExp = 0;
	
	bool missClick = false;
	bool initCombat = true;
	int indexTurnBar = -1;

	int posX = 0;
	int posY = 0;

	//Easings title
	float currentIteration;
	float totalIterations;
	float initialPosition;
	float deltaPosition;

	int state = 0;
	bool flash = false;
	bool drawEntities = false;
	float logoAlpha = 0;
	float timeCounter = 0;

	float posBackgroundLeft = -1280;
	float posBackgroundRight = 1280;
};
#endif //__SCENE_BATTLE_H__