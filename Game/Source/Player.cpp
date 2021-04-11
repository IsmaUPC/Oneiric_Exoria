#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Audio.h"
#include "ModuleFadeToBlack.h"
#include "GuiMenuPause.h"
#include "SceneManager.h"

Player::Player() : Entity()
{
	name.Create("player");
}

Player::Player(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture) 
	: Entity(pTypeEntity, pPosition, pVelocity, pTexture)
{
	playerData.state = IDLE;
	name.Create("player");

}

Player::~Player()
{}

bool Player::Start()
{
	//Animations

	idleAnimR = new Animation();
	idleAnimL = new Animation();
	idleAnimUp = new Animation();
	idleAnimDown = new Animation();

	walkAnimR = new Animation();
	walkAnimL = new Animation();
	walkAnimUp = new Animation();
	walkAnimDown = new Animation();

	iPoint pathInit = app->map->WorldToMap(positionInitial->x, positionInitial->y);
	app->map->ResetPath(pathInit);

	//playerData.texture = app->tex->Load("Assets/Textures/Characters/kenzie.png");
	playerData.texture = app->tex->Load("Assets/Textures/Characters/keiler.png");
	playerData.position = *positionInitial;
	playerData.state = IDLE;
	playerData.velocity = 1;
	playerData.direction = WALK_R;
	lastDirection = playerData.direction;

	radiusCollision = app->entity->CalculateDistance(playerData.pointsCollision[0], playerData.pointsCollision[2]) / 2;
	playerData.centerPoint.x = app->entity->CalculateDistance(playerData.pointsCollision[0], playerData.pointsCollision[1]) / 2;
	playerData.centerPoint.y = app->entity->CalculateDistance(playerData.pointsCollision[0], playerData.pointsCollision[3]) / 2;

	bonfireFx = app->audio->LoadFx("Assets/Audio/Fx/bonfire.wav");
	damageFx = app->audio->LoadFx("Assets/Audio/Fx/damage.wav");

	playerData.respawns = 3;
	playerData.coins = 0;
	playerData.lives = 3;

	inCheckPoint = false;
	checkpointMove = false;
	endUpdate = true;
	win = false;
	debugCheckPoints = false;

	idleAnimL->loop = true;
	idleAnimL->speed = 0.05f;

	idleAnimR->loop = true;
	idleAnimR->speed = 0.05f;

	idleAnimUp->loop = true;
	idleAnimUp->speed = 0.05f;

	idleAnimDown->loop = true;
	idleAnimDown->speed = 0.05f;


	for (int i = 0; i < 6; i++)
	{
		idleAnimL->PushBack({ (32 * i) + (32 * 12), 78, 32, 50 });
		walkAnimL->PushBack({ (32 * i) + (32 * 12), 142, 32, 50 });
	}
	for (int i = 0; i < 6; i++)
	{
		idleAnimR->PushBack({ (32 * i) , 78, 32, 50 });
		walkAnimR->PushBack({ (32 * i) , 142, 32, 50 });
	}
	for (int i = 0; i < 6; i++)
	{
		idleAnimUp->PushBack({ (32 * i) + (32*6), 78, 32, 50 });
		walkAnimUp->PushBack({ (32 * i) + (32 * 6), 142, 32, 50 });
	}
	for (int i = 0; i < 6; i++)
	{
		idleAnimDown->PushBack({ (32*i) + (32 * 18), 78, 32, 50 });
		walkAnimDown->PushBack({ (32 * i) + (32 * 18), 142, 32, 50 });
	}
	   
	playerData.currentAnimation = idleAnimR;

	//Init position partner
	for (int i = 0; i < numPartners; i++)
	{
		partners[i].texture = app->tex->Load("Assets/Textures/Characters/brenda.png");
		partners[i].position.x = playerData.position.x - (40 * i) - 40;
		partners[i].position.y = playerData.position.y;
		partners[i].direction = WALK_R;
		partners[i].currentAnimation = idleAnimR;
		partners[i].breadcrumb = 0;
	}

	// Camera follow the player
	iPoint posCamera;
	posCamera.x = playerData.position.x - (WINDOW_W / 2);
	posCamera.y = playerData.position.y - (WINDOW_H / 2);

	posCamera = app->map->WorldToMap(posCamera);

	app->render->camera.x = -posCamera.x;
	app->render->camera.y = -posCamera.y;

	app->entityManager->AddEntity(HUD, app->render->camera.x, app->render->camera.y);
	
	return true;
}

