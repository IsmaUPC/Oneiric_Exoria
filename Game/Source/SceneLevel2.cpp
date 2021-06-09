#include "SceneLevel2.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Player.h"
#include "Map.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "DialogSystem.h"
#include "QuestManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

#define PARALLAX_SPEED -1.3f

SceneLevel2::SceneLevel2(SceneType type) : SceneControl(type)
{
	active = true;
	name.Create("sceneLevel2");
}

// Destructor
SceneLevel2::~SceneLevel2()
{}

// Called before render is available
bool SceneLevel2::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	numThisScene = 2;
	return ret;
}

// Called before the first frame
bool SceneLevel2::Start()
{
	app->SaveConfigRequested();
	
	app->audio->PlayMusic("Audio/Music/level_music.ogg");

	fxList[0].fxName = app->audio->LoadFx("Audio/Fx/arcade_machine.wav");
	fxList[1].fxName = app->audio->LoadFx("Audio/Fx/coffe_machine.wav");
	fxList[2].fxName = fxList[1].fxName;
	fxList[3].fxName = app->audio->LoadFx("Audio/Fx/tv.wav");
	fxList[4].fxName = app->audio->LoadFx("Audio/Fx/washing_machine.wav");

	fxCount = 5;

	// Load map
	app->SetLastScene((Module*)this);
	victory = false;
	app->player->changeScene = false;
	app->map->Load("school_2.tmx");
	app->map->active = true;
	// Positions initials
	app->player->positionInitial = new iPoint(930,730);

	app->player->Init();
	app->player->Start();

	// Return size image
	//SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);

	//NPC
	app->entityManager->AddEntity(NPC, 23, 21, 4, 0, false);
	app->entityManager->AddEntity(NPC, 11, 38, 5, 0, false);
	app->entityManager->AddEntity(NPC, 12, 34, 6, 0, false);

	// Interactive objects
	app->entityManager->AddEntity(NPC, 24, 19, 10, 0, false); // Arcade 
	app->entityManager->AddEntity(NPC, 33, 18, 11, 0, false); // Tv
	app->entityManager->AddEntity(NPC, 45, 37, 12, 0, false); // Book
	app->entityManager->AddEntity(NPC, 46, 22, 13, 0, false); // Coffe
	app->entityManager->AddEntity(NPC, 11, 22, 14, 0, false); // Water

	// Position Items
	fxList[0].position = {24,19};
	fxList[0].position = app->map->MapToWorld(fxList[0].position);
	fxList[1].position = {46,22};
	fxList[1].position = app->map->MapToWorld(fxList[1].position);
	fxList[2].position = {11,22};
	fxList[2].position = app->map->MapToWorld(fxList[2].position);
	fxList[3].position = {33,18};
	fxList[3].position = app->map->MapToWorld(fxList[3].position);
	fxList[4].position = { 8,7 };
	fxList[4].position = app->map->MapToWorld(fxList[4].position);

	// Items Channel
	for (int i = 0; i < fxCount; i++)
	{
		fxList[i].channel = app->audio->SetChannel();
	}

	// Set Max Distance
	fxList[0].maxDistance = 96;
	fxList[1].maxDistance = 32;
	fxList[2].maxDistance = 128;
	fxList[3].maxDistance = 128;
	fxList[4].maxDistance = 160;

	//Interactive objects
	app->entityManager->AddEntity(NPC, 24, 19, 10, 0, false);
	app->entityManager->AddEntity(NPC, 33, 18, 11, 0, false);
	app->entityManager->AddEntity(NPC, 45, 37, 12, 0, false);
	app->entityManager->AddEntity(NPC, 46, 22, 13, 0, false);
	app->entityManager->AddEntity(NPC, 11, 22, 14, 0, false);

	// Dialog System buttons
	btn1 = new GuiButton(40, { -app->render->camera.x + WINDOW_W / 2 - 400, -app->render->camera.y + 675, 150, 50 }, "", RECTANGLE);
	btn1->SetObserver(this);
	app->guiManager->AddGuiButton(btn1);

	btn2 = new GuiButton(41, { -app->render->camera.x + WINDOW_W / 2 - 400 + 175, -app->render->camera.y + 675, 150, 50 }, "", RECTANGLE);
	btn2->SetObserver(this);
	app->guiManager->AddGuiButton(btn2);

	btn3 = new GuiButton(42, { -app->render->camera.x + WINDOW_W / 2 - 400 + 250, -app->render->camera.y + 675, 150, 50 }, "", RECTANGLE);
	btn3->SetObserver(this);
	app->guiManager->AddGuiButton(btn3);

	return true;
}

void SceneLevel2::SetDebugCollaider()
{
	app->map->SetDrawColl();
}

