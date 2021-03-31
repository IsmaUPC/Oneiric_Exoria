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
#include "Pathfinding.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

#define PARALLAX_SPEED -1.3f

SceneLevel2::SceneLevel2()
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
	
	// Load map
	app->SetLastScene((Module*)this);
	victory = false;
	app->player->win = false;
	if (app->map->Load("map_2.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(w, h, &data)) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	app->map->active = true;
	// Positions initials
	app->player->positionInitial = new iPoint(576,1534);
	app->entityManager->Start();
	app->entityManager->AddEntity(GROUND_ENEMY, 34, 31);
	app->entityManager->AddEntity(GROUND_ENEMY, 57, 25);
	app->entityManager->AddEntity(GROUND_ENEMY, 77, 8);
	app->entityManager->AddEntity(GROUND_ENEMY, 119, 10);
	app->entityManager->AddEntity(GROUND_ENEMY, 149, 8);
	app->entityManager->AddEntity(GROUND_ENEMY, 130, 8);
	app->entityManager->AddEntity(GROUND_ENEMY, 168, 8);
	app->entityManager->AddEntity(GROUND_ENEMY, 198, 6);
	app->entityManager->AddEntity(GROUND_ENEMY, 77, 29);
	app->entityManager->AddEntity(GROUND_ENEMY, 127, 29);
	app->entityManager->AddEntity(GROUND_ENEMY, 145, 29);
	app->entityManager->AddEntity(GROUND_ENEMY, 163, 23);
	app->entityManager->AddEntity(GROUND_ENEMY, 203, 15);
	app->entityManager->AddEntity(AIR_ENEMY, 53, 20);
	app->entityManager->AddEntity(AIR_ENEMY, 66, 10);
	app->entityManager->AddEntity(AIR_ENEMY, 189, 11);
	app->entityManager->AddEntity(AIR_ENEMY, 201, 9);

	app->player->Init();
	app->player->Start();
	

	// Load music
	app->audio->PlayMusic("Assets/Audio/Music/loki_8bits.ogg");

	//Parallax
	img = app->tex->Load("Assets/Textures/sky_2.png");
	SDL_QueryTexture(img, NULL, NULL, &withBG, &moveBG1);
	moveBG0 = -1;
	moveBG1 = 0;
	moveBG2 = 1;
	posX0 = 0;
	posX1 = 0;
	posX2 = 0;
	xW = 0;
	xSpeed = 0;
	animationFather.texture = app->tex->Load("Assets/Textures/dino_orange.png");

	animationFather.position = { 10500, 639 };
	idleAnim.loop = true;
	idleAnim.speed = 0.025;

	for (int i = 0; i < 4; i++)
		idleAnim.PushBack({ 117 * i,0, 117, 117 });

	animationFather.currentAnimation = &idleAnim;

	SDL_QueryTexture(img, NULL, NULL, &imgW, &imgH);

	app->render->camera.y -= imgH;
	app->sceneManager->lastLevel = 2;

	speedImg = PARALLAX_SPEED;
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
	app->map->checKpointsMap.checkPointOnAnim->Update();
	iPoint vec;
	vec.x = 0, vec.y = 0;
	app->input->GetMousePosition(vec.x, vec.y);

	idleAnim.speed = (dt * 100) * 0.025f;

	animationFather.currentAnimation->Update();
	
	if (app->player->win)victory = true;

	else if (app->player->CheckGameOver(2) && lose == false && app->player->godMode == false)
	{
		LOG("GAME OVER!");
		lose = true;
	}
	return true;
}

// Called each loop iteration
bool SceneLevel2::PostUpdate()
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
bool SceneLevel2::CleanUp()
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

void SceneLevel2::Parallax()
{
	imgY = (int)((app->render->camera.y / 6)) * -0.2f;
	
	int x = -app->render->camera.x;
	imgX = (int)(app->render->camera.x / 6) - 10;
	imgX *= PARALLAX_SPEED;
	
	xW = x + withBG;
	xSpeed = x + imgX;

	posX0 = (moveBG0 * withBG);
	posX1 = (moveBG1 * withBG);
	posX2 = (moveBG2 * withBG);

	////Back to front
	if (xW + imgX > (posX0 + (withBG * 0.5f)) + imgX && xSpeed > (withBG * (moveBG0 + 2)) + imgX) moveBG0 += 3;
	if (xW + imgX > (posX1 + (withBG * 0.5f)) + imgX && xSpeed > (withBG * (moveBG1 + 2)) + imgX) moveBG1 += 3;
	if (xW + imgX > (posX2 + (withBG * 0.5f)) + imgX && xSpeed > (withBG * (moveBG2 + 2)) + imgX) moveBG2 += 3;
	//front to back
	if (xSpeed < (posX0 + (withBG * 0.5f)) + imgX && (moveBG2 > moveBG0)) moveBG2 -= 3;
	if (xSpeed < (posX1 + (withBG * 0.5f)) + imgX && (moveBG0 > moveBG1)) moveBG0 -= 3;
	if (xSpeed < (posX2 + (withBG * 0.5f)) + imgX && (moveBG1 > moveBG2)) moveBG1 -= 3;

	posX0 = (moveBG0 * withBG) + imgX;
	posX1 = (moveBG1 * withBG) + imgX;
	posX2 = (moveBG2 * withBG) + imgX;

	app->render->DrawTexture(img, posX0, imgY);
	app->render->DrawTexture(img, posX1, imgY);
	app->render->DrawTexture(img, posX2, imgY);
}

void SceneLevel2::DebugKeys()
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
		SetDebugCollaider();
	

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) 
	{
		if (app->player->godMode == false)app->player->godMode = true;
		else app->player->godMode = false;
	}
}
bool SceneLevel2::OnGuiMouseClickEvent(GuiControl* control)
{
	return app->sceneManager->menu->Event(control);
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