bool Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player Parser");
	bool ret = true;
	
	return true;
}


bool Player::LoadState(pugi::xml_node& player) 
{
	bool ret=true;
	playerData.position.x = player.child("position").attribute("x").as_int(playerData.position.x);
	playerData.position.y = player.child("position").attribute("y").as_int(playerData.position.y);
	playerData.respawns = player.child("lives").attribute("num_respawns").as_int(playerData.respawns);
	playerData.coins = player.child("coins").attribute("count").as_int(playerData.coins);

	pugi::xml_node positionPartners = player.child("partners");
	partners[0].position.x = positionPartners.child("partner_1").attribute("x").as_int(partners[0].position.x);
	partners[1].position.x = positionPartners.child("partner_2").attribute("x").as_int(partners[1].position.x);
	partners[2].position.x = positionPartners.child("partner_3").attribute("x").as_int(partners[2].position.x);

	partners[0].position.y = positionPartners.child("partner_1").attribute("y").as_int(partners[0].position.y);
	partners[1].position.y = positionPartners.child("partner_2").attribute("y").as_int(partners[1].position.y);
	partners[2].position.y = positionPartners.child("partner_3").attribute("y").as_int(partners[2].position.y);

	return ret;
}

bool Player::SaveState(pugi::xml_node& player) const
{
	pugi::xml_node positionPlayer = player.child("position");
	pugi::xml_node positionPartners = player.child("partners");
	pugi::xml_node coinsPlayer = player.child("coins");
	pugi::xml_node respawnsPlayer = player.child("lives");

	if (app->removeGame)
	{
		if (app->sceneManager->lastLevel == 1)
		{
			positionPlayer.attribute("x").set_value(432);
			positionPlayer.attribute("y").set_value(1170);
		}
		else if (app->sceneManager->lastLevel == 2)
		{
			positionPlayer.attribute("x").set_value(576);
			positionPlayer.attribute("y").set_value(1534);
		}
		coinsPlayer.attribute("count").set_value(0);
		respawnsPlayer.attribute("num_respawns").set_value(3);
	}
	else
	{
		positionPlayer.attribute("x").set_value(playerData.position.x);
		positionPlayer.attribute("y").set_value(playerData.position.y);
		coinsPlayer.attribute("count").set_value(playerData.coins);
		respawnsPlayer.attribute("num_respawns").set_value(playerData.respawns);

		positionPartners.child("partner_1").attribute("x").set_value(partners[0].position.x);
		positionPartners.child("partner_2").attribute("x").set_value(partners[1].position.x);
		positionPartners.child("partner_3").attribute("x").set_value(partners[2].position.x);

		positionPartners.child("partner_1").attribute("y").set_value(partners[0].position.y);
		positionPartners.child("partner_2").attribute("y").set_value(partners[1].position.y);
		positionPartners.child("partner_3").attribute("y").set_value(partners[2].position.y);
	}

	return true;
}

bool Player::PreUpdate() 
{
	return true;
}

bool Player::Update(float dt) 
{
	playerData.currentAnimation->Update();

	if (!app->sceneManager->GetIsPause())
	{
		if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
			DebugCP();
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && endUpdate && debugCheckPoints){
			endUpdate = false;
			DebugCP();
		}


		SpeedAnimationCheck(dt);
		playerData.velocity = floor(1000 * dt/ 8) ;

		CameraPlayer();
		// Move player inputs control
		if (!checkpointMove)PlayerControls(dt);
		// Move Between CheckPoints
		else MoveBetweenCheckPoints();

		PlayerMoveAnimation(playerData.state, playerData.direction, playerData.currentAnimation);
		OffsetPartners();
	}
	
	return true;
}

