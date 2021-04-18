#include "SceneLevel3.h"
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
#include "Pathfinding.h"
#include "DialogSystem.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

#define PARALLAX_SPEED -1.3f

SceneLevel3::SceneLevel3()
{
	active = true;
	name.Create("sceneLevel3");
}

// Destructor
SceneLevel3::~SceneLevel3()
{}

// Called before render is available
bool SceneLevel3::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	numThisScene = 2;
	return ret;
}

// Called before the first frame
bool SceneLevel3::Start()
{
	app->SaveConfigRequested();

	app->audio->PlayMusic("Assets/Audio/Music/level_music.ogg");
	loseFx = app->audio->LoadFx("Assets/Audio/Fx/lose.wav");
	// Load map
	app->SetLastScene((Module*)this);
	victory = false;
	app->player->win = false;
	if (app->map->Load("school_3.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(w, h, &data)) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	app->map->active = true;
	// Positions initials
	app->player->positionInitial = new iPoint(480, 490);


	//Move to TpNode Class
	  //Spawn Player in Tp Position
	{
		if (app->sceneManager->originTpNode != NULL)
		{
			int idNode = app->sceneManager->originTpNode->idNode;
			uint typeNode = app->sceneManager->originTpNode->typeTpNode;//select next node

			//if the type of node is even, it means that it is of type down, if it is odd otherwise, 
			//to decide the next one it is added or subtracted depending on its origin
			(typeNode % 2 == 0) ? typeNode += 1 : typeNode -= 1;

			iPoint pos = app->player->FindNodeTpById(typeNode, idNode)->position;

			if (typeNode % 2 == 0)
			{
				pos.y -= 2;
				app->player->playerData.direction = WALK_UP;
			}
			else
			{
				pos.y += 2;
				app->player->playerData.direction = WALK_DOWN;
			}

			pos = app->map->MapToWorld(pos);

			app->player->positionInitial = new iPoint(pos.x, pos.y);
		}
		app->sceneManager->originTpNode = nullptr;
	}

	app->player->Init();
	app->player->Start();

	// Add Entities
	app->entityManager->AddEntity(BANDIT, 14, 9, 1, 1);
	app->entityManager->AddEntity(FIGHTER, 24, 8, 2, 1, false);
	app->entityManager->AddEntity(SAPLING, 4, 12, 3, 2, false);

	//SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);


	// Dialog System buttons
	btn1 = new GuiButton(40, { -app->render->camera.x + WINDOW_W / 2 - 400, -app->render->camera.y + 675, 150, 50 }, "booooton", RECTANGLE);
	btn1->SetObserver(this);
	app->guiManager->AddGuiButton(btn1);

	btn2 = new GuiButton(41, { -app->render->camera.x + WINDOW_W / 2 - 400 + 175, -app->render->camera.y + 675, 150, 50 }, "booooton", RECTANGLE);
	btn2->SetObserver(this);
	app->guiManager->AddGuiButton(btn2);

	btn3 = new GuiButton(42, { -app->render->camera.x + WINDOW_W / 2 - 400 + 250, -app->render->camera.y + 675, 150, 50 }, "booooton", RECTANGLE);
	btn3->SetObserver(this);
	app->guiManager->AddGuiButton(btn3);

	return true;
}

void SceneLevel3::SetDebugCollaider()
{
	app->map->SetDrawColl();
}

// Called each loop iteration
bool SceneLevel3::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool SceneLevel3::Update(float dt)
{
	// DEBUG KEYS
	DebugKeys();
	app->map->checKpointsMap.checkPointOnAnim->Update();
	iPoint vec;
	vec.x = 0, vec.y = 0;
	app->input->GetMousePosition(vec.x, vec.y);


	if (app->player->win)victory = true;

	else if (app->player->CheckGameOver(2) && lose == false && app->player->godMode == false)
	{
		LOG("GAME OVER!");
		lose = true;
		app->audio->PlayFx(loseFx);
	}

	//GamePad& pad = app->input->pads[0];
	if (app->dialogueSystem->missClick && !app->input->pads[0].a) {
		app->dialogueSystem->missClick = false;
	}

	UpdateDialog();


	return true;
}

void SceneLevel3::UpdateDialog()
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

	if (app->dialogueSystem->onDialog == false)
	{
		btn1->active = false;
		btn2->active = false;
		btn3->active = false;
	}
}

// Called each loop iteration
bool SceneLevel3::PostUpdate()
{

	// Draw map
	app->map->Draw();

	bool ret = true;

	if (victory == true)
	{
		victory = false;
		TransitionToScene(SceneType::LEVEL1);
		return true;
	}
	if (lose == true)
	{
		lose = false;
		TransitionToScene(SceneType::LOSE);
		return true;
	}

	return ret;
}

// Called before quitting
bool SceneLevel3::CleanUp()
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

	active = false;
	return ret;
}


void SceneLevel3::DebugKeys()
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
bool SceneLevel3::OnGuiMouseClickEvent(GuiControl* control)
{
	switch (control->type)
	{
	case GuiControlType::BUTTON:
	{
		//Option 1
		if (control->id == 40 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 0);
			app->dialogueSystem->missClick = true;
		}
		//Option 2
		else if (control->id == 41 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 1);
			app->dialogueSystem->missClick = true;
		}
		//Option 3
		else if (control->id == 42 && !app->dialogueSystem->missClick)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 2);
			app->dialogueSystem->missClick = true;
		}
	}
	default: break;
	}

	return app->guiManager->GetMenuPause()->Event(control);
}

bool SceneLevel3::LoadState(pugi::xml_node& data)
{
	return true;
}

bool SceneLevel3::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(2);
	return true;
}