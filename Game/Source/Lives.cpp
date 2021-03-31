#include "App.h"
#include "Lives.h"
#include "Audio.h"
#include "Player.h"
#include "EntityManager.h"



Lives::Lives(iPoint pos) : Entity()
{
	name.Create("lives");

	position = pos;
}
Lives::Lives(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture, int dropScore, uint deadFx)
	: Entity(pTypeEntity, pPosition, pVelocity, pTexture, dropScore, deadFx)
{
	name.Create("lives");
	position = pPosition;
}

Lives::~Lives()
{}
 
bool Lives::Start()
{
	liveAnimation = new Animation();
	particleAnimation = new Animation();
	entityData->currentAnimation = new Animation();
	entityData->state = IDLE;
	active = true;

	texLive = entityData->texture;
	liveFx = entityData->deadFx;

	numPoints = 4;
	pointsCollision = new iPoint[4]{ { 4, 0 }, { 40 ,0 }, { 32,-26 }, { 4 ,-26 } };

	liveAnimation->loop = true;
	liveAnimation->speed = 0.06f;

	for (int i = 0; i < 16; i++)
		liveAnimation->PushBack({ (44 * i),0, 44, 32 });


	particleAnimation->loop = false;
	particleAnimation->speed = 0.06f;

	for (int i = 0; i < 6; i++)
		particleAnimation->PushBack({ (64 * i),32, 64, 64 });

	return true;
}

bool Lives::Awake(pugi::xml_node& config)
{
	LOG("Loading Lives Parser");
	bool ret = true;


	return ret;
}

bool Lives::PreUpdate()
{
	CurrentLiveAnimation();
	iPoint currentPositionPlayer = app->player->playerData.position;
	iPoint auxPositionLive[4];

	if (entityData->state == DEAD)
	{
		isCollected = true;
		pendingToDelete = true;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			auxPositionLive[i] = { position.x + pointsCollision[i].x,
				position.y + pointsCollision[i].y };
		}
		iPoint auxPositionPlayer[6];
		for (int i = 0; i < 6; i++)
		{
			auxPositionPlayer[i] = { currentPositionPlayer.x + app->player->playerData.pointsCollision[i].x,
				-48 + currentPositionPlayer.y + app->player->playerData.pointsCollision[i].y };
		}
		if (collision.IsInsidePolygons(auxPositionPlayer, app->player->playerData.numPoints, auxPositionLive, numPoints)
			&& collision.IsInsidePolygons(auxPositionLive, numPoints, auxPositionPlayer, app->player->playerData.numPoints) && entityData->state == IDLE)
		{
			entityData->state = DEADING;
			app->entityManager->score += entityData->dropScore;
			app->audio->PlayFx(liveFx);
			app->player->LivePlus();
		}
		if (entityData->state == DEADING && entityData->currentAnimation->HasFinished())
			entityData->state = DEAD;
	}
	return false;
}

bool Lives::Update(float dt)
{
	entityData->currentAnimation->Update();
	entityData->currentAnimation->speed = (dt * 13);
	return true;
}

bool Lives::PostUpdate()
{
	SDL_Rect rectLives;
	rectLives = entityData->currentAnimation->GetCurrentFrame();
	app->render->DrawTexture(texLive, position.x, position.y, &rectLives);

	return true;
}

bool Lives::CleanUp()
{
	if (!active)
		return true;
	pendingToDelete = true;

	active = false;

	return true;
}

void Lives::CurrentLiveAnimation()
{
	switch (entityData->state)
	{
	case IDLE:
		entityData->currentAnimation = liveAnimation;
		break;

	case DEADING:
		entityData->currentAnimation = particleAnimation;
		break;

	default:
		break;
	}
}