void Player::OffsetPartners()
{
	if (path.Count() == 0)
	{
		int offsetX = abs(playerData.position.x - partners[0].position.x);
		int offsetY = abs(playerData.position.y - partners[0].position.y);
		switch (playerData.direction)
		{
		case WALK_R:
			if (offsetX > 40)
			{
				for (int i = 0; i < numPartners; i++)
				{
					partners[i].position.x++;
				}
			}
			break;
		case WALK_L:
			if (offsetX > 40)
			{
				for (int i = 0; i < numPartners; i++)
				{
					partners[i].position.x--;
				}
			}
			break;
		case WALK_UP:
			if (offsetY > 40)
			{
				for (int i = 0; i < numPartners; i++)
				{
					partners[i].position.y--;
				}
			}
			break;
		case WALK_DOWN:
			if (offsetY > 40)
			{
				for (int i = 0; i < numPartners; i++)
				{
					partners[i].position.y++;
				}
			}
			break;
		default:
			break;
		}
	}
}

void Player::SpeedAnimationCheck(float dt)
{
	idleAnimL->speed = (dt * 5) ;
	idleAnimR->speed = (dt * 5) ;
	idleAnimUp->speed = (dt * 5) ;
	idleAnimDown->speed = (dt * 5) ;

	walkAnimR->speed = (dt * 5);
	walkAnimL->speed = (dt * 5);
	walkAnimUp->speed = (dt * 5);
	walkAnimDown->speed = (dt * 5);

}

void Player::MoveBetweenCheckPoints()
{
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		if ((lastCP + 1) >= checkPoints.Count()) lastCP = 0;
		else lastCP++;
		playerData.position = IPointMapToWorld(checkPoints.At(lastCP)->data);
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		if (lastCP == 0) lastCP = checkPoints.Count() - 1;
		else lastCP--;
		playerData.position = IPointMapToWorld(checkPoints.At(lastCP)->data);
	}
}

void Player::CameraPlayer()
{
	// Camera follow the player
	int followPositionPalyerX = playerData.position.x - (WINDOW_W / 2);
	int followPositionPalyerY = playerData.position.y - (WINDOW_H / 2);

	app->render->camera.x = -followPositionPalyerX;
	app->render->camera.y = -followPositionPalyerY;
	/*if (playerData.position.x < (WINDOW_W/2))
		if (app->render->camera.x < 48) followPositionPalyerX = 0;*/

	//// Camera delimitation x
	//if (app->render->camera.x <= (playerData.position.x * -1)&& app->render->camera.x >= -((app->map->data.width * app->map->data.tileWidth) - WINDOW_W))
	//	app->render->camera.x = followPositionPalyerX;
	//else if (followPositionPalyerX<-1&& followPositionPalyerX>-((app->map->data.width * app->map->data.tileWidth) - WINDOW_W+32))
	//	app->render->camera.x = followPositionPalyerX;

	//// Reposition right in limit camera in X
	//if ((playerData.position.x *-1)< -((app->map->data.width * app->map->data.tileWidth) - (WINDOW_W/2) + 32))
	//	app->render->camera.x = -((app->map->data.width * app->map->data.tileWidth) - WINDOW_W + 32);

	//if ((playerData.position.x *-1)> -(WINDOW_W/2) + 32)
	//	app->render->camera.x = 0;

	//// Camera delimitation y
	//if ((app->render->camera.y <= -48) && (app->render->camera.y >= -((app->map->data.height * app->map->data.tileHeight) - (WINDOW_H + (4 * app->map->data.tileHeight)))))
	//	app->render->camera.y = followPositionPalyerY;
	//else if ((followPositionPalyerY < -48) && (followPositionPalyerY > -((app->map->data.height * app->map->data.tileHeight) - (WINDOW_H + (4 * app->map->data.tileHeight)))))
	//	app->render->camera.y = followPositionPalyerY;
	//if (app->render->camera.y >= -39)
	//	app->render->camera.y = -39;
}

