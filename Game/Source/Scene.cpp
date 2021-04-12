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
	app->player->positionInitial = new iPoint(300,300 );	

	// Active calls
	app->player->Init();
	app->player->Start();
	app->audio->active = true;

	// Add Entities
	app->entityManager->AddEntity(BANDIT, 9, 4, 1);

	// Load music
	app->audio->PlayMusic("Assets/Audio/Music/hades_8bits.ogg");

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
		TransitionToScene(SceneType::WIN);
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
