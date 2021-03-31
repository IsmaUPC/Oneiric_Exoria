#include "FireBall.h"
#include "Player.h"
#include "Audio.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

FireBall::FireBall() : Entity()
{
	name.Create("fire");
}
FireBall::FireBall(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture)
	: Entity(pTypeEntity, pPosition, pVelocity, pTexture)
{
	name.Create("fire");
}

FireBall::~FireBall()
{
}

bool FireBall::Awake(pugi::xml_node&)
{
	return true;
}

bool FireBall::Start()
{
	active = true;

	entityData->velocity = 12;


	startPos = { -100,-100 };
	entityData->position = startPos;
	entityData->texture = app->tex->Load("Assets/Textures/fire_ball.png");


	SDL_QueryTexture(entityData->texture, NULL, NULL, &texW, &texH);

	texW = texW / 5;

	fireBallAnim->speed = 0.3;
	for (int i = 0; i < 5; i++)
		fireBallAnim->PushBack({ texW * i, 0, texW, texH });

	explsionTex = app->tex->Load("Assets/Textures/fire_ball_explosion.png");
	int imgW, imgH;
	SDL_QueryTexture(explsionTex, NULL, NULL, &imgW, &imgH);
	imgW = imgW / 2;

	explosionAnim->loop = false;
	explosionAnim->speed = 0.65;
	for (int i = 0; i < 2; i++) 
		explosionAnim->PushBack({ imgW * i, 0, imgW, imgH });

	explosionAnim->PushBack({ 0, 0, 0, 0 });

	entityData->pointsCollision = new iPoint[4]{ { 0, 0 }, { texW , 0 }, { texW,-texH }, { 0 ,-texH } };

	hitFx = app->audio->LoadFx("Assets/Audio/Fx/hit.wav");
	shootFx = app->audio->LoadFx("Assets/Audio/Fx/fireball.wav");

	cooldown = 2;
	startexplosion = false;

	stateShoot = 0;
	app->player->SetStateShoot(&stateShoot);
	app->player->SetPositionShoot(&entityData->position);
	app->player->SetCollidersShoot(entityData->pointsCollision);
	return true;
}

bool FireBall::PreUpdate()
{
	return true;
}

bool FireBall::Update(float dt)
{
	
	if ((app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN 
		|| app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN) && !app->player->godMode)
	{
		Shoot();
	}

	if (lastState == SHOOT && stateShoot == WAIT)
	{
		explosionAnim->Reset();
		explosionPos = entityData->position;
		app->audio->PlayFx(hitFx);
		BackToPos0();
	}

	switch (stateShoot)
	{
	case CAN_SHOOT:
		return true;
		break;
	case SHOOT:

		fireBallAnim->Update();
		if (direc == MoveDirection::WALK_R)
			entityData->position.x += entityData->velocity;
		else
			entityData->position.x -= entityData->velocity;

		if (frameTime.ReadSec()>=cooldown && !stopTime)
		{
			BackToPos0();
		}
		break;
	case WAIT:
		if (frameTime.ReadSec() >=cooldown && !stopTime)
		{
			BackToPos0();
			stateShoot = CAN_SHOOT;
			
			entityData->fireBallState = stateShoot;
		}
		break;
	default:
		break;
	}
	lastState = stateShoot;

	if (startexplosion)
		explosionAnim->Update();

	if (CheckCollision(app->map->WorldToMap(entityData->position.x, entityData->position.y)) == COLLISION)*app->player->playerData.stateShoot = 2;
	if (CheckCollision(app->map->WorldToMap(entityData->position.x+13, entityData->position.y)) == COLLISION)*app->player->playerData.stateShoot = 2;
	if (startexplosion)
	{
		if (explosionAnim->HasFinished()) 
		{
			startexplosion = false;
		}
	}
	return true;
}

bool FireBall::PostUpdate()
{
	if (app->sceneManager->GetIsPause() && !stopTime)
	{
		auxTimePause.Start();
		stopTime = true;
	}
	if (!app->sceneManager->GetIsPause() && stopTime)
	{
		frameTime.startTime += auxTimePause.Read();
		stopTime = false;
	}
	SDL_Rect rect;
	rect = fireBallAnim->GetCurrentFrame();
	if (direc== MoveDirection::WALK_R)
		app->render->DrawTexture(entityData->texture, entityData->position.x, entityData->position.y, &rect);	
	else
		app->render->DrawTextureFlip(entityData->texture, entityData->position.x, entityData->position.y, &rect);

	if (startexplosion)
	{
		
		rect = explosionAnim->GetCurrentFrame();

		app->render->DrawTexture(explsionTex, explosionPos.x, explosionPos.y, &rect);

	}

	return true;
}

bool FireBall::CleanUp()
{
	if (!active)
		return true;

	app->audio->Unload1Fx(hitFx);
	app->audio->Unload1Fx(shootFx);
	app->tex->UnLoad(entityData->texture);

	active = false;
	pendingToDelete = true;

	return true;
}

void FireBall::SetPosition(iPoint pos)
{
	entityData->position.x = pos.x;
	entityData->position.y = pos.y;
}

void FireBall::BackToPos0()
{
	startexplosion = true;
	entityData->position = startPos;
	stateShoot = WAIT;
	entityData->fireBallState = stateShoot;
}

void FireBall::Shoot()
{
	if (stateShoot == CAN_SHOOT)
	{
		app->audio->PlayFx(shootFx);
		app->player->playerData.state = ATTACK;
		stateShoot = SHOOT;
		entityData->fireBallState = stateShoot;
		frameTime.Start();
		entityData->position = app->player->playerData.position;
		entityData->position.y -= 36;
		entityData->position.x += (5* app->player->playerData.direction);
		direc = app->player->playerData.direction;
	}
}