void Player::AddBreadcrumb()
{
	iPoint* pos = new iPoint;
	pos->x = tmp.x;
	pos->y = tmp.y;
	path.Add(pos);
}

void Player::DeleteBreadcrumb(iPoint* pos)
{
	ListItem<iPoint*>* item;

	for (item = path.start; item != NULL; item = item->next)
	{
		if (item->data == pos)
		{
			path.Del(item);
		}
	}
}

void Player::PlayerMoveAnimation(State state, MoveDirection direction, Animation* &currentAnimation)
{
	switch (state)
	{
	case IDLE:
		switch (direction)
		{
		case WALK_L:
			currentAnimation = idleAnimL;
			break;

		case WALK_R:
			currentAnimation = idleAnimR;
			break;

		case WALK_UP:
			currentAnimation = idleAnimUp;
			break;

		case WALK_DOWN:
			currentAnimation = idleAnimDown;
			break;
		}
		break;

	case WALK:
		switch (direction)
		{
		case WALK_L:
			currentAnimation = walkAnimL;
			break;

		case WALK_R:
			currentAnimation = walkAnimR;
			break;

		case WALK_UP:
			currentAnimation = walkAnimUp;
			break;

		case WALK_DOWN:
			currentAnimation = walkAnimDown;
			break;
		}
		break;

	default:
		break;
	}
}

void Player::PlayerControls(float dt)
{
	diagonal = 0;
	GamePad& pad = app->input->pads[0];
	// Player Run
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT
		&& (playerData.state == State::WALK || playerData.state == State::RUN))
	{
		vel = playerData.velocity * 1.5;
		playerData.state = State::RUN;
	}
	// Comprobamos si las tecas están pulsadas al mismo tiempo
	if ((!(app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		&& (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)) 
		|| (pad.l_x < -0.2f || pad.l_x > 0.2f) || (pad.left == true || pad.right == true))
	{
		diagonal++;
		if (playerData.state == State::IDLE || playerData.state == State::WALK)
		{
			playerData.state = State::WALK;
			vel = playerData.velocity;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || pad.l_x > 0.2f || pad.right)MovePlayer(MoveDirection::WALK_R, dt);
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT || pad.l_x < -0.2f || pad.left)MovePlayer(MoveDirection::WALK_L, dt);
	}else playerData.state = State::IDLE;


	if ((!(app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		&& (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT))
		|| (pad.l_y < -0.2f || pad.l_y > 0.2f) || (pad.up == true || pad.down == true))
	{
		diagonal++;
		if (playerData.state == State::IDLE || playerData.state == State::WALK)
		{
			playerData.state = State::WALK;
			vel = playerData.velocity;
		}
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || pad.l_y < -0.2f || pad.up) MovePlayer(MoveDirection::WALK_UP, dt);
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT || pad.l_y > 0.2f || pad.down) MovePlayer(MoveDirection::WALK_DOWN, dt);
	}
	

	// If player change direction active boolean
	if (lastDirection != playerData.direction || diagonal == 2)
	{
		lastDirection = playerData.direction;
		AddBreadcrumb();
	}
	if(!playerCollision) MovePartners();

	if (godMode == true)
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)playerData.position.y -= vel;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)playerData.position.y += vel;
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)playerData.position.x -= vel;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)playerData.position.x += vel;
	}

}

void Player::MovePlayer(MoveDirection playerDirection, float dt)
{
	playerData.state = State::WALK;
	tmp = playerData.position;
	playerData.direction = playerDirection;

	switch (playerData.state)
	{
	case IDLE:
		// Future conditions in state IDLE...
		break;	

	case WALK:
	case RUN:
		// Move in state WALK 
		MoveToDirection(playerDirection, playerData.position);
		// Future conditions in state WALK...
		break;

	default:
		break;
	}

	if (CollisionPlayer(playerData.position))
	{
		playerData.position = tmp;
		if (diagonal == 2 && playerCollision == false)
			playerCollision = false;
		else playerCollision = true;
	}
	else // If player no collision partners can move
	{
		playerCollision = false;
	}
}

