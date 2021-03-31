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

	if (app->map->Load("map_1.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(w, h, &data)) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	app->map->active = true;

	// Positions Initials
	app->entityManager->Start();
	app->player->positionInitial = new iPoint(432,1170 );
	app->entityManager->AddEntity(GROUND_ENEMY, 43, 27);
	app->entityManager->AddEntity(GROUND_ENEMY, 30, 17);
	app->entityManager->AddEntity(GROUND_ENEMY, 20, 14);	

	// Active calls
	app->player->Init();
	app->player->Start();
	app->audio->active = true;

	// Load music
	app->audio->PlayMusic("Assets/Audio/Music/hades_8bits.ogg");
	img = app->tex->Load("Assets/Textures/sky.png");
	animationFather.texture = app->tex->Load("Assets/Textures/dino_orange.png");
	
	animationFather.position = { 2352, 495 };
	idleAnim->loop = true;
	idleAnim->speed = 0.025;

	for (int i = 0; i < 4; i++)
		idleAnim->PushBack({ 117 * i,0, 117, 117 });

	animationFather.currentAnimation = idleAnim;

	SDL_QueryTexture(img, NULL,NULL,&imgW,&imgH);

	app->render->camera.y -= imgH;

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

	animationFather.currentAnimation->Update();

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
	// Draw Background
	Parallax();
	// Draw map
	app->map->Draw();

	bool ret = true;
	SDL_Rect rectFather;
	rectFather = animationFather.currentAnimation->GetCurrentFrame();

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
	app->render->DrawTextureFlip(animationFather.texture, animationFather.position.x, animationFather.position.y - (rectFather.h), &rectFather);


	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->map->CleanUp();
	app->tex->UnLoad(img);
	app->tex->UnLoad(animationFather.texture);
	app->player->CleanUp();
	app->entityManager->CleanUp();

	app->sceneManager->SetPause(false);

	active = false;
	return true;
}

void Scene::Parallax()
{
	speedImg = -0.9f;
	imgX = (int)(app->render->camera.x / 6) - 10;
	imgX *= speedImg;

	imgY = (int)((app->render->camera.y / 6) + 1250) * 0.2f;

	app->render->DrawTexture(img, imgX, imgY);
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
	return app->sceneManager->menu->Event(control);
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
