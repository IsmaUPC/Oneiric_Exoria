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

	victory=false;
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
	app->player->positionInitial = new iPoint(570, 270);	

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

			if (typeNode % 2 == 0)pos.y -= 2;
			else pos.y += 2;


			pos = app->map->MapToWorld(pos);

			app->player->positionInitial = new iPoint(pos.x, pos.y);
		}
		app->sceneManager->originTpNode = nullptr;
	}


	// Active calls
	app->player->Init();
	app->player->Start();
	app->audio->active = true;

	// Add Entities
	app->entityManager->AddEntity(BANDIT, 26, 14, 1, 1);
	app->entityManager->AddEntity(FIGHTER, 24, 8, 2, 1, false);
	app->entityManager->AddEntity(SAPLING, 16, 5, 3, 2, false);
	app->entityManager->AddEntity(NPC, 31, 23, 1, 0, false);
	app->entityManager->AddEntity(NPC, 21, 6, 2, 0, false);

	// Load music
	app->audio->PlayMusic("Assets/Audio/Music/hades_8bits.ogg");

	// Dialog System buttons
	btn1 = new GuiButton(40, { -app->render->camera.x + WINDOW_W / 2 - 400, -app->render->camera.y + 675, 150, 50 }, "booooton", RECTANGLE);
	btn1->SetObserver(this);
	app->guiManager->AddGuiButton(btn1);

	btn2 = new GuiButton(40, { -app->render->camera.x + WINDOW_W / 2 - 400 + 175, -app->render->camera.y + 675, 150, 50 }, "booooton", RECTANGLE);
	btn2->SetObserver(this);
	app->guiManager->AddGuiButton(btn2);

	btn3 = new GuiButton(40, { -app->render->camera.x + WINDOW_W / 2 - 400 + 250, -app->render->camera.y + 675, 150, 50 }, "booooton", RECTANGLE);
	btn3->SetObserver(this);
	app->guiManager->AddGuiButton(btn3);

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

	btn1->bounds.x = -app->render->camera.x + WINDOW_W / 2 - 400 + 175;
	btn1->bounds.y = -app->render->camera.y + 675;
	btn2->bounds.x = -app->render->camera.x + WINDOW_W / 2 - 400 + 175*2;
	btn2->bounds.y = -app->render->camera.y + 675;
	btn3->bounds.x = -app->render->camera.x + WINDOW_W / 2 - 400 + 175*3;
	btn3->bounds.y = -app->render->camera.y + 675;

	if (app->dialogueSystem->onDialog == false)
	{
		btn1->active = false;
		btn2->active = false;
		btn3->active = false;
	}
	else
	{
		btn1->active = true;
		btn2->active = true;
		btn3->active = true;
	}

	return ret;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	// Draw map
	app->map->Draw();

	bool ret = true;

	if (victory == true)
	{
		victory = false;
		TransitionToScene(SceneType::LEVEL2);
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
bool Scene::CleanUp()
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

void Scene::DebugKeys()
{
	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
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
		if (control->id == 40)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 0);
		}
		//Option 2
		else if (control->id == 41)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 1);
		}
		//Option 3
		else if (control->id == 42)
		{
			app->dialogueSystem->PerformDialogue(app->dialogueSystem->id, 2);
		}
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