// Called each loop iteration
bool SceneLevel2::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneLevel2::Update(float dt)
{
	// DEBUG KEYS
	DebugKeys();
	//app->map->checKpointsMap.checkPointOnAnim->Update();
	iPoint vec;
	vec.x = 0, vec.y = 0;
	app->input->GetMousePosition(vec.x, vec.y);


	if (app->player->changeScene)victory = true;

	else if (app->player->CheckGameOver(2) && lose == false && app->player->godMode == false)
	{
		LOG("GAME OVER!");
		lose = true;
	}

	//GamePad& pad = app->input->pads[0];
	if (app->dialogueSystem->missClick && !app->input->pads[0].a) {
		app->dialogueSystem->missClick = false;
	}

	UpdateDialog();

	for (int i = 0; i < fxCount; i++)
	{
		if (app->audio->SetDistanceFx(fxList[i].channel, AngleToListener(app->player->playerData.position, fxList[i].position) + 90,
			DistanceToListener(app->player->playerData.position, fxList[i].position), fxList[i].maxDistance))
			app->audio->PlayFx(fxList[i].fxName, fxList[i].channel);
		else app->audio->StopFx(fxList[i].channel);
	}

	return true;
}

void SceneLevel2::UpdateDialog()
{
	if ((app->dialogueSystem->onDialog == true) && (app->dialogueSystem->actualLetter == app->dialogueSystem->totalLetters || app->dialogueSystem->dialogSpeed == 0))
	{
		int w, h;
		for (int i = 0; i < app->dialogueSystem->currentNode->answersList.Count(); i++)
		{
			btn1->active = false;
			btn2->active = false;
			btn3->active = false;
			if (i == 0)
			{
				btn1->text = app->dialogueSystem->currentNode->answersList.At(i)->data.c_str();
				btn1->active = true;
				TTF_SizeText(app->sceneManager->guiFont, btn1->text.GetString(), &w, &h);
				btn1->ResizeButton(&w, &h);
				btn1->bounds.x = (-app->render->camera.x + WINDOW_W / 2 - 300) + 80;
				btn1->bounds.y = -app->render->camera.y + 665;
			}
			if (i == 1)
			{
				btn2->text = app->dialogueSystem->currentNode->answersList.At(i)->data.c_str();
				btn1->active = true;
				btn2->active = true;
				TTF_SizeText(app->sceneManager->guiFont, btn2->text.GetString(), &w, &h);
				btn2->ResizeButton(&w, &h);
				btn2->bounds.x = (-app->render->camera.x + WINDOW_W / 2 - 300) + 80 + 175;
				btn2->bounds.y = -app->render->camera.y + 665;
			}
			if (i == 2)
			{
				btn3->text = app->dialogueSystem->currentNode->answersList.At(i)->data.c_str();
				btn1->active = true;
				btn2->active = true;
				btn3->active = true;
				TTF_SizeText(app->sceneManager->guiFont, btn3->text.GetString(), &w, &h);
				btn3->ResizeButton(&w, &h);
				btn3->bounds.x = (-app->render->camera.x + WINDOW_W / 2 - 300) + 80 + 175 * 2;
				btn3->bounds.y = -app->render->camera.y + 665;
			}
		}
	}

	if (app->dialogueSystem->onDialog == false || app->sceneManager->GetIsPause())
	{
		btn1->active = false;
		btn2->active = false;
		btn3->active = false;
	}
}

// Called each loop iteration
bool SceneLevel2::PostUpdate()
{
	// Draw map
	app->map->Draw();

	bool ret = true;

	return ret;
}

// Called before quitting
bool SceneLevel2::CleanUp()
{
	bool ret = true;

	if (!active)
		return true;

	for (int i = 0; i < fxCount; i++)
	{
		app->audio->Unload1Fx(fxList[i].fxName);
		app->audio->DeleteChannel(fxList[i].channel);
	}

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->map->CleanUp();
	app->player->CleanUp();
	app->entityManager->ClearList(ret);

	app->sceneManager->SetPause(false);

	RELEASE(btn1);
	RELEASE(btn2);
	RELEASE(btn3);

	active = false;
	return ret;
}


void SceneLevel2::DebugKeys()
{
	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) 
	{
		app->render->camera.x = 0;
		app->player->playerData.position = *app->player->positionInitial;
		app->player->playerData.direction = WALK_R;
		Mix_RewindMusic();
	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) 
		SetDebugCollaider();
	

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) 
	{
		if (app->player->godMode == false)app->player->godMode = true;
		else app->player->godMode = false;
	}
}
bool SceneLevel2::OnGuiMouseClickEvent(GuiControl* control)
{
	bool ret = true;
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		//Option 1
		if (control->id == 40 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 0);
			btn1->state = GuiControlState::NORMAL;
		}
		//Option 2
		else if (control->id == 41 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 1);
			btn2->state = GuiControlState::NORMAL;
		}
		//Option 3
		else if (control->id == 42 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 2);
			btn3->state = GuiControlState::NORMAL;
		}
		app->dialogueSystem->missClick = true;
		app->dialogueSystem->actualLetter = 0;
		btn1->active = false;
		btn2->active = false;
		btn3->active = false;
	}
	default: break;
	}
	ret = app->guiManager->GetMenuPause()->Event(control);
	app->guiManager->GetStatsMenu()->Event(control);

	return ret;
}

bool SceneLevel2::LoadState(pugi::xml_node& data)
{
	return true;
}

bool SceneLevel2::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(2);
	return true;
}