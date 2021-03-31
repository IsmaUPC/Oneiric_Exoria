#include "App.h"
#include "Coins.h"
#include "Audio.h"
#include "Player.h"



Coins::Coins(iPoint pos) : Entity()
{
	name.Create("coins");

	position = pos;
}
Coins::Coins(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture)
	: Entity(pTypeEntity, pPosition, pVelocity, pTexture)
{
	name.Create("coins");
	position = pPosition;

}

Coins::~Coins()
{}

bool Coins::Start()
{

	coinAnimation = new Animation();
	particleAnimation = new Animation();
	entityData->currentAnimation= new Animation();
	entityData->state = IDLE;
	active = true;

	texCoin = entityData->texture;
	coinFx= app->audio->LoadFx("Assets/Audio/Fx/coin.wav");
	
	numPoints = 4;
	pointsCollision = new iPoint[4]{ { 0, 0 }, { 48 , 0 }, { 48,-48 }, { 0 ,-48 } };
	
	coinAnimation->loop = true;
	coinAnimation->speed = 0.20f;
	
	for (int i = 0; i < 16; i++)
		coinAnimation->PushBack({ 0,(40 * i), 40, 40 });


	particleAnimation->loop = false;
	particleAnimation->speed = 0.20f;
	
	for (int i = 0; i < 7; i++)
		particleAnimation->PushBack({ 40,(60 * i), 60, 60 });

	return true;
}


bool Coins::Awake(pugi::xml_node& config)
{
	LOG("Loading Coins Parser");
	bool ret = true;


	return ret;
}

bool Coins::PreUpdate()
{
	CurrentCoinAnimation();
	iPoint currentPositionPlayer = app->player->playerData.position;
	iPoint auxPositionCoin[4];

	if (entityData->state == DEAD)
	{
		isCollected = true;
		pendingToDelete = true;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			auxPositionCoin[i] = { position.x + pointsCollision[i].x,
				position.y + pointsCollision[i].y };
		}
		iPoint auxPositionPlayer[6];
		for (int i = 0; i < 6; i++)
		{
			auxPositionPlayer[i] = { currentPositionPlayer.x + app->player->playerData.pointsCollision[i].x,
				-48 + currentPositionPlayer.y + app->player->playerData.pointsCollision[i].y };

		}
		if (collision.IsInsidePolygons(auxPositionPlayer, app->player->playerData.numPoints, auxPositionCoin, numPoints)
			&& collision.IsInsidePolygons(auxPositionCoin, numPoints, auxPositionPlayer, app->player->playerData.numPoints) && entityData->state == IDLE)
		{
			entityData->state = DEADING;
			app->audio->PlayFx(coinFx);
			app->player->CoinPlus();
		}
		if (entityData->state == DEADING && entityData->currentAnimation->HasFinished())
			entityData->state = DEAD;
	}
	return false;
}

bool Coins::Update(float dt)
{
	entityData->currentAnimation->Update();
	entityData->currentAnimation->speed = (dt * 9);

	return true;
}
bool Coins::PostUpdate()
{

	SDL_Rect rectCoins;
	rectCoins = entityData->currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texCoin, position.x, position.y, &rectCoins);

	return true;

}

bool Coins::CleanUp()
{
	if (!active)
		return true;

	app->audio->Unload1Fx(coinFx);


	delete coinAnimation;
	delete particleAnimation;
	delete pointsCollision;

	pendingToDelete = true;
	active = false;

	return true;
}
void Coins::CurrentCoinAnimation()
{
	switch (entityData->state)
	{
	case IDLE:
		entityData->currentAnimation = coinAnimation;
		break;

	case DEADING:
		entityData->currentAnimation = particleAnimation;
		break;

	default:
		break;
	}
}