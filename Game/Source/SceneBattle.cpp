#include "SceneBattle.h"
#include "SceneManager.h"

#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Input.h"
#include "Fonts.h"

#include "SceneManager.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

#define LOGO_FADE_SPEED 7

SceneBattle::SceneBattle()
{
	active = true;
	name.Create("sceneBattle");
}

SceneBattle::~SceneBattle()
{
}

bool SceneBattle::Awake()
{
	LOG("Loading SceneLose");
	bool ret = true;
	return ret;
}

bool SceneBattle::Start()
{
	SDL_Texture* btnTextureAtlas = app->guiManager->btnTextureAtlas;
	app->render->camera.x = app->render->camera.y = 0;

	app->sceneManager->SetEnemeyDetected(false);
	texPlayers = app->sceneManager->GetPlayerTexture();
	texEnemies = app->tex->Load("Assets/Textures/Enemies/enemies_battle.png");
	enemySelect = app->tex->Load("Assets/Textures/GUI/enemy_selector.png");

	app->audio->PlayMusic("Assets/Audio/Music/battle_music.ogg");

	fxWin = app->audio->LoadFx("Assets/Audio/Fx/win_start.wav");
	fxLose = app->audio->LoadFx("Assets/Audio/Fx/lose.wav");
	fxAttack = app->audio->LoadFx("Assets/Audio/Fx/attack.wav");
	fxDefense = app->audio->LoadFx("Assets/Audio/Fx/defense.wav");
	fxExit = app->audio->LoadFx("Assets/Audio/Fx/exit.wav");
	fxFireball = app->audio->LoadFx("Assets/Audio/Fx/spell_fireball.wav");
	fxShadow = app->audio->LoadFx("Assets/Audio/Fx/spell_shadow.wav");
	fxHealing = app->audio->LoadFx("Assets/Audio/Fx/spell_healing.wav");
	fxBanditDies = app->audio->LoadFx("Assets/Audio/Fx/bandit_slain.wav");
	fxFighterDies = app->audio->LoadFx("Assets/Audio/Fx/fighter_slain.wav");
	fxSaplingDies = app->audio->LoadFx("Assets/Audio/Fx/sapling_slain.wav");
	fxAllyDies = app->audio->LoadFx("Assets/Audio/Fx/ally_slain.wav");
	fxCancelButton = app->audio->LoadFx("Assets/Audio/Fx/button_cancel.wav");

	// Load Animations
	LoadAnimations();

	// Add Enemies and Players
	AddEntities();
	AddPartners();

	enemies = app->entityManager->spawnQueue;
	enemySelected = 0;

	// Inicialize the stats
	InicializeStats();
	app->entityManager->spawnQueue = enemies;

	TypeEntity pType;
	for (int i = 0; i < enemies.Count(); i++)
	{
		pType = enemies.At(i)->data->entityData.type;
		if (pType != KENZIE_ && pType != KEILER_ && pType != ISRRA_ && pType != BRENDA_)
			totalExp += enemies.At(i)->data->stats.exp;
	}

	int num = enemies.Count();
	turnSort = new Entity[num];
	// Gui Buttons
	AddBattleMenu(btnTextureAtlas);

	god = false;
	missClick = false;
	hit = false;

	app->sceneManager->SetTransitionToBattleFinish(false);

	return true;
}

void SceneBattle::LoadAnimations()
{
	// Load Player Animations
	for (int i = 0; i < 4; i++)
	{
		Animation* b = new Animation;
		b->speed = 0.1f;
		for (int j = 0; j < 6; j++)
		{
			if (i == 3)
			{
				b->PushBack({ 64 * j,92 * i, 64, 96 });
			}
			else b->PushBack({ 64 * j,92 * i, 64, 92 });
		}
		animationsPlayer.Add(b);
	}

	//Load player hit animations
	for (int i = 0; i < 4; i++)
	{
		Animation* b = new Animation;
		b->speed = 0.1f;
		for (int j = 0; j < 2; j++)
		{
			b->PushBack({j * 64 + ((64 * 2) * i), 672, 64, 96});
		}
		animationsHitPlayer.Add(b);
	}

	// Load Animation Bar Turn
	for (int i = 0; i < 4; i++)
	{
		Animation* b = new Animation;
		b->speed = 0.1;
		for (int j = 0; j < 4; j++)
		{
			b->PushBack({ 384,32 * j + (128 * i), 32, 32 });
		}
		spritesBarTurn.Add(b);
	}

	// Load Animations Enemies
	int numSprites = 1;
	for (int i = 0; i < 12; i++)
	{
		Animation* b = new Animation;
		if (i == 2 || i == 6 || i == 10)
		{
			b->loop = false;
		}
		else b->loop = true;
		b->speed = 0.1;
		if (i % 2) numSprites = 2;
		else numSprites = 4;
		if (i == 6) numSprites = 5;
		else if (i == 8) numSprites = 5;
		else if (i == 10) numSprites = 6;
		if ((i+1) % 4 == 0 && i != 0)
		{
			for (int j = 0; j < 4; j++)
			{
				b->PushBack({ 32 * j,64 * i, 32, 32 });
			}
			spritesBarTurn.Add(b);
		}
		else
		{
			for (int j = 0; j < numSprites; j++)
			{
				b->PushBack({ 128 * j,64 * i, 128, 64 });
			}
			animationsEnemies.Add(b);
		}
	}
}