void Player::MovePartners()
{
	for (int i = 0; i < numPartners; i++)
	{
		if (path.Count() == 0 || path.At(partners[i].breadcrumb) == NULL)
		{
			partners[i].direction = playerData.direction;
			MoveToDirection(partners[i].direction, partners[i].position);
		}
		else
		{
			PartnerDirection(i);
			NextBreadcrumb(i);
		}
		PlayerMoveAnimation(playerData.state, partners[i].direction, partners[i].currentAnimation);
	}
	playerCollision = true;
}

void Player::PartnerDirection(int index)
{
	iPoint partnerPos = partners[index].position;
	int i = partners[index].breadcrumb;

	if (partnerPos.x < path.At(i)->data->x)
	{
		partners[index].direction = WALK_R;
		MoveToDirection(partners[index].direction, partners[index].position);
	}
	else if (partnerPos.x > path.At(i)->data->x)
	{
		partners[index].direction = WALK_L;
		MoveToDirection(partners[index].direction, partners[index].position);
	}
	if (partnerPos.y < path.At(i)->data->y)
	{
		partners[index].direction = WALK_DOWN;
		MoveToDirection(partners[index].direction, partners[index].position);
	}
	else if (partnerPos.y > path.At(i)->data->y)
	{
		partners[index].direction = WALK_UP;
		MoveToDirection(partners[index].direction, partners[index].position);
	}	
}

void Player::NextBreadcrumb(int index)
{
	iPoint partnerPos = partners[index].position;
	int i = partners[index].breadcrumb;

	if (partnerPos.x == path.At(i)->data->x && partnerPos.y == path.At(i)->data->y)
	{
		partners[index].breadcrumb++;
		if (index == 2)
		{
			DeleteBreadcrumb(path.start->data);
			for (int i = 0; i < numPartners; i++)
			{
				partners[i].breadcrumb--;
			}
		}
		//if (path.Count() != 0 && path.At(partners[index].breadcrumb) != NULL) PartnerDirection(index);
	}
}

void Player::MoveToDirection(MoveDirection direction, iPoint &position)
{
	switch (direction)
	{	
	// Move in to correct direction
	case WALK_L:
		position.x -= vel;
		break;

	case WALK_R:
		position.x += vel;
		break;

	case WALK_UP:
		position.y -= vel;
		break;

	case WALK_DOWN:
		position.y += vel;
		break;

	default:
		break;
	}
}

void Player::DebugCP()
{
	if (!checkpointMove) 
	{
		debugCheckPoints = true;
		checkpointMove = true;
		inCheckPoint = true;
		if (app->map->checKpointsMap.list.Count() != checkPoints.Count())
		{
			for (int i = 0; i < app->map->checKpointsMap.list.Count(); i++)
			{
				bool find = false;
				for (int j = 0; j < checkPoints.Count(); j++)
				{
					if (app->map->checKpointsMap.list.At(i)->data->pos == checkPoints.At(j)->data)
					{
						find = true;
					}
				}
				if (!find)
				{
					checkPoints.Add(app->map->checKpointsMap.list.At(i)->data->pos);
					app->map->checKpointsMap.list.At(i)->data->active = true;
				}
				
			}
		}
	}
	else 
	{
		debugCheckPoints = false;
		checkpointMove = false;
		inCheckPoint = false;
	}
}

iPoint Player::IPointMapToWorld(iPoint ipoint)
{
	iPoint CPos = app->map->MapToWorld(ipoint.x, ipoint.y);
	return CPos;
}

bool Player::PostUpdate() 
{
	SDL_Rect rectPlayer;
	rectPlayer = playerData.currentAnimation->GetCurrentFrame();

	// Draw partners
	for (int i = 0; i < numPartners; i++)
	{
		app->render->DrawTexture(partners[i].texture, partners[i].position.x, partners[i].position.y, &partners[i].currentAnimation->GetCurrentFrame());
	}

	// Draw player in correct direction
	app->render->DrawTexture(playerData.texture, playerData.position.x, playerData.position.y, &rectPlayer);

	// Paint walls above the player
	app->map->DrawUp();

	endUpdate = true;
	return true;
}

