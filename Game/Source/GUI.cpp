#include "GUI.h"
#include "Player.h"
#include "FireBall.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "ModuleFonts.h"

#include "Defs.h"
#include "Log.h"

GUI::GUI() : Entity()
{
	active = true;
	name.Create("GUI");
}

GUI::GUI(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture)
	: Entity(pTypeEntity, pPosition, pVelocity, pTexture)
{
	name.Create("GUI");
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

	headAnim = new Animation();
	arrowAnim = new Animation();
	buttonEAnim = new Animation();
	fireBallOnAnim = new Animation();
	fireBallOffAnim = new Animation();
	coinHudAnim = new Animation();

	active = true;
	int imgH = 0;
	int imgW = 0;

	headTex = entityData->texture;
	SDL_QueryTexture(headTex, NULL, NULL, &headW, &imgH);

	headAnim->PushBack({0,0,headW,imgH });

	imgH = 0;
	imgW = 0;
	arrowTex = app->tex->Load("Assets/Textures/GUI/arrows.png");
	SDL_QueryTexture(arrowTex, NULL, NULL, &imgW, &imgH);
	imgW = imgW / 2;

	arrowAnim->PushBack({ 0,0,imgW,imgH });
	buttonEAnim->PushBack({ imgW,0,imgW,imgH });


	imgCoin = app->tex->Load("Assets/Textures/GUI/coin.png");
	coinHudAnim->loop = true;
	coinHudAnim->speed = 0.20f;

	for (int i = 0; i < 16; i++)
		coinHudAnim->PushBack({ 0,(80 * i), 80, 80 });

	imgH = 0;
	imgW = 0;
	fireBallTex = app->tex->Load("Assets/Textures/GUI/fire_ball.png");
	SDL_QueryTexture(fireBallTex, NULL, NULL, &imgW, &imgH);
	imgH = imgH / 2;

	fireBallOnAnim->PushBack(	{ 0,0,imgW,imgH		});
	fireBallOffAnim->PushBack(	{ 0,imgH,imgW,imgH	});

	respawn = &app->player->playerData.respawns;
	coins = &app->player->playerData.coins;

	// Text
	hudFont = app->fonts->Load("Assets/Textures/GUI/hud_font.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.,0123456789им?!*$%&()+-/:;<=>@_      ", 5, 705, 225);

	fireBallState = app->player->GetStateShoot();

	activeFPS = false;
	timer.Start();
	minuts = app->entityManager->timeSave / 60000;
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

	coinHudAnim->speed = (dt * 9);
	miliseconds = timer.Read()+app->entityManager->timeSave - minuts * 60000;
	coinHudAnim->Update();
	Chronometer();
	return true;
}

bool GUI::PostUpdate()
{
	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;
	
	point0.x = point0.x + headPositionX;
	point0.y = point0.y + headPositionY;


	// Respawns
	SDL_Rect rectGUI;
	rectGUI = headAnim->GetCurrentFrame();
	for (int i = 0; i < *respawn; i++)
		app->render->DrawTexture(headTex,point0.x +((headW + headSeparation)*i),point0.y,&rectGUI);
	
	// ChecKPoints HUD
	rectGUI = buttonEAnim->GetCurrentFrame();
	if (app->player->GetInCheckPoint())
		app->render->DrawTexture(arrowTex, app->player->playerData.position.x+10, app->player->playerData.position.y - 90, &rectGUI);
	
	rectGUI = arrowAnim->GetCurrentFrame();
	if (app->player->GetCheckPointMove())
	{
		app->render->DrawTexture(arrowTex, app->player->playerData.position.x + 60, app->player->playerData.position.y - 40, &rectGUI);
		app->render->DrawTextureFlip(arrowTex,app->player->playerData.position.x - 45, app->player->playerData.position.y - 40, &rectGUI);
	}

	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;

	// Score
	point0.x = point0.x + 50;
	point0.y = point0.y + 100;
	
	sprintf_s(scoreText, 12, "%.06d", app->entityManager->score);
	app->fonts->BlitText(point0.x, point0.y, hudFont, scoreText);

	// Coin HUD
	point0.x = point0.x - 20;
	point0.y = point0.y + 70;

	SDL_Rect rectCoins;
	rectCoins = coinHudAnim->GetCurrentFrame();
	app->render->DrawTexture(imgCoin, point0.x, point0.y, &rectCoins);

	sprintf_s(coinText, 9, "x%d", *coins);
	app->fonts->BlitText(point0.x + rectCoins.w, point0.y + 12, hudFont, coinText);

	// FireBall
	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;
	point0.x = point0.x + 40;
	point0.y = point0.y + WINDOW_H - 120;

	if (*fireBallState == 0)
	{
		rectGUI = fireBallOnAnim->GetCurrentFrame();
		app->render->DrawTexture(fireBallTex, point0.x, point0.y,&rectGUI);
	}
	else
	{
		rectGUI = fireBallOffAnim->GetCurrentFrame();
		app->render->DrawTexture(fireBallTex, point0.x, point0.y, &rectGUI);
	}
	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;
	point0.x = point0.x + (WINDOW_W - 250);
	point0.y = point0.y + 20;
	// Time
	if (app->sceneManager->GetIsPause() && !stopTime)
	{
		auxTimePause.Start();
		stopTime = true;
	}
	if (!app->sceneManager->GetIsPause() && stopTime)
	{
		timer.startTime += auxTimePause.Read();
		stopTime = false;
	}
	point0.x = point0.x - 100;
	sprintf_s(timeText, 10, "%d:%02d:%02d", minuts, miliseconds / 100, miliseconds2);
	app->fonts->BlitText(point0.x, point0.y, hudFont, timeText);

	if (activeFPS)
	{
		point0.x += 170;
		point0.y += WINDOW_H - 100;
		sprintf_s(coinText, 10, "%3d", app->GetFramesOnLastSecond());

		app->fonts->BlitText(point0.x, point0.y, hudFont, coinText);

	}
	return true;
}

void GUI::Chronometer()
{
	if (miliseconds >= 60000 && !stopTime)
	{
		minuts++;
	}
	miliseconds = miliseconds * 0.1;

	int centenas = 0;
	if (miliseconds >= 100)
	{
		centenas = miliseconds / 100;
		miliseconds2 = miliseconds - (centenas * 100);
	}	
}

bool GUI::CleanUp()
{
	if (!active)
	{
		return true;
	}

	app->tex->UnLoad(arrowTex);
	app->tex->UnLoad(imgCoin);
	app->tex->UnLoad(fireBallTex);
	app->fonts->UnLoad(hudFont);

	delete headAnim;
	delete arrowAnim;
	delete buttonEAnim;
	delete fireBallOnAnim;
	delete fireBallOffAnim;
	delete coinHudAnim;

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
	if(!app->removeGame)data.child("time").attribute("value").set_value(miliseconds*10+minuts*60000);
	else data.child("time").attribute("value").set_value(0);
	return true;
}