void SceneBattle::AddEntities()
{
	int id = app->entityManager->GetCurrentEntity()->entityData.id;
	int level = app->entityManager->GetCurrentEntity()->entityData.level;
	int randomLvl = 1;
	switch (id)
	{
	case 1:
		// Load textures
		img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

		// Add Enemies
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(BANDIT, 13, 17, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(FIGHTER, 10, 15, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(BANDIT, 10, 19, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(SAPLING, 7, 17, 0, randomLvl);
		break;

	case 2:
		// Load textures
		img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

		// Add Enemies
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(FIGHTER, 13, 17, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(BANDIT, 10, 15, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(SAPLING, 10, 19, 0, randomLvl);
		break;

	case 3:
		// Load textures
		img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

		// Add Enemies
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(SAPLING, 13, 17, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(FIGHTER, 10, 15, 0, randomLvl);
		break;
	case 4:
		// Load textures
		img = app->tex->Load("Assets/Textures/Backgrounds/background_1.png");

		// Add Enemies
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(BANDIT, 13, 17, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(FIGHTER, 10, 15, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(FIGHTER, 10, 19, 0, randomLvl);
		while (randomLvl < 0) randomLvl = level + (rand() % 3);
		app->entityManager->AddEntity(SAPLING, 7, 17, 0, randomLvl);
		break;
	default:
		break;
	}
}

void SceneBattle::AddPartners()
{
	// Partners and Player
	int num = app->player->GetNumPartners();
	if(app->player->playerData.health > 0)
		app->entityManager->AddEntity(KENZIE_, 26, 13, 0, app->player->playerData.level);

	for (int i = 0; i < num; i++)
	{
		if (app->player->GetPartners()[i].health > 0)
		{
			switch (app->player->GetPartners()[i].type)
			{
			case KEILER:
				app->entityManager->AddEntity(KEILER_, 29, 15, 0, app->player->GetPartners()[i].level);
				break;
			case ISRRA:
				app->entityManager->AddEntity(ISRRA_, 29, 19, 0, app->player->GetPartners()[i].level);
				break;
			case BRENDA:
				app->entityManager->AddEntity(BRENDA_, 26, 17, 0, app->player->GetPartners()[i].level);
				break;
			default:
				break;
			}
		}		
	}
}

void SceneBattle::InicializeStats()
{
	float strong = 1;
	for (int i = 0; i < enemies.Count(); i++)
	{
		int level = enemies.At(i)->data->entityData.level;
		// Add switch for assign strong, if is a type enemy strong = x, if is other type strong = y
		TypeEntity pType = enemies.At(i)->data->entityData.type;
		if (pType == KENZIE_ || pType == KEILER_ || pType == ISRRA_ || pType == BRENDA_)
		{
			switch (pType)
			{
			case KENZIE_ :
				enemies.At(i)->data->stats.attack = 3.5 * level + 9.5;
				enemies.At(i)->data->stats.defense = 1.5 * level + 6.5;
				enemies.At(i)->data->stats.health = app->player->playerData.health;
				enemies.At(i)->data->stats.maxHealth = 2 * level + 6;
				enemies.At(i)->data->stats.mana = app->player->playerData.mana;
				enemies.At(i)->data->stats.maxMana = 3 * level +8;
				enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
				enemies.At(i)->data->stats.exp = app->player->playerData.exp;
				enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(0)->data;
				break;
			case KEILER_:
				enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
				enemies.At(i)->data->stats.defense = 1.5 * level + 5.5;
				enemies.At(i)->data->stats.health = app->player->GetPartners()[0].health;
				enemies.At(i)->data->stats.maxHealth = 4 * level + 10;
				enemies.At(i)->data->stats.mana = app->player->GetPartners()[0].mana;
				enemies.At(i)->data->stats.maxMana = 2.5 * level + 7.5;
				enemies.At(i)->data->stats.speed = 2.5 * level + 8.5;
				enemies.At(i)->data->stats.exp = app->player->GetPartners()[0].exp;
				enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(1)->data;
				break;
			case ISRRA_:
				enemies.At(i)->data->stats.attack = 2 * level + 7;
				enemies.At(i)->data->stats.defense = 2 * level + 7;
				enemies.At(i)->data->stats.health = app->player->GetPartners()[1].health;
				enemies.At(i)->data->stats.maxHealth = 2.5 * level + 7.5;
				enemies.At(i)->data->stats.mana = app->player->GetPartners()[1].mana;
				enemies.At(i)->data->stats.maxMana = 3.5 * level + 8.5;
				enemies.At(i)->data->stats.speed = 2.5 * level + 7.5;
				enemies.At(i)->data->stats.exp = app->player->GetPartners()[1].exp;
				enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(2)->data;
				break;
			case BRENDA_:
				enemies.At(i)->data->stats.attack = 1.5 * level + 6.5;
				enemies.At(i)->data->stats.defense = 3.5 * level + 9.5;
				enemies.At(i)->data->stats.health = app->player->GetPartners()[2].health;
				enemies.At(i)->data->stats.maxHealth = 3.5 * level + 9.5;
				enemies.At(i)->data->stats.mana = app->player->GetPartners()[2].mana;
				enemies.At(i)->data->stats.maxMana = 1.5 * level + 6.5;
				enemies.At(i)->data->stats.speed = 1.5 * level + 6.5;
				enemies.At(i)->data->stats.exp = app->player->GetPartners()[2].exp;
				enemies.At(i)->data->entityData.currentAnimation = animationsPlayer.At(3)->data;
				break;

			default:
				break;
			}
			enemies.At(i)->data->entityData.texture = texPlayers;
		}
		else
		{
			switch (pType)
			{
			case BANDIT:
				strong = 1;
				enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(0)->data;
				break;
			case FIGHTER:
				strong = 1.1;
				enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(3)->data;
				break;
			case SAPLING:
				strong = 1.2;
				enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(6)->data;
				break;
			default:
				break;
			}
			enemies.At(i)->data->stats.attack = ((2 * level + 7) * 0.75) * strong;
			enemies.At(i)->data->stats.defense = ((2 * level + 7) * 0.8) * strong;
			enemies.At(i)->data->stats.health = ((2.5 * level + 7.5) * 1.5) * strong;
			enemies.At(i)->data->stats.maxHealth = enemies.At(i)->data->stats.health;
			enemies.At(i)->data->stats.mana = (enemies.At(i)->data->stats.health / 2) * strong;
			enemies.At(i)->data->stats.speed = ((2.5 * level + 7.5) * 0.9) * strong;
			enemies.At(i)->data->stats.exp = (sqrt(CalculateExp(level)) / 3) * strong;

			enemies.At(i)->data->entityData.texture = texEnemies;
		}
	}
}

void SceneBattle::AddBattleMenu(SDL_Texture* btnTextureAtlas)
{
	int padding = 50;
	int yPosition = 50;
	int xPosition = WINDOW_W - 95;
	btnAttack = new GuiButton(20, { xPosition ,yPosition + (padding * 0),  183, 91 }, "ATTACK", RECTANGLE, btnTextureAtlas);
	btnAttack->SetObserver(this);
	btnAttack->active = false;
	app->guiManager->AddGuiButton(btnAttack);

	btnMagic = new GuiButton(21, { xPosition , yPosition + (padding * 1),  183, 91 }, "MAGIC", RECTANGLE, btnTextureAtlas);
	btnMagic->SetObserver(this);
	btnMagic->active = false;
	app->guiManager->AddGuiButton(btnMagic);

	btnDefense = new GuiButton(22, { xPosition,yPosition + (padding * 2),  183, 91 }, "DEFENSE", RECTANGLE, btnTextureAtlas);
	btnDefense->SetObserver(this);
	btnDefense->active = false;
	app->guiManager->AddGuiButton(btnDefense);

	btnExit = new GuiButton(23, { xPosition, yPosition + (padding * 3),  183, 91 }, "EXIT", RECTANGLE, btnTextureAtlas);
	btnExit->SetObserver(this);
	btnExit->active = false;
	app->guiManager->AddGuiButton(btnExit);

	btnContinue = new GuiButton(24, { WINDOW_W/2, WINDOW_H/2 + 150,  180, 90 }, "CONTINUE", RECTANGLE, btnTextureAtlas);
	btnContinue->SetObserver(this);
	btnContinue->active = false;
	app->guiManager->AddGuiButton(btnContinue);

	//MenuMagic
	menuMagic = new GuiMenuMagic({ WINDOW_W - 420, yPosition +20}, this);
	activeMenuMagic = false;

	// Easings inicialize variables
	currentIteration = 0;
	totalIterations = 80;
	initialPosition = -1280;
	deltaPosition = 1280;
}

bool SceneBattle::PreUpdate()
{ 
	return true;
}

bool SceneBattle::Update(float dt_)
{
	dt = dt_;

	Transition(dt_);

	if (drawEntities)
	{
		SpeedAnimationCheck(dt_);

		/*for (int i = 0; i < animationsEnemies.Count(); i++)
		{
			animationsEnemies.At(i)->data->Update();
		}*/

		if (missClick && !app->input->pads[0].a && !app->input->pads[0].left && !app->input->pads[0].right) {
			missClick = false;
		}

		UpdateAnimationEnemies();

		if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			god = !god;
		}

		if (!win && !lose) BattleSystem();

		// Win/Lose Condition
		CheckWinLose();
	}	

	return true;
}

void SceneBattle::Transition(float dt_)
{
	if (state == 0)
	{
		posBackgroundLeft = EaseCircIn(currentIteration, initialPosition, deltaPosition, totalIterations);
		posBackgroundRight = EaseCircIn(currentIteration, -initialPosition, -deltaPosition, totalIterations);
	}

	if (currentIteration < totalIterations)
	{
		++currentIteration;
	}
	else
	{
		if (logoAlpha == 0) app->audio->PlayFx(fxAttack);
		if (state == 0) state = 1;
		if (state == 1)
		{
			flash = true;
			logoAlpha += (LOGO_FADE_SPEED);

			if (logoAlpha > 255.0f)
			{
				logoAlpha = 255.0f;
				state = 2;
			}
		}
		else if (state == 2)
		{
			timeCounter += dt_;
			if (timeCounter >= 0.5f)
			{
				drawEntities = true;
				app->sceneManager->SetTransitionToBattleFinish(true);
				AbleButtons();
				state = 3;
			}
		}
		else if (state == 3)
		{
			if (logoAlpha != 0)logoAlpha -= (LOGO_FADE_SPEED);

			if (logoAlpha <= 0.0f)
			{
				flash = false;
			}
		}
	}
}

bool SceneBattle::PostUpdate()
{
	if (!assigneDone)AssignEntities();

	if (!drawEntities)
	{
		SDL_Rect rect = {0,0,1280,240};
		app->render->DrawTexture(img, posBackgroundLeft, 0, &rect);
		rect = { 0,240,1280,240 };
		app->render->DrawTexture(img, posBackgroundRight, 240, &rect);
		rect = { 0,480,1280,240 };
		app->render->DrawTexture(img, posBackgroundLeft, 480, &rect);
	}
	else
	{
		app->render->DrawTexture(img, 0, 0);

		if (god)
		{
			app->render->DrawRectangle({ WINDOW_W - 50, WINDOW_H - 50,  50, 50 }, orange.r, orange.g, orange.b, 255);
		}

		//Icon Enemy selected
		if (!win && !lose)
		{
			IconEnemySelected();
			DrawAllBattlesElements();
		}

		// Draw turn bar
		app->render->DrawRectangle({ 20, 30, 48, 64 * tam - 16 }, blue.r, blue.g, blue.b, 100);
		DrawTurnBar();
		app->render->DrawRectangle({ 20, 30, 48, 64 * tam - 16 }, orange.r, orange.g, orange.b, 255, false);

		if (win)
		{
			DrawSceneWin();
		}
		else if (lose)
		{
			DrawSceneLose();
		}
	}	

	if (flash)
	{
		app->render->DrawRectangle({ 0, 0, WINDOW_W, WINDOW_H }, 255, 255, 255, logoAlpha);
	}

	return true;
}

void SceneBattle::DrawAllBattlesElements()
{
	app->render->DrawTextBox(WINDOW_W - 175, 20, 150, 230, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	if (activeMenuMagic)
	{
		app->render->DrawTextBox(WINDOW_W - 450, 40, 250, 200, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	}
	// Draw Bar lives
	for (int i = 0; i < enemies.Count(); i++)
	{
		if (enemies.At(i)->data->entityData.state != DEAD)
		{
			posX = (int)enemies.At(i)->data->entityData.position.x + 2 * enemies.At(i)->data->entityData.pointsCollision[0].x + 2 * enemies.At(i)->data->entityData.centerPoint.x;
			posY = (int)enemies.At(i)->data->entityData.position.y - 30;
			rec = { posX - 45, posY, 90, 20 };
			live = rec;
			live.w = enemies.At(i)->data->stats.health * rec.w / enemies.At(i)->data->stats.maxHealth;

			sprintf_s(textLive, 8, "%d/%d", (int)enemies.At(i)->data->stats.health, enemies.At(i)->data->stats.maxHealth);
			DrawBarLives();
		}
	}

	for (int i = 0; i < partners.Count(); i++)
	{
		if (partners.At(i)->data->entityData.state != DEAD)
		{
			posX = (int)partners.At(i)->data->entityData.position.x + partners.At(i)->data->entityData.centerPoint.x;
			posY = (int)partners.At(i)->data->entityData.position.y - 30;
			rec = { posX - 45, posY, 90, 20 };
			live = rec;
			live.w = partners.At(i)->data->stats.health * rec.w / partners.At(i)->data->stats.maxHealth;

			sprintf_s(textLive, 8, "%d/%d", (int)partners.At(i)->data->stats.health, partners.At(i)->data->stats.maxHealth);
			DrawBarLives();
		}
	}
}

void SceneBattle::DrawSceneWin()
{
	int w = 0, h = 0;
	TTF_SizeText(app->sceneManager->titleFont, textVictory, &w, &h);
	app->fonts->BlitText(WINDOW_W / 2 - w / 2, 80, 1, textVictory, white);

	app->render->DrawTextBox(WINDOW_W / 2 - 400, WINDOW_H / 2 - 200, 800, 400, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	for (int i = 0; i < partners.Count(); i++)
	{
		posX = WINDOW_W / 2 - 400 + 40 + (i * 190);
		posY = WINDOW_H / 2 - 200 + 40;
		app->render->DrawRectangle({ posX, posY , 150, 150 }, black.r, black.g, black.b, 100);
		app->render->DrawRectangle({ posX, posY , 150, 150 }, 60, 43, 13, 255, false);

		// Draw Head Players
		if (partners.At(i)->data->entityData.type == KENZIE_)face = { 0,372,145,145 };
		else if (partners.At(i)->data->entityData.type == KEILER_)face = { 145,372,145,145 };
		else if (partners.At(i)->data->entityData.type == ISRRA_)face = { 0,517,145,145 };
		else if (partners.At(i)->data->entityData.type == BRENDA_)face = { 145,517,145,145 };
		app->render->DrawTexture(texPlayers, posX + 2, posY + 2, &face);

		// Draw Bar Lives
		rec = { posX, posY + 200, 150, 25 };
		live = rec;
		live.w = partners.At(i)->data->stats.health * rec.w / partners.At(i)->data->stats.maxHealth;
		sprintf_s(textLive, 8, "%d/%d", (int)partners.At(i)->data->stats.health, partners.At(i)->data->stats.maxHealth);
		DrawBarLives();

		// Draw Bar Experience
		rec.y += 30;
		live.y += 30;
		int exp = CalculateExp(partners.At(i)->data->entityData.level);
		live.w = partners.At(i)->data->stats.exp * rec.w / exp;
		sprintf_s(textExperience, 14, "%d/%d", (int)partners.At(i)->data->stats.exp, exp);
		DrawBarExperience();

		if (currentExp < totalExp)
		{
			currentExp += dt * 2;
			for (int i = 0; i < partners.Count(); i++)
			{
				if (partners.At(i)->data->entityData.state != DEAD)
				{
					partners.At(i)->data->stats.exp += dt * 2;
					if (partners.At(i)->data->stats.exp > exp)
					{
						partners.At(i)->data->stats.exp = 0;
						partners.At(i)->data->entityData.level++;
					}

				}
			}
		}

		// Draw Level
		rec.y -= 70;
		live.y -= 70;
		sprintf_s(textLevel, 8, "Lvl.%d", partners.At(i)->data->entityData.level);
		TTF_SizeText(app->sceneManager->guiFont, textLevel, &w, &h);
		app->fonts->BlitText(rec.x + rec.w / 2 - w / 2, rec.y + rec.h / 2 - h / 2, 0, textLevel, { 60, 43, 13 });
	}
	//Item draw
	for (int i = 0; i < 2; i++)
	{
		if (itemsId[i] != 0)
		{
			sprintf_s(textItems, 24, "%s x%d", app->player->itemManager->itemList.At(itemsId[i])->data->name.GetString(), itemMulti);
			app->fonts->BlitText(281, rec.y + rec.h / 2 - h / 2 + 100 + i * 25, 0, textItems, { 60, 43, 13 });
		}
		if (itemMulti == 2)
		{
			break;
		}
	}
}

void SceneBattle::GenerateItems()
{
	int fnumb = rand() % 3;
	for (int i = 0; i < fnumb; i++)
	{
		int numb = rand() % 5 + 1;
		app->player->itemManager->AddItem(numb);
		itemsId[i] = numb;
	}
	for (int i = 0; i < fnumb-1; i++)
	{
		if (itemsId[i] == itemsId[i+1])
		{
			itemMulti++;
		}
	}
}

void SceneBattle::DrawSceneLose()
{
	app->render->DrawRectangle({ 0,0, WINDOW_W, WINDOW_H }, 0, 0, 0, 200);
	int w = 0, h = 0;
	sprintf_s(textLoose, 28, "YOU LOOSE");
	TTF_SizeText(app->sceneManager->titleFont, textLoose, &w, &h);
	app->fonts->BlitText(WINDOW_W / 2 - w / 2, 180, 1, textLoose, red, dt*35);
	app->render->DrawTextBox(WINDOW_W / 2 - 119, WINDOW_H / 2 + 50, 238, 119, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	if (btnContinue->state == GuiControlState::FOCUSED)
	{
		sprintf_s(textLoose, 28, "Continue from the last save");
		TTF_SizeText(app->sceneManager->guiFont, textLoose, &w, &h);
		app->fonts->BlitText(WINDOW_W / 2 - w / 2, 600, 0, textLoose, white);
	}
	else if (btnExit->state == GuiControlState::FOCUSED)
	{
		sprintf_s(textLoose, 28, "Return to title screen");
		TTF_SizeText(app->sceneManager->guiFont, textLoose, &w, &h);
		app->fonts->BlitText(WINDOW_W / 2 - w / 2, 600, 0, textLoose, white);
	}
}

void SceneBattle::IconEnemySelected()
{
	if (faseAction == SELECT_ENEMY) {
		int posCursorX;
		int posCursorY;
		int posSelectX;
		int posSelectY;

		if (magicInUse == nullptr || magicInUse->type == 0)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || (app->input->pads[0].left && !missClick)) {
				missClick = true;
				enemySelected--;
				if (enemySelected < 0)
				{
					enemySelected = enemies.Count() - 1;
				}
				if (enemies.At(enemySelected)->data->stats.health <= 0) {
					for (int i = enemySelected; i >= 0; i--) {
						if (enemies.At(i)->data->stats.health > 0) {
							enemySelected = i;
							break;
						}
					}
					if (enemies.At(enemySelected)->data->stats.health <= 0) {
						for (int i = enemies.Count() - 1; i >= 0; i--) {
							if (enemies.At(i)->data->stats.health > 0) {
								enemySelected = i;
								break;
							}
						}
					}
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || (app->input->pads[0].right && !missClick)) {
				missClick = true;
				enemySelected++;
				if (enemySelected >= enemies.Count())
				{
					enemySelected = 0;
				}
				if (enemies.At(enemySelected)->data->stats.health <= 0) {
					for (int i = enemySelected; i < enemies.Count(); i++) {
						if (enemies.At(i)->data->stats.health > 0) {
							enemySelected = i;
							break;
						}
					}
					if (enemies.At(enemySelected)->data->stats.health <= 0) {
						for (int i = 0; i < enemies.Count(); i++) {
							if (enemies.At(i)->data->stats.health > 0) {
								enemySelected = i;
								break;
							}
						}
					}
				}
			}

			posCursorX = (int)enemies.At(enemySelected)->data->entityData.position.x + 50;
			posCursorY = (int)enemies.At(enemySelected)->data->entityData.position.y - 70;
			
			posSelectX = (int)enemies.At(enemySelected)->data->entityData.position.x + 13;
			posSelectY = (int)enemies.At(enemySelected)->data->entityData.position.y + 15;

		}
		else if (magicInUse->type == 1)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || (app->input->pads[0].left && !missClick)) {
				missClick = true;
				enemySelected--;
				if (enemySelected < 0)
				{
					enemySelected = partners.Count() - 1;
				}
				if (partners.At(enemySelected)->data->stats.health <= 0) {
					for (int i = enemySelected; i >= 0; i--) {
						if (partners.At(i)->data->stats.health > 0) {
							enemySelected = i;
							break;
						}
					}
					if (partners.At(enemySelected)->data->stats.health <= 0) {
						for (int i = partners.Count() - 1; i >= 0; i--) {
							if (partners.At(i)->data->stats.health > 0) {
								enemySelected = i;
								break;
							}
						}
					}
				}
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || (app->input->pads[0].right && !missClick)) {
				missClick = true;
				enemySelected++;
				if (enemySelected >= partners.Count())
				{
					enemySelected = 0;
				}
				if (partners.At(enemySelected)->data->stats.health <= 0) {
					for (int i = enemySelected; i < partners.Count(); i++) {
						if (partners.At(i)->data->stats.health > 0) {
							enemySelected = i;
							break;
						}
					}
					if (partners.At(enemySelected)->data->stats.health <= 0) {
						for (int i = 0; i < partners.Count(); i++) {
							if (partners.At(i)->data->stats.health > 0) {
								enemySelected = i;
								break;
							}
						}
					}
				}
			}

			posCursorX = (int)partners.At(enemySelected)->data->entityData.position.x - 10;
			posCursorY = (int)partners.At(enemySelected)->data->entityData.position.y - 70;

			posSelectX = (int)partners.At(enemySelected)->data->entityData.position.x - 20;
			posSelectY = (int)partners.At(enemySelected)->data->entityData.position.y + 40;
		
		}
		//app->render->DrawRectangle({ posCursorX, posCursorY ,20,20 }, red.r, red.g, red.b, 255);
		app->render->DrawTexture(enemySelect, posSelectX, posSelectY, NULL, 3);
		app->render->DrawTexture(app->guiManager->handCursor, posCursorX, posCursorY, &app->guiManager->handAnim->GetCurrentFrame(), 1, 0, 90);
	}
}

void SceneBattle::UpdateAnimationEnemies()
{
	for (int i = 0; i < enemies.Count(); i++)
	{
		TypeEntity eType = enemies.At(i)->data->entityData.type;
		if (enemies.At(i)->data->entityData.currentAnimation->HasFinished() == true &&
			((enemies.At(i)->data->entityData.currentAnimation == animationsEnemies.At(2)->data ||
				enemies.At(i)->data->entityData.currentAnimation == animationsEnemies.At(5)->data) ||
				enemies.At(i)->data->entityData.currentAnimation == animationsEnemies.At(8)->data))
		{
			switch (eType)
			{
			case BANDIT:
				animationsEnemies.At(2)->data->Reset();
				enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(0)->data;
				break;
			case FIGHTER:
				animationsEnemies.At(5)->data->Reset();
				enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(3)->data;
				break;
			case SAPLING:
				animationsEnemies.At(8)->data->Reset();
				enemies.At(i)->data->entityData.currentAnimation = animationsEnemies.At(6)->data;
				break;
			default:
				break;
			}
		}
	}
}

void SceneBattle::BattleSystem()
{
	switch (faseAction)
	{
	case SELECT_ACTION:

		if (turnSort[turn].entityData.type < KENZIE_)
		{
			faseAction = ENEMY_ATTACK;
		}
		else
		{
			int theHealth = 1;
			for (int i = 0; i < partners.Count(); i++) {
				if (turnSort[turn].entityData.positionInitial == partners.At(i)->data->entityData.positionInitial)
					theHealth = partners.At(i)->data->stats.health;
			}
			if (theHealth <= 0)
			{
				faseAction = END_ACTION;
			}
		}
		break;

	case SELECT_ENEMY:
	{
		btnAttack->state = GuiControlState::DISABLED;
		btnMagic->state = GuiControlState::DISABLED;
		btnDefense->state = GuiControlState::DISABLED;
		btnExit->state = GuiControlState::DISABLED;

		if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN || app->input->pads[0].b || app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT) {
			btnAttack->state = GuiControlState::NORMAL;
			btnMagic->state = GuiControlState::NORMAL;
			btnDefense->state = GuiControlState::NORMAL;
			btnExit->state = GuiControlState::NORMAL;

			app->audio->PlayFx(fxCancelButton);

			faseAction = SELECT_ACTION;
		}

		if (magicInUse == nullptr || magicInUse->type == 0)
		{
			//Select enemy with mouse 
			int mouseX, mouseY;
			app->input->GetMousePosition(mouseX, mouseY);
			bool click;
			for (int i = 0; i < enemies.Count(); i++) {
				if ((mouseX > (enemies.At(i)->data->entityData.position.x + 20) && (mouseX < (enemies.At(i)->data->entityData.position.x + 100)) &&
					(mouseY > (enemies.At(i)->data->entityData.position.y - 20)) && (mouseY < (enemies.At(i)->data->entityData.position.y + 80)))) {
					enemySelected = i;
					if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
						faseAction = DO_ACITON;
					}
				}
			}
		}
		else if (magicInUse->type == 1) {
			//app->render->DrawRectangle({ (int)partners.At(i)->data->entityData.position.x, (int)partners.At(i)->data->entityData.position.y ,60, 90 }, red.r, red.g, red.b, 255);

			//Select ally with mouse 
			int mouseX, mouseY;
			app->input->GetMousePosition(mouseX, mouseY);
			bool click;
			for (int i = 0; i < partners.Count(); i++) {
				if ((mouseX > (partners.At(i)->data->entityData.position.x) && (mouseX < (partners.At(i)->data->entityData.position.x + 60)) &&
					(mouseY > (partners.At(i)->data->entityData.position.y)) && (mouseY < (partners.At(i)->data->entityData.position.y + 90)))) {
					enemySelected = i;
					if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
						faseAction = DO_ACITON;
					}
				}
			}
		}


		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN || (app->input->pads[0].a && !missClick)) {
			faseAction = DO_ACITON;
		}
	}
	break;

	case DO_ACITON:
	{
		if (!hit)
		{

			hit = true;
			if (magicInUse == nullptr)
			{
				float attack = turnSort[turn].stats.attack;
				newHealth = enemies.At(enemySelected)->data->stats.health - attack;
				app->audio->PlayFx(fxAttack);
			}
			else
			{
				switch (magicInUse->type)
				{
				case 0:
					newHealth = enemies.At(enemySelected)->data->stats.health - magicInUse->damage;
					break;
				case 1:
					newHealth = partners.At(enemySelected)->data->stats.health + magicInUse->damage;
					break;
				default:
					break;
				}

				switch (magicInUse->element)
				{
				case 1:
					app->audio->PlayFx(fxFireball);
					break;
				case 2:
					app->audio->PlayFx(fxHealing);
					break;
				case 3:
					app->audio->PlayFx(fxShadow);
					break;
				default:
					break;

				}

				activeMenuMagic = false;
				btnAttack->state = GuiControlState::DISABLED;
				btnMagic->state = GuiControlState::DISABLED;
				btnDefense->state = GuiControlState::DISABLED;
				btnExit->state = GuiControlState::DISABLED;
			}
		}
		else
		{
			if (magicInUse == nullptr) {
				if (newHealth <= 0)newHealth = 0;
				//Progres damage
				if (enemies.At(enemySelected)->data->stats.health > newHealth) {
					TypeEntity eType = enemies.At(enemySelected)->data->entityData.type;
					if (eType == BANDIT || eType == FIGHTER || eType == SAPLING)
					{
						switch (eType)
						{
						case BANDIT:
							enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(1)->data;
							break;
						case FIGHTER:
							enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(4)->data;
							break;
						case SAPLING:
							enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(7)->data;
							break;
						default:
							break;
						}
					}
					enemies.At(enemySelected)->data->stats.health -= dt * reduceLieveVelocity;
				}
				else {
					TypeEntity eType = enemies.At(enemySelected)->data->entityData.type;
					if (eType == BANDIT || eType == FIGHTER || eType == SAPLING)
					{
						switch (eType)
						{
						case BANDIT:
							enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(0)->data;
							break;
						case FIGHTER:
							enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(3)->data;
							break;
						case SAPLING:
							enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(6)->data;
							break;
						default:
							break;
						}
					}
					enemies.At(enemySelected)->data->stats.health = newHealth;
					if (enemies.At(enemySelected)->data->stats.health < 1)
					{
						enemies.At(enemySelected)->data->stats.health = 0;
						enemies.At(enemySelected)->data->entityData.state = DEAD;

						switch (enemies.At(enemySelected)->data->entityData.type)
						{
						case BANDIT:
							app->audio->PlayFx(fxBanditDies);
							break;
						case FIGHTER:
							app->audio->PlayFx(fxFighterDies);
							break;
						case SAPLING:
							app->audio->PlayFx(fxSaplingDies);
							break;
						default:
							break;
						}

						assigneDone = false;

						for (int i = 0; i < tam; i++) {
							if (turnSort[i].entityData.positionInitial == enemies.At(enemySelected)->data->entityData.positionInitial) {
								indexTurnBar = i;
								break;
							}
						}
					}
					hit = false;
					faseAction = END_ACTION;
				}
			}
			else
			{
				switch (magicInUse->type)
				{
				case 0:
				{
					if (newHealth <= 0)newHealth = 0;
					//Progres damage
					if (enemies.At(enemySelected)->data->stats.health > newHealth) {
						TypeEntity eType = enemies.At(enemySelected)->data->entityData.type;
						if (eType == BANDIT || eType == FIGHTER || eType == SAPLING)
						{
							switch (eType)
							{
							case BANDIT:
								enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(1)->data;
								break;
							case FIGHTER:
								enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(4)->data;
								break;
							case SAPLING:
								enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(7)->data;
								break;
							default:
								break;
							}
						}
						enemies.At(enemySelected)->data->stats.health -= dt * reduceLieveVelocity;
					}
					else {
						TypeEntity eType = enemies.At(enemySelected)->data->entityData.type;
						if (eType == BANDIT || eType == FIGHTER || eType == SAPLING)
						{
							switch (eType)
							{
							case BANDIT:
								enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(0)->data;
								break;
							case FIGHTER:
								enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(3)->data;
								break;
							case SAPLING:
								enemies.At(enemySelected)->data->entityData.currentAnimation = animationsEnemies.At(6)->data;
								break;
							default:
								break;
							}
						}
						enemies.At(enemySelected)->data->stats.health = newHealth;
						if (enemies.At(enemySelected)->data->stats.health < 1)
						{
							enemies.At(enemySelected)->data->stats.health = 0;
							enemies.At(enemySelected)->data->entityData.state = DEAD;

							switch (enemies.At(enemySelected)->data->entityData.type)
							{
							case BANDIT:
								app->audio->PlayFx(fxBanditDies);
								break;
							case FIGHTER:
								app->audio->PlayFx(fxFighterDies);
								break;
							case SAPLING:
								app->audio->PlayFx(fxSaplingDies);
								break;
							default:
								break;
							}
							assigneDone = false;

							for (int i = 0; i < tam; i++) {
								if (turnSort[i].entityData.positionInitial == enemies.At(enemySelected)->data->entityData.positionInitial)
								{
									indexTurnBar = i;
									break;
								}
							}
						}
						hit = false;
						magicInUse = nullptr;
						faseAction = END_ACTION;
					}
				}
				break;
				case 1:
				{
					if (newHealth >= partners.At(enemySelected)->data->stats.maxHealth) newHealth = partners.At(enemySelected)->data->stats.maxHealth;
					//Progres damage
					if (partners.At(enemySelected)->data->stats.health < newHealth) {
						partners.At(enemySelected)->data->stats.health += dt * reduceLieveVelocity;
					}
					else {
						partners.At(enemySelected)->data->stats.health = newHealth;
						hit = false;
						magicInUse = nullptr;
						faseAction = END_ACTION;
					}
				}
				break;
				default:
					break;
				}

			}
		}
	}
	break;

	case END_ACTION:
		moveBarTurn = true;
		faseAction = SELECT_ACTION;
		break;

	case ENEMY_ATTACK:
		btnAttack->state = GuiControlState::DISABLED;
		btnMagic->state = GuiControlState::DISABLED;
		btnDefense->state = GuiControlState::DISABLED;
		btnExit->state = GuiControlState::DISABLED;

		if (!moveBarTurn)
		{
			if (!hit)
			{
				app->audio->PlayFx(fxAttack);
				ally = 0;
				hit = true;
				// Find heald of actual enemy
				int theHealth = 1;
				int enemyIndex = 0;
				for (int i = 0; i < enemies.Count(); i++) {
					if (turnSort[turn].entityData.positionInitial == enemies.At(i)->data->entityData.positionInitial)
					{
						theHealth = enemies.At(i)->data->stats.health;
						enemyIndex = i;
					}
				}

				TypeEntity eType = enemies.At(enemyIndex)->data->entityData.type;
				if (eType == BANDIT || eType == FIGHTER || eType == SAPLING)
				{
					switch (eType)
					{
					case BANDIT:
						enemies.At(enemyIndex)->data->entityData.currentAnimation = animationsEnemies.At(2)->data;
						break;
					case FIGHTER:
						enemies.At(enemyIndex)->data->entityData.currentAnimation = animationsEnemies.At(5)->data;
						break;
					case SAPLING:
						enemies.At(enemyIndex)->data->entityData.currentAnimation = animationsEnemies.At(8)->data;
						break;
					default:
						break;
					}
				}

				if (theHealth > 0) {
					if (magicInUse == nullptr)
					{
						ally = (rand() % partners.Count());
						if (partners.At(ally)->data->stats.health <= 0) {
							for (int i = ally; i >= 0; i--) {
								if (partners.At(i)->data->stats.health > 0) {
									ally = i;
									break;
								}
							}
							if (partners.At(ally)->data->stats.health <= 0) {
								for (int i = partners.Count() - 1; i >= 0; i--) {
									if (partners.At(i)->data->stats.health > 0) {
										ally = i;
										break;
									}
								}
							}
						}

						if (partners.At(ally)->data->stats.defenseOn)
						{
							newHealth = partners.At(ally)->data->stats.health - turnSort[turn].stats.attack / 2;
							partners.At(ally)->data->stats.defenseOn = false;
						}
						else newHealth = partners.At(ally)->data->stats.health - turnSort[turn].stats.attack;

						if (god) {
							newHealth = partners.At(ally)->data->stats.health;
						}
					}
					else
					{
						ally = (rand() % partners.Count());
						if (partners.At(ally)->data->stats.health <= 0) {
							for (int i = ally; i > 0; i--) {
								if (partners.At(i)->data->stats.health > 0) {
									ally = i;
									break;
								}
							}
							if (partners.At(ally)->data->stats.health <= 0) {
								for (int i = partners.Count() - 1; i > 0; i--) {
									if (partners.At(i)->data->stats.health > 0) {
										ally = i;
										break;
									}
								}
							}
						}

						newHealth = partners.At(ally)->data->stats.health - magicInUse->damage;
						if (partners.At(ally)->data->stats.defenseOn)
						{
							newHealth = partners.At(ally)->data->stats.health - magicInUse->damage / 2;
							partners.At(ally)->data->stats.defenseOn = false;
						}
						else newHealth = partners.At(ally)->data->stats.health - magicInUse->damage;

						if (god) {
							newHealth = partners.At(ally)->data->stats.health;
						}
					}
				}
			}
			else
			{
				if (newHealth <= 0)newHealth = 0;
				//Progres damage
				if (partners.At(ally)->data->stats.health > newHealth) {
					//partners.At(ally)->data->entityData.state = HIT;
					//partners.At(ally)->data->entityData.currentAnimation = animationsHitPlayer.At(ally)->data;
					/*if (enemies.At)
					{

					}*/
					TypeEntity pType = partners.At(ally)->data->entityData.type;
					if (pType == KENZIE_ || pType == KEILER_ || pType == ISRRA_ || pType == BRENDA_)
					{
						switch (pType)
						{
						case KENZIE_:
							partners.At(ally)->data->entityData.currentAnimation = animationsHitPlayer.At(0)->data;
							break;
						case KEILER_:
							partners.At(ally)->data->entityData.currentAnimation = animationsHitPlayer.At(1)->data;
							break;
						case ISRRA_:
							partners.At(ally)->data->entityData.currentAnimation = animationsHitPlayer.At(2)->data;
							break;
						case BRENDA_:
							partners.At(ally)->data->entityData.currentAnimation = animationsHitPlayer.At(3)->data;
							break;

						default:
							break;
						}
					}
					partners.At(ally)->data->stats.health -= dt * reduceLieveVelocity;
				}
				else {
					TypeEntity pType = partners.At(ally)->data->entityData.type;
					if (pType == KENZIE_ || pType == KEILER_ || pType == ISRRA_ || pType == BRENDA_)
					{
						switch (pType)
						{
						case KENZIE_:
							partners.At(ally)->data->entityData.currentAnimation = animationsPlayer.At(0)->data;
							break;
						case KEILER_:
							partners.At(ally)->data->entityData.currentAnimation = animationsPlayer.At(1)->data;
							break;
						case ISRRA_:
							partners.At(ally)->data->entityData.currentAnimation = animationsPlayer.At(2)->data;
							break;
						case BRENDA_:
							partners.At(ally)->data->entityData.currentAnimation = animationsPlayer.At(3)->data;
							break;

						default:
							break;
						}
					}
					partners.At(ally)->data->stats.health = newHealth;
					if (partners.At(ally)->data->stats.health < 1) {
						partners.At(ally)->data->stats.health = 0;
						partners.At(ally)->data->entityData.state = DEAD;
						app->audio->PlayFx(fxAllyDies);
						assigneDone = false;
						for (int i = 0; i < tam; i++) {
							if (turnSort[i].entityData.positionInitial == partners.At(ally)->data->entityData.positionInitial)
							{
								indexTurnBar = i;
								break;
							}
						}
					}
					hit = false;
					faseAction = END_ACTION;
				}
			}
		}
		break;
	default:
		break;
	}
}

void SceneBattle::CheckWinLose()
{
	if (!win && !lose)
	{
		for (int i = 0; i < enemies.Count(); i++)
		{
			if (enemies.At(i)->data->entityData.state != DEAD) break;
			if (i == enemies.Count() - 1)
			{
				win = true;
				app->audio->PlayFx(fxWin);
				AbleDisableButtons();
				GenerateItems();
				app->sceneManager->SetWinBattle(true);
				app->audio->PlayMusic("Assets/Audio/Music/win_music.ogg", 0);
			}
		}
		// Lose Condition
		for (int i = 0; i < partners.Count(); i++)
		{
			if (partners.At(i)->data->entityData.state != DEAD) break;
			if (i == partners.Count() - 1)
			{
				btnContinue->bounds.y -= 70;
				btnContinue->bounds.x += 5;
				btnExit->bounds = btnContinue->bounds;
				btnExit->bounds.y += 40;
				btnExit->bounds.x = WINDOW_W / 2 - 25;
				AbleDisableButtons();
				lose = true;
				app->audio->PlayFx(fxLose);
				app->audio->PlayMusic("Assets/Audio/Music/lose_music.ogg", 0);
				btnExit->active = true;
				btnExit->state = GuiControlState::NORMAL;
				app->sceneManager->SetLoseBattle(true);
				for (int i = 0; i < enemies.Count(); i++)
				{
					enemies.At(i)->data->entityData.state = DEAD;
				}
			}
		}
	}
}

void SceneBattle::AbleDisableButtons()
{
	btnContinue->active = true;
	btnAttack->active = false;
	btnDefense->active = false;
	btnMagic->active = false;
	btnExit->active = false;
}
void SceneBattle::AbleButtons()
{
	btnAttack->active = true;
	btnDefense->active = true;
	btnMagic->active = true;
	btnExit->active = true;
}

void SceneBattle::DrawBarLives()
{
	if (win) tex = { 0, 32, rec.w, rec.h + 7 };
	else tex = { 160, 32, rec.w, rec.h + 7 };
	app->render->DrawTexture(app->guiManager->uiAtlas, rec.x, rec.y - 7, &tex);

	if (live.w > rec.w) live.w = rec.w;
	if (live.w < 0)live.w = 0;
	if (live.w > rec.w / 2)
	{
		if (win) tex = { 0, 0, live.w, rec.h + 7 };
		else tex = { 160, 0, live.w, rec.h + 7 };
		app->render->DrawTexture(app->guiManager->uiAtlas, live.x, live.y - 7, &tex);
	}
	else if (live.w < rec.w / 4)
	{
		if (win) tex = { 0, 224, live.w, rec.h + 7 };
		else tex = { 160, 224, live.w, rec.h + 7 };
		app->render->DrawTexture(app->guiManager->uiAtlas, live.x, live.y - 7, &tex);
	}
	else if (live.w <= rec.w / 2)
	{
		if (win) tex = { 0, 192, live.w, rec.h + 7 };
		else tex = { 160, 192, live.w, rec.h + 7 };
		app->render->DrawTexture(app->guiManager->uiAtlas, live.x, live.y - 7, &tex);
	}

	int w = 0,  h = 0;
	TTF_SizeText(app->sceneManager->guiFont, textLive, &w, &h);
	app->fonts->BlitText(rec.x + rec.w / 2 - w / 2, rec.y + rec.h / 2 - h / 2 - 7, 0, textLive, { 60, 43, 13 });
}

void SceneBattle::DrawBarExperience()
{
	tex = { 0, 32, rec.w, rec.h + 7 };
	app->render->DrawTexture(app->guiManager->uiAtlas, rec.x, rec.y - 7, &tex);

	if (live.w > rec.w) live.w = rec.w;
	if (live.w < 0)live.w = 0;
	tex = { 0, 256, live.w, rec.h + 7 };
	app->render->DrawTexture(app->guiManager->uiAtlas, live.x, live.y - 7, &tex);

	int w = 0, h = 0;
	TTF_SizeText(app->sceneManager->guiFont, textExperience, &w, &h);
	app->fonts->BlitText(rec.x + rec.w / 2 - w / 2, rec.y + rec.h / 2 - h / 2 - 7, 0, textExperience, { 60, 43, 13 });
}

void SceneBattle::DrawTurnBar()
{
	if (moveBarTurn) offset -= dt * 60, numArrows = 0;
	else offset = 38, numArrows = 1;
	for (int i = 0; i < tam; i++)
	{
		switch (turnSort[i].entityData.type)
		{
		case KENZIE_:
			if (i == 0) {
				spritesBarTurn.At(0)->data->Update();
				face = spritesBarTurn.At(0)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(0)->data->frames[0];
			app->render->DrawTexture(texPlayers, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(0)->data->frames[0];
				app->render->DrawTexture(texPlayers, 28, 64 * tam + offset, &face);
			}
			break;
		case KEILER_:
			if (i == 0) {
				spritesBarTurn.At(1)->data->Update();
				face = spritesBarTurn.At(1)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(1)->data->frames[0];
			app->render->DrawTexture(texPlayers, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(1)->data->frames[0];
				app->render->DrawTexture(texPlayers, 28, 64 * tam + offset, &face);
			}
			break;
		case ISRRA_:
			if (i == 0) {
				spritesBarTurn.At(2)->data->Update();
				face = spritesBarTurn.At(2)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(2)->data->frames[0];
			app->render->DrawTexture(texPlayers, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(2)->data->frames[0];
				app->render->DrawTexture(texPlayers, 28, 64 * tam + offset, &face);
			}
			break;
		case BRENDA_:
			if (i == 0) {
				spritesBarTurn.At(3)->data->Update();
				face = spritesBarTurn.At(3)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(3)->data->frames[0];
			app->render->DrawTexture(texPlayers, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(3)->data->frames[0];
				app->render->DrawTexture(texPlayers, 28, 64 * tam + offset, &face);
			}
			break;

		case BANDIT:
			if (i == 0) {
				spritesBarTurn.At(4)->data->Update();
				face = spritesBarTurn.At(4)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(4)->data->frames[1];
			app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(4)->data->frames[0];
				app->render->DrawTexture(texEnemies, 28, 64 * tam + offset, &face);
			}
			break;
		case FIGHTER:
			if (i == 0) {
				spritesBarTurn.At(5)->data->Update();
				face = spritesBarTurn.At(5)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(5)->data->frames[1];
			app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(5)->data->frames[0];
				app->render->DrawTexture(texEnemies, 28, 64 * tam + offset, &face);
			}
			break;
		case SAPLING:
			if (i == 0) {
				spritesBarTurn.At(6)->data->Update();
				face = spritesBarTurn.At(6)->data->GetCurrentFrame();
			}
			else face = spritesBarTurn.At(6)->data->frames[1];
			app->render->DrawTexture(texEnemies, 28, 64 * i + offset, &face);
			if (moveBarTurn && i == 0)
			{
				face = spritesBarTurn.At(6)->data->frames[0];
				app->render->DrawTexture(texEnemies, 28, 64 * tam + offset, &face);
			}
			break;
		default:
			break;
		}
		face = { 416,0, 32,14 };
		if (i < tam - numArrows)app->render->DrawTexture(texPlayers, 28, 64 * i + 40 + offset, &face);
	}
	if (moveBarTurn)
	{
		face = { 20,0,48,30 };
		app->render->DrawTexture(img, 20, 0, &face);
		face = { 20,64 * tam - 16 + 30,48,64 };
		app->render->DrawTexture(img, 20, 64 * tam - 16 + 30, &face);
		btnAttack->state = GuiControlState::DISABLED;
		btnMagic->state = GuiControlState::DISABLED;
		btnDefense->state = GuiControlState::DISABLED;
		btnExit->state = GuiControlState::DISABLED;
	}

	if (offset < -64 + 38) {
		moveBarTurn = false, DisplaceToLeft(), faseAction = SELECT_ACTION;
		btnAttack->state = GuiControlState::NORMAL;
		btnMagic->state = GuiControlState::NORMAL;
		btnDefense->state = GuiControlState::NORMAL;
		btnExit->state = GuiControlState::NORMAL;
	}
}

bool SceneBattle::CleanUp()
{
	bool ret = true;
	app->tex->UnLoad(img);
	app->tex->UnLoad(texEnemies);
	app->tex->UnLoad(enemySelect);

	app->audio->Unload1Fx(fxWin);
	app->audio->Unload1Fx(fxLose);
	app->audio->Unload1Fx(fxAttack);
	app->audio->Unload1Fx(fxDefense);
	app->audio->Unload1Fx(fxExit);
	app->audio->Unload1Fx(fxFireball);
	app->audio->Unload1Fx(fxShadow);
	app->audio->Unload1Fx(fxHealing);
	app->audio->Unload1Fx(fxBanditDies);
	app->audio->Unload1Fx(fxFighterDies);
	app->audio->Unload1Fx(fxSaplingDies);
	app->audio->Unload1Fx(fxAllyDies);
	app->audio->Unload1Fx(fxCancelButton);

	spritesBarTurn.Clear();
	animationsPlayer.Clear();
	animationsEnemies.Clear();

	delete[] turnSort;
	turnSort = nullptr;

	app->entityManager->ClearList(ret);
	enemies = app->entityManager->entities;
	partners = app->entityManager->partners;

	RELEASE(menuMagic);
	magicInUse = nullptr;
	texPlayers = nullptr;

	app->sceneManager->SetWinBattle(false);

	return ret;
}

void SceneBattle::SpeedAnimationCheck(float dt)
{
	for (int i = 0; i < spritesBarTurn.Count(); i++)
	{
		spritesBarTurn.At(i)->data->speed = dt * 6;
	}
	for (int i = 0; i < animationsPlayer.Count(); i++)
	{
		animationsPlayer.At(i)->data->speed = dt * 6;
	}
	for (int i = 0; i < animationsEnemies.Count(); i++)
	{
		animationsEnemies.At(i)->data->speed = dt * 6;
	}
}

void SceneBattle::AssignEntities()
{
	enemies = app->entityManager->entities;
	partners = app->entityManager->partners;
	if (initCombat)
	{
		for (int i = 0; i < enemies.Count(); i++)
		{
			turnSort[i].entityData = enemies.At(i)->data->entityData;
			turnSort[i].stats = enemies.At(i)->data->stats;
		}
		int j = 0;
		tam = enemies.Count() + partners.Count();
		for (int i = enemies.Count(); i < tam; i++)
		{
			turnSort[i].entityData = partners.At(j)->data->entityData;
			turnSort[i].stats = partners.At(j)->data->stats;
			j++;
		}
	
		BubbleSort();
		initCombat = false;
	}
	if (indexTurnBar != -1)
	{
		for (int i = indexTurnBar; i < tam - 1; i++)
		{
			turnSort[i] = turnSort[i + 1];
		}
		tam--;
		indexTurnBar = -1;
	}

	assigneDone = true;
}

bool SceneBattle::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		//ATTACK
		if (control->id == 20)
		{
			magicInUse = nullptr;
			faseAction = SELECT_ENEMY;
			for (int i = 0; i < enemies.Count(); i++)
			{
				if (enemies.At(i)->data->stats.health > 0) {
					enemySelected = i;
					break;
				}
			}
			missClick = true;
		}
		//MAGIC
		else if (control->id == 21)
		{
			activeMenuMagic = true;

			menuMagic->SetIdTurn(turnSort[turn].entityData.type);

			menuMagic->AbleDisableMagic();
			menuMagic->MovePosition();

			btnAttack->state = GuiControlState::DISABLED;
			btnMagic->state = GuiControlState::DISABLED;
			btnDefense->state = GuiControlState::DISABLED;
			btnExit->state = GuiControlState::DISABLED;

		}
		//DEFENSE
		else if (control->id == 22)
		{
			app->audio->PlayFx(fxDefense);
			moveBarTurn = true;
			for (int i = 0; i < partners.Count(); i++)
			{
				if (partners.At(i)->data->entityData.type == turnSort[turn].entityData.type)
				{
					partners.At(i)->data->stats.defenseOn = true;
				}
			}
		}
		//EXIT
		else if (control->id == 23)
		{
			if (lose)
			{
				SaveStateLose();
				TransitionToScene(SceneType::INTRO);
			}
			else
			{
				ContinueGame();
				isContinue = true;
				app->audio->PlayFx(fxExit);
				TransitionToScene(SceneType::LEVEL3);
			}			
		}
		// Continue
		else if (control->id == 24)
		{
			if (win)
			{
				ContinueGame();
			}
			else if (lose)
			{
				SaveStateLose();
			}

			isContinue = true;
			TransitionToScene(SceneType::LEVEL3);
		}

		//--MAGIC MENU--
		else if (control->id == 30)
		{
			activeMenuMagic = false;
			btnAttack->state = GuiControlState::NORMAL;
			btnMagic->state = GuiControlState::NORMAL;
			btnDefense->state = GuiControlState::NORMAL;
			btnExit->state = GuiControlState::NORMAL;
			menuMagic->AbleDisableMagic();

		}
		//Magic1
		else if (control->id == 31)
		{
			magicInUse = menuMagic->GetMagic(1);
			UseAMagic();
		}
		//Magic2
		else if (control->id == 32)
		{
			magicInUse = menuMagic->GetMagic(2);
			UseAMagic();
		}
		//Magic3
		else if (control->id == 33)
		{
			magicInUse = menuMagic->GetMagic(3);
			UseAMagic();
		}
		//Magic4
		else if (control->id == 34)
		{
			magicInUse = menuMagic->GetMagic(4);
			UseAMagic();
		}
	}
	default: break;
	}
	return true;
}

void SceneBattle::UseAMagic()
{
	missClick = true;
	if (magicInUse == nullptr) {
		// not exists magic
	}
	else {
		// Use magicInUse
		menuMagic->AbleDisableMagic();
		switch (magicInUse->type)
		{
		case 0:
			for (int i = 0; i < enemies.Count(); i++)
			{
				if (enemies.At(i)->data->stats.health > 0) {
					enemySelected = i;
					break;
				}
			}
			break;
		case 1:
			for (int i = 0; i < partners.Count(); i++)
			{
				if (partners.At(i)->data->stats.health > 0) {
					enemySelected = i;
					break;
				}
			}
			break;
		default:
			break;
		}

		activeMenuMagic = false;
		faseAction = SELECT_ENEMY;
	}
}

int SceneBattle::CalculateExp(int level)
{
	return (0.04 * (level * level * level) + 0.8 * (level * level) + 2 * level) * 3.5;
}
void SceneBattle::SaveStateLose()
{
	iPoint playerPos;
	app->player->playerData.health = 2 * app->player->playerData.level + 6;
	app->player->playerData.position = { 570, 270 };
	playerPos = app->player->playerData.position;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)app->player->GetPartners()[i].health = 4 * app->player->GetPartners()[i].level + 10;
		else if (i == 1)app->player->GetPartners()[i].health = 2.5 * app->player->GetPartners()[i].level + 7.5;
		else if (i == 2)app->player->GetPartners()[i].health = 3.5 * app->player->GetPartners()[i].level + 9.5;
	}
	for (int i = 0; i < app->player->GetNumPartners(); i++)
	{
		app->player->GetPartners()[i].position.x = playerPos.x - (40 * i) - 40;
		app->player->GetPartners()[i].position.y = playerPos.y;
	}
}
void SceneBattle::SaveState(TypeEntity pType, int i)
{
	for (int j = 0; j < partners.Count(); j++)
	{
		if (partners.At(j)->data->entityData.type == pType)
		{
			app->player->GetPartners()[i].level = partners.At(j)->data->entityData.level;
			app->player->GetPartners()[i].exp = partners.At(j)->data->stats.exp;
			app->player->GetPartners()[i].health = partners.At(j)->data->stats.health;
			app->player->GetPartners()[i].mana = partners.At(j)->data->stats.mana;
			break;
		}
		else app->player->GetPartners()[i].health = 0;
	}
}

void SceneBattle::ContinueGame()
{
	for (int i = 0; i < partners.Count(); i++)
	{
		if (partners.At(i)->data->entityData.type == KENZIE_)
		{
			app->player->playerData.level = partners.At(i)->data->entityData.level;
			app->player->playerData.exp = partners.At(i)->data->stats.exp;
			app->player->playerData.health = partners.At(i)->data->stats.health;
			app->player->playerData.mana = partners.At(i)->data->stats.mana;
			break;
		}
		else app->player->playerData.health = 0;
	}

	for (int i = 0; i < app->player->GetNumPartners(); i++)
	{
		switch (app->player->GetPartners()[i].type)
		{
		case KEILER:
			SaveState(KEILER_, i);
			break;
		case ISRRA:
			SaveState(ISRRA_, i);
			break;
		case BRENDA:
			SaveState(BRENDA_, i);
			break;
		default:
			break;
		}
	}
}

void SceneBattle::BubbleSort()
{
	int numSwaps = -1;
	while (numSwaps != 0)
	{
		numSwaps = 0;
		for (int i = 0; i < tam - 1; i++)
		{
			Entity aux;
			if (turnSort[i].stats.speed < turnSort[i + 1].stats.speed)
			{
				aux = turnSort[i];
				turnSort[i] = turnSort[i + 1];
				turnSort[i + 1] = aux;
				numSwaps++;
			}
		}
	}
}

void SceneBattle::DisplaceToLeft()
{
	Entity aux = turnSort[0];
	for (int i = 0; i < tam-1; i++)
	{
		turnSort[i] = turnSort[i + 1];
	}
	turnSort[tam - 1] = aux;
}