bool Player::CleanUp() 
{
	if (!active)
		return true;
	
	app->tex->UnLoad(playerData.texture);
	active = false;
	pendingToDelete = true;

	// Player
	delete positionInitial;
	positionInitial = nullptr;

	delete idleAnimL;
	delete idleAnimR;
	delete idleAnimUp;
	delete idleAnimDown;

	delete walkAnimL;
	delete walkAnimR;
	delete walkAnimUp;
	delete walkAnimDown;

	idleAnimL = nullptr;
	idleAnimR = nullptr;
	idleAnimUp = nullptr;
	idleAnimDown = nullptr;

	walkAnimL = nullptr;
	walkAnimR = nullptr;
	walkAnimUp = nullptr;
	walkAnimDown = nullptr;

	// Partners
	for (int i = 0; i < numPartners; i++)
	{
		app->tex->UnLoad(partners[i].texture);

		// Animations of each partner
		// ...
	}

	checkPoints.Clear();
	path.Clear();
	app->entityManager->DeleteHUD();

	app->input->ShakeController(0, 60, 0);

	return true;
}

bool Player::CollisionPlayer(iPoint nextPosition) 
{
	iPoint positionMapPlayer;
	int y = nextPosition.y;
	int x = nextPosition.x ;

	for (int i = 0; i < playerData.numPoints; i++)
	{	
		inCheckPoint = false;
		// Convert position player WorldToMap 
		positionMapPlayer = app->map->WorldToMap(x+playerData.pointsCollision[i].x, y+playerData.pointsCollision[i].y);
		if (CheckCollision(positionMapPlayer) == COLLISION) return true;
	}
	return false;
}

bool Player::CheckGameOver(int level)
{
	if (playerData.state==DEAD)
	{
		//playerData.currentAnimation = deadAnim;
		return true;
	}
	if (level == 1 && !godMode)
	{
		if (playerData.position.y > 1720)
		{
			playerData.respawns--;
			if (playerData.respawns < 1)playerData.state = DEAD;

			if (playerData.state != DEAD)
			{
				if (checkPoints.Count() == 0)
					playerData.position = *positionInitial;
				else
					playerData.position = IPointMapToWorld(checkPoints.end->data);
			}
		}
	}
	if (level == 2 && !godMode)
	{
		if (playerData.position.y > 1968)
		{
			playerData.respawns--;
			if (playerData.respawns < 1)playerData.state = DEAD;

			if (playerData.state != DEAD)
			{
				if (checkPoints.Count() == 0)
					playerData.position = *positionInitial;
				else
					playerData.position = IPointMapToWorld(checkPoints.end->data);
			}
		}
	}
	return false;
}

void Player::SetHit()
{
	if (playerData.respawns > 0 && playerData.state != HIT && !godMode) 
	{
		playerData.respawns--;
		playerData.state = HIT;
		hitDirection = playerData.direction;
		app->audio->PlayFx(damageFx);
	}
	
}

void Player::ActiveCheckpoint(iPoint positionMapPlayer)
{
	if (app->map->data.layers.At(2)->data->Get(positionMapPlayer.x, positionMapPlayer.y) == app->map->data.tilesets.At(2)->data->firstgid + 2)
	{
		for (int i = 0; i < checkPoints.Count(); i++)
		{
			if (checkPoints.At(i)->data == positionMapPlayer)
			{
				lastCP = i;
				if (checkPoints.Count() > 1)
				{
					inCheckPoint = true;
					if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && endUpdate)
					{
						endUpdate = false;
						checkpointMove = !checkpointMove;
					}
				}
				return;
			}
		}
		checkPoints.Add(positionMapPlayer);

		LOG("CHECKPOINT pos:%d,%d", positionMapPlayer.x, positionMapPlayer.y);
		app->map->CheckPointActive(positionMapPlayer);
		// FX
		app->audio->PlayFx(bonfireFx);
	}
}

