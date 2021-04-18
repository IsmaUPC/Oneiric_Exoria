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
#include "Pathfinding.h"
#include "DialogSystem.h"

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
	
	app->audio->PlayMusic("Assets/Audio/Music/level_music.ogg");
	loseFx = app->audio->LoadFx("Assets/Audio/Fx/lose.wav");
	// Load map
	app->SetLastScene((Module*)this);
	victory = false;
	app->player->win = false;
	if (app->map->Load("school_2.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;

		if (app->map->CreateWalkabilityMap(w, h, &data)) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}
	app->map->active = true;
	// Positions initials
	app->player->positionInitial = new iPoint(400,750);


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

	//Parallax
	moveBG0 = -1;
	moveBG1 = 0;
	moveBG2 = 1;
	posX0 = 0;
	posX1 = 0;
	posX2 = 0;
	xW = 0;
	xSpeed = 0;

	idleAnim.loop = true;
	idleAnim.speed = 0.025;

	for (int i = 0; i < 4; i++)
		idleAnim.PushBack({ 117 * i,0, 117, 117 });


	app->render->camera.y -= imgH;
	app->sceneManager->lastLevel = 2;

	speedImg = PARALLAX_SPEED;

	app->entityManager->AddEntity(NPC, 23, 21, 4, 0, false);
	app->entityManager->AddEntity(NPC, 11, 38, 5, 0, false);
	app->entityManager->AddEntity(NPC, 12, 34, 6, 0, false);

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

void SceneLevel2::UpdateDialog()
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
bool SceneLevel2::PostUpdate()
{
	// Draw Background
	Parallax();
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
bool SceneLevel2::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->map->CleanUp();
	app->player->CleanUp();

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

bool SceneLevel2::LoadState(pugi::xml_node& data)
{
	return true;
}

bool SceneLevel2::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(2);
	return true;
}