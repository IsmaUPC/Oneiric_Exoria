#include "SceneDungeon.h"
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

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

#define PARALLAX_SPEED -1.3f

SceneDungeon::SceneDungeon(SceneType type) : SceneControl(type)
{
	active = true;
	name.Create("dungeon");
}

// Destructor
SceneDungeon::~SceneDungeon()
{}

// Called before render is available
bool SceneDungeon::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	numThisScene = 2;
	return ret;
}

// Called before the first frame
bool SceneDungeon::Start()
{
	//app->SaveConfigRequested();
	
	//app->audio->PlayMusic("Audio/Music/level_music.ogg");
	app->player->positionInitial = new iPoint(200, 500);

	// TODO dungoen
	tmxDungeon = app->sceneManager->tmxDungeonsList.At(app->sceneManager->levelDungeon)->data.GetString();
	// Load map
	app->SetLastScene((Module*)this);

	app->audio->PlayMusic("Audio/Music/dungeon_music.ogg");
	app->audio->SetVolumeMusic(app->sceneManager->GetCurrentVolume() + 20);
	victory = false;
	app->player->win = false;
	LoadDungeon(tmxDungeon);

	app->map->LoadObstaclesDungeon();

	app->map->active = true;
	// Positions initials
	//app->player->positionInitial = new iPoint(930,730);

	app->player->Init();
	app->player->Start();


	// Return size image
	//SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);

	
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

void SceneDungeon::LoadDungeon(const char* dungeon)
{
	app->map->Load(dungeon);
}

void SceneDungeon::SetDebugCollaider()
{
	app->map->SetDrawColl();
}

// Called each loop iteration
bool SceneDungeon::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneDungeon::Update(float dt)
{
	// DEBUG KEYS
	DebugKeys();
	iPoint vec;
	vec.x = 0, vec.y = 0;
	app->input->GetMousePosition(vec.x, vec.y);
	if (app->player->win)victory = true;
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

	return true;
}

void SceneDungeon::UpdateDialog()
{
	if (app->dialogueSystem->onDialog == true)
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
bool SceneDungeon::PostUpdate()
{
	// Draw map
	app->map->Draw();

	bool ret = true;

	return ret;
}

// Called before quitting
bool SceneDungeon::CleanUp()
{
	bool ret = true;

	if (!active)
		return true;

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


void SceneDungeon::DebugKeys()
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
bool SceneDungeon::OnGuiMouseClickEvent(GuiControl* control)
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
			app->dialogueSystem->missClick = true;
			btn1->state = GuiControlState::NORMAL;
		}
		//Option 2
		else if (control->id == 41 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 1);
			app->dialogueSystem->missClick = true;
			btn2->state = GuiControlState::NORMAL;
		}
		//Option 3
		else if (control->id == 42 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 2);
			app->dialogueSystem->missClick = true;
			btn3->state = GuiControlState::NORMAL;
		}
	}
	default: break;
	}
	ret = app->guiManager->GetMenuPause()->Event(control);
	app->guiManager->GetStatsMenu()->Event(control);

	return ret;
}

bool SceneDungeon::LoadState(pugi::xml_node& data)
{
	return true;
}

bool SceneDungeon::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(lastLevel+1);
	return true;
}


