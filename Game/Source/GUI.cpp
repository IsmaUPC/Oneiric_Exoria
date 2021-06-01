#include "GUI.h"
#include "Player.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "Fonts.h"
#include "DialogSystem.h"
#include "GuiManager.h"
#include "SceneBattle.h"
#include "EntityManager.h"

#include "Defs.h"
#include "Log.h"

GUI::GUI() : Entity()
{
	active = true;
	name.Create("GUI");
}

GUI::GUI(Entity* entity, SDL_Texture* pTexture)
{
	name.Create("GUI");
	entityData = entity->entityData;
	entityData.texture = pTexture;
}

GUI::~GUI()
{
}

bool GUI::Awake(pugi::xml_node& config)
{
	LOG("Loading GUI Parser");
	bool ret = true;

	return ret;
}

bool GUI::Start()
{
	active = true;
	int imgH = 0;
	int imgW = 0;

	//playerUi = app->tex->Load("Textures/Characters/atlas_players_battle.png");

	activeFPS = false;
	/*timer.Start();
	minuts = app->entityManager->timeSave / 60000;*/
	return true;
}

bool GUI::PreUpdate()
{
	if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		activeFPS = !activeFPS;
	}
	return true;
}

bool GUI::Update(float dt)
{
	//miliseconds = timer.Read()+app->entityManager->timeSave - minuts * 60000;
	//Chronometer();
	entityData.position.x = -app->render->camera.x;
	entityData.position.y = -app->render->camera.y;		
	
	if (app->dialogueSystem->pendingDialog)
	{
		if (app->dialogueSystem->despawnDialog)
		{
			initSpawnPos = true;
			app->dialogueSystem->despawnDialog = false;
		}			

		if (initSpawnPos)
		{
			if (app->dialogueSystem->spawnDialog) InitPosBoxText(160, -160);
			else InitPosBoxText(0, 160);
			initSpawnPos = false;
		}		

		if (currentIteration < totalIterations)
		{
			hight = EaseCubicIn(currentIteration, spawnPos, deltaPos, totalIterations);
			currentIteration++;
		}
		else
		{
			if (!app->dialogueSystem->spawnDialog)
				app->dialogueSystem->pendingDialog = false;
			else
			{
				hight = 0;
				app->dialogueSystem->onDialog = true;
			}
		}
	}	

	return true;
}

bool GUI::PostUpdate()
{
	// Text box
	if (app->dialogueSystem->pendingDialog == true)
	{
		app->render->DrawTextBox(-app->render->camera.x + WINDOW_W / 2 - 300, -app->render->camera.y + 565 + hight, 600, 150, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	}
	if (app->entityManager->drawCloud == true)
	{
		app->render->DrawTexture(app->guiManager->uiAtlas, app->player->playerData.position.x + 10, app->player->playerData.position.y - 30, &app->guiManager->talkCloud->GetCurrentFrame());
		app->entityManager->drawCloud = false;
	}
	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;

	// Time
	//point0.x = -app->render->camera.x;
	//point0.y = -app->render->camera.y;
	//point0.x = point0.x + (WINDOW_W - 250);
	//point0.y = point0.y + 20;
	//
	//if (app->sceneManager->GetIsPause() && !stopTime)
	//{
	//	auxTimePause.Start();
	//	stopTime = true;
	//}
	//if (!app->sceneManager->GetIsPause() && stopTime)
	//{
	//	timer.startTime += auxTimePause.Read();
	//	stopTime = false;
	//}
	//point0.x = point0.x - 100;
	//sprintf_s(timeText, 10, "%d:%02d:%02d", minuts, miliseconds / 100, miliseconds2);
	//app->fonts->BlitText(point0.x, point0.y, 0, timeText, { 255, 255, 255 });

	if (activeFPS)
	{
		point0.x += 170;
		point0.y += WINDOW_H - 100;
		sprintf_s(fps, 10, "%3d", app->GetFramesOnLastSecond());

		app->fonts->BlitText(point0.x, point0.y, 0, fps, { 60, 43, 13 });

	}

	return true;
}

void GUI::InitPosBoxText(int spawnPos_, int deltaPos_)
{
	spawnPos = spawnPos_;
	deltaPos = deltaPos_;
	currentIteration = 0;
}

void GUI::Chronometer()
{
	/*if (miliseconds >= 60000 && !stopTime)
	{
		minuts++;
	}
	miliseconds = miliseconds * 0.1;

	int centenas = 0;
	if (miliseconds >= 100)
	{
		centenas = miliseconds / 100;
		miliseconds2 = miliseconds - (centenas * 100);
	}*/	
}

bool GUI::CleanUp()
{
	if (!active)
	{
		return true;
	}

	app->tex->UnLoad(playerUi);

	active = false;

	pendingToDelete = true;
	return true;
}
bool GUI::LoadState(pugi::xml_node& data)
{
	app->entityManager->timeSave = data.child("time").attribute("value").as_int(0);
	
	return true;
}

bool GUI::SaveState(pugi::xml_node& data) const
{
	/*if(!app->removeGame)data.child("time").attribute("value").set_value(miliseconds*10+minuts*60000);
	else data.child("time").attribute("value").set_value(0);*/
	return true;
}

