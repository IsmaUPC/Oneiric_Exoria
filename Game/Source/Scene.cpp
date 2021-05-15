#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Scene.h"
#include "Player.h"
#include "EntityManager.h"
#include "Map.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "Pathfinding.h"
#include "DialogSystem.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

Scene::Scene()
{
	active = true;
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	numThisScene = 1;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	app->SaveConfigRequested();
	app->SetLastScene((Module*)this);

	// Load music and Fx
	app->audio->PlayMusic("Assets/Audio/Music/level_music.ogg");

	fxList[0].fxName = app->audio->LoadFx("Assets/Audio/Fx/pc_typing.wav");
	fxCount = 1;

	victory = false;
	app->player->win = false;

	if (app->map->Load("school_1.tmx") == true)
	//if (app->map->Load("level.tmx") == true)
	//if (app->map->Load("room.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(w, h, &data)) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	app->map->active = true;

	// Positions Initials
	app->player->positionInitial = new iPoint(325, 450);	

	// Active calls
	app->player->Init();
	app->player->Start();
	app->audio->active = true;

	//NPCs
	app->entityManager->AddEntity(NPC, 31, 24, 1, 0, false);
	app->entityManager->AddEntity(NPC, 21, 5, 2, 0, false);
	app->entityManager->AddEntity(NPC, 16, 21, 3, 0, false);

	//Interactuable objects
	app->entityManager->AddEntity(NPC, 20, 12, 7, 0, false);
	app->entityManager->AddEntity(NPC, 31, 30, 8, 0, false);
	app->entityManager->AddEntity(NPC, 10, 3, 9, 0, false);
	app->entityManager->AddEntity(NPC, 31, 3, 9, 0, false);

	// Position Items
	fxList[0].position = { 31,24 };
	fxList[0].position = app->map->MapToWorld(fxList[0].position);

	// Items Channel
	for (int i = 0; i < fxCount; i++)
	{
		fxList[i].channel = app->audio->SetChannel();
	}

	// Set Max Distance
	fxList[0].maxDistance = 64;

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

	//app->dialogueSystem->missClick = false;
	return true;
}

void Scene::SetDebugCollaider()
{
	app->map->SetDrawColl();
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	bool ret = true;

	//GamePad& pad = app->input->pads[0];
	if (app->dialogueSystem->missClick && !app->input->pads[0].a) {
		app->dialogueSystem->missClick = false;
	}

	// DEBUG KEYS
	DebugKeys();
	app->map->checKpointsMap.checkPointOnAnim->Update();
	if (app->input->GetKey(SDL_SCANCODE_M)==KEY_UP)
	{
		app->input->GetMousePosition(app->map->tileDestiny.x, app->map->tileDestiny.y);
		app->map->tileDestiny = app->map->WorldToMap( (app->render->camera.x*-1+  app->map->tileDestiny.x),
			(app->render->camera.y -app->map->tileDestiny.y)*-1);
	}

	iPoint vec;
	vec.x = 0, vec.y = 0;
	app->input->GetMousePosition(vec.x, vec.y);

	idleAnim->speed = (dt * 100) * 0.025f;

	if(app->player->win)victory = true;

	else if (app->player->CheckGameOver(1) && lose == false && app->player->godMode == false)
	{
		LOG("GAME OVER!");
		lose = true;
	}

	UpdateDialog();

	for (int i = 0; i < fxCount; i++)
	{
		if (app->audio->SetDistanceFx(fxList[i].channel, AngleToListener(app->player->playerData.position, fxList[i].position) + 90,
			DistanceToListener(app->player->playerData.position, fxList[i].position), fxList[i].maxDistance))
			app->audio->PlayFx(fxList[i].fxName, fxList[i].channel);
		else app->audio->StopFx(fxList[i].channel);
	}

	return ret;
}

void Scene::UpdateDialog()
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
				btn3->bounds.x = (-app->render->camera.x + WINDOW_W / 2 - 300) + 80 + 175*2;
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
bool Scene::PostUpdate()
{
	// Draw map
	app->map->Draw();

	bool ret = true;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	bool ret = true;
	if (!active)
		return true;

	for (int i = 0; i < fxCount; i++)
	{
		app->audio->Unload1Fx(fxList[i].fxName);
		app->audio->DeleteChannel(fxList[i].channel);
	}

	//////LOG("Freeing scene");
	Mix_HaltMusic();
	app->map->CleanUp();
	app->player->CleanUp();
	app->entityManager->ClearList(ret);

	app->sceneManager->SetPause(false);

	active = false;
	return ret;
}

void Scene::DebugKeys()
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
	{
		SetDebugCollaider();
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (app->player->godMode == false)app->player->godMode = true;
		else app->player->godMode = false;
	}
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
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

	return app->guiManager->GetMenuPause()->Event(control);
}

bool Scene::LoadState(pugi::xml_node& data)
{
	return true;
}

bool Scene::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(1);
	return true;
}
