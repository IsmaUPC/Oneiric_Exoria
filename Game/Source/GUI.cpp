#include "GUI.h"
#include "Player.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "Fonts.h"
#include "DialogSystem.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

GUI::GUI() : Entity()
{
	active = true;
	name.Create("GUI");
}

GUI::GUI(Entity* entity, SDL_Texture* pTexture)
{
	name.Create("GUI");
	entityData = entity->entityData;
	entityData.texture = pTexture;
	headTex = pTexture;
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
	coinHudAnim = new Animation();

	active = true;
	int imgH = 0;
	int imgW = 0;

	//headTex = entityData.texture;
	SDL_QueryTexture(headTex, NULL, NULL, &headW, &imgH);

	//headAnim->PushBack({0,0,headW,imgH });

	imgH = 0;
	imgW = 0;
	//arrowTex = app->tex->Load("Assets/Textures/GUI/arrows.png");
	//SDL_QueryTexture(arrowTex, NULL, NULL, &imgW, &imgH);
	imgW = imgW / 2;

	//arrowAnim->PushBack({ 0,0,imgW,imgH });
	//buttonEAnim->PushBack({ imgW,0,imgW,imgH });


	//imgCoin = app->tex->Load("Assets/Textures/GUI/coin.png");
	coinHudAnim->loop = true;
	coinHudAnim->speed = 0.20f;

	for (int i = 0; i < 16; i++)
		coinHudAnim->PushBack({ 0,(80 * i), 80, 80 });

	respawn = &app->player->playerData.respawns;
	coins = &app->player->playerData.coins;

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
	entityData.position.x = -app->render->camera.x;
	entityData.position.y = -app->render->camera.y;

	return true;
}

bool GUI::PostUpdate()
{
	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;
	
	point0.x = point0.x + headPositionX;
	point0.y = point0.y + headPositionY;

	//text box
	if (app->dialogueSystem->onDialog == true) app->render->DrawTextBox(-app->render->camera.x + WINDOW_W / 2 - 300, -app->render->camera.y + 565, 600, 150, { 24, 61, 172 }, { 97, 159, 207 }, { 0, 33, 78 }, app->guiManager->moonCorner, 200);

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
	//app->fonts->BlitText(point0.x, point0.y, 0, scoreText, { 255, 255, 255 });

	// Coin HUD
	point0.x = point0.x - 20;
	point0.y = point0.y + 70;

	SDL_Rect rectCoins;
	rectCoins = coinHudAnim->GetCurrentFrame();
	//app->render->DrawTexture(imgCoin, point0.x, point0.y, &rectCoins);

	sprintf_s(coinText, 9, "x%d", *coins);
	//app->fonts->BlitText(point0.x + rectCoins.w, point0.y + 12, 0, coinText, { 255, 255, 255 });
	

	// Time
	point0.x = -app->render->camera.x;
	point0.y = -app->render->camera.y;
	point0.x = point0.x + (WINDOW_W - 250);
	point0.y = point0.y + 20;
	
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
	//app->fonts->BlitText(point0.x, point0.y, 0, timeText, { 255, 255, 255 });

	if (activeFPS)
	{
		point0.x += 170;
		point0.y += WINDOW_H - 100;
		sprintf_s(coinText, 10, "%3d", app->GetFramesOnLastSecond());

		app->fonts->BlitText(point0.x, point0.y, 0, coinText, { 255, 255, 255 });

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

	RELEASE(headAnim);
	RELEASE(arrowAnim);
	RELEASE(buttonEAnim);
	RELEASE(coinHudAnim);

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

