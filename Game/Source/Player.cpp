#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Audio.h"
#include "ModuleFadeToBlack.h"
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

	idleAnim = new Animation();
	walkAnim = new Animation();
	atakAnim = new Animation();
	damageAnim = new Animation();
	runAnim = new Animation();
	jumpAnim = new Animation();

	iPoint pathInit =  app->map->WorldToMap(positionInitial->x ,positionInitial->y) ;
	app->map->ResetPath(pathInit);

	playerData.texture = app->tex->Load("Assets/Textures/dino_green.png");
	playerData.position = *positionInitial;
	playerData.state = IDLE;
	playerData.velocity = 1;
	playerData.isJumped = false;
	playerData.direction = WALK_R;

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

	idleAnim->loop = true;
	idleAnim->speed = 0.05f;

	walkAnim->loop = true;
	walkAnim->speed = 0.08f;

	damageAnim->loop = false;
	damageAnim->speed = 0.005f;

	runAnim->loop = true;
	runAnim->speed = 0.10f;
	
	atakAnim->loop = false;
	atakAnim->speed = 0.10f;

	jumpAnim->loop = true;
	jumpAnim->speed = 0.24f;

	for (int i = 0; i < 4; i++)
		idleAnim->PushBack({ 78 * i,0, 78, 78 });

	for (int i = 0; i < 6; i++)
		jumpAnim->PushBack({ 312 + (78 * i),0, 78, 78 });

	for (int i = 0; i < 6; i++)
		walkAnim->PushBack({ 312 + (78 * i),0, 78, 78 });

	for (int i = 0; i < 2; i++)
		atakAnim->PushBack({ 858 + (78 * i),0, 78, 78 });

	for (int i = 0; i < 4; i++)
		damageAnim->PushBack({ 1008 + (78 * i),0, 78, 78 });

	for (int j = 0; j < 1; j++)
		for (int i = 2; i < 4; i++)
			damageAnim->PushBack({ 1008 + (78 * i),0, 78, 78 });

	deadAnim->PushBack({ 1008 + (78 * 1),0, 78, 78 });

	for (int i = 0; i < 4; i++)
		runAnim->PushBack({ 1319 + (78 * i),0, 78, 78 });


	   
	playerData.currentAnimation = idleAnim;
	velX = playerData.velocity;

	app->entityManager->AddEntity(FIREBALL, 0, 0);
	app->entityManager->AddEntity(HUD, 0, 0);

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
	return ret;
}

bool Player::SaveState(pugi::xml_node& player) const
{
	pugi::xml_node positionPlayer = player.child("position");
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
	}

	return true;
}

bool Player::PreUpdate() 
{
	return true;
}

bool Player::Update(float dt) 
{
	if (!app->sceneManager->GetIsPause())
	{
		if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
			DebugCP();
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && endUpdate && debugCheckPoints){
			endUpdate = false;
			DebugCP();
		}


		PlayerMoveAnimation();
		SpeedAnimationCheck(dt);
		playerData.velocity = floor(1000 * dt/ 4) ;
		gravity = ceil(600 * dt);

		MoveHit();
		GravityDown(dt);
		CameraPlayer();
		if (playerData.state == ATTACK && playerData.currentAnimation->HasFinished())
		{
			playerData.state = IDLE;
			atakAnim->Reset();
		}
		if (playerData.state != HIT && playerData.state != DEAD && playerData.state != ATTACK)
		{
			// Move player inputs control
			if (!checkpointMove)PlayerControls(dt);
			// Move Between CheckPoints
			else MoveBetweenCheckPoints();
		}
	}
	
	return true;
}

void Player::MoveHit()
{
	// DELETE IN RELEASE
	if (app->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)SetHit();
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) app->entityManager->score += 50;

	if (playerData.currentAnimation == damageAnim) 
	{
		if (!playerData.currentAnimation->HasFinished())
		{
			tmp = playerData.position;
			playerData.position.x -= 2 * playerData.direction;
			if (!jumpHit)
			{
				jumpHit = true;
				if (app->GetFramerate() == 60)	velY = -10.5f;
				else if (app->GetFramerate() == 30) velY = -21;
			}
			if (CollisionPlayer(playerData.position)) playerData.position = tmp;
		}
		else
		{	
			if(playerData.respawns <1)playerData.state = DEAD;
			else
			{
				playerData.position = IPointMapToWorld(checkPoints.end->data);
				damageAnim->Reset();
				playerData.state = IDLE;
			}
		}
	}
	if (playerData.currentAnimation->HasFinished())damageAnim->Reset();
}

void Player::GravityDown(float dt)
{
	if (godMode == false)
	{
		playerData.currentAnimation->Update();
		GravityDownCollision(dt);
	}
	else playerData.currentAnimation = idleAnim;
}

void Player::SpeedAnimationCheck(float dt)
{
	idleAnim->speed = (dt * 5) ;
	walkAnim->speed = (dt * 9) ;
	jumpAnim->speed = (dt * 18) ;
	atakAnim->speed = (dt * 5) ;
	damageAnim->speed = (dt * 10) ;
	runAnim->speed = (dt * 9) ;
	
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
	int followPositionPalyerX = (WINDOW_W / 2) + (playerData.position.x * -1);
	int followPositionPalyerY = (WINDOW_H / 2) + (playerData.position.y * -1) + 125;

	if (playerData.position.x < (WINDOW_W/2))
		if (app->render->camera.x < 48) followPositionPalyerX = 0;

	// Camera delimitation x
	if (app->render->camera.x <= (playerData.position.x * -1)&& app->render->camera.x >= -((app->map->data.width * app->map->data.tileWidth) - WINDOW_W))
		app->render->camera.x = followPositionPalyerX;
	else if (followPositionPalyerX<-1&& followPositionPalyerX>-((app->map->data.width * app->map->data.tileWidth) - WINDOW_W+32))
		app->render->camera.x = followPositionPalyerX;

	// Reposition right in limit camera in X
	if ((playerData.position.x *-1)< -((app->map->data.width * app->map->data.tileWidth) - (WINDOW_W/2) + 32))
		app->render->camera.x = -((app->map->data.width * app->map->data.tileWidth) - WINDOW_W + 32);

	if ((playerData.position.x *-1)> -(WINDOW_W/2) + 32)
		app->render->camera.x = 0;

	// Camera delimitation y
	if ((app->render->camera.y <= -48) && (app->render->camera.y >= -((app->map->data.height * app->map->data.tileHeight) - (WINDOW_H + (4 * app->map->data.tileHeight)))))
		app->render->camera.y = followPositionPalyerY;
	else if ((followPositionPalyerY < -48) && (followPositionPalyerY > -((app->map->data.height * app->map->data.tileHeight) - (WINDOW_H + (4 * app->map->data.tileHeight)))))
		app->render->camera.y = followPositionPalyerY;
	if (app->render->camera.y >= -39)
		app->render->camera.y = -39;
}

void Player::PlayerMoveAnimation()
{
	switch (playerData.state)
	{
	case IDLE:
		playerData.currentAnimation = idleAnim;
		break;

	case WALK:
		playerData.currentAnimation = walkAnim;
		break;

	case JUMP:
		playerData.currentAnimation = jumpAnim;
		break;

	case RUN:
		playerData.currentAnimation = runAnim;
		break;
	
	case HIT:
		playerData.currentAnimation = damageAnim;
		break;	
	
	case ATTACK:
		playerData.currentAnimation = atakAnim;
		break;

	default:
		break;
	}
}

void Player::PlayerControls(float dt)
{
	// Player Run
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT
		&& (playerData.state == State::WALK || playerData.state == State::RUN))
	{
		velX = playerData.velocity * 2;
		playerData.state = State::RUN;
	}
		// Comprobamos si las tecas est�n pulsadas al mismo tiempo
	if (!(app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		&& (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT))
	{
		if (playerData.state == State::IDLE || playerData.state == State::WALK) 
		{
			playerData.state = State::WALK;
			velX = playerData.velocity;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)MovePlayer(MoveDirection::WALK_R, dt);
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)MovePlayer(MoveDirection::WALK_L, dt);

	}	// Any key is pressed or A and D pressed in same time, set player in IDLE state
	else if(playerData.state != JUMP) playerData.state = State::IDLE;

	// Player Jump
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) Jump();

	if (godMode == true)
	{
		velX = playerData.velocity * 2;
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)playerData.position.y -= velX;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)playerData.position.y += velX;
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)playerData.position.x -= velX;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)playerData.position.x += velX;
	}

}

void Player::MovePlayer(MoveDirection playerDirection, float dt)
{
	tmp = playerData.position;
	playerData.direction = playerDirection;

	switch (playerData.state)
	{
	case IDLE:
		// Future conditions in state IDLE...
		break;	

	case JUMP:
		// Move in state JUMP 
		MoveToDirection(velX);
		// Future conditions in state JUMP...
		break;

	case WALK:
		// Move in state WALK 
		MoveToDirection(velX);
		// Future conditions in state WALK...
		break;

	case RUN:
		// Move in state RUN 
		MoveToDirection(velX);
		// Future conditions in state RUN...
		break;

	default:
		break;
	}

	if (CollisionPlayer(playerData.position))playerData.position = tmp;
	
}

void Player::MoveToDirection(int velocity)
{
	switch (playerData.direction)
	{	
	// Move in to correct direction
	case WALK_L:
		playerData.position.x -= velocity;
		break;

	case WALK_R:
		playerData.position.x += velocity;
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
	// Draw player in correct direction
	if (playerData.direction == MoveDirection::WALK_R )
		app->render->DrawTexture(playerData.texture, playerData.position.x -15, playerData.position.y - (rectPlayer.h - 10), &rectPlayer);
	else if (playerData.direction == MoveDirection::WALK_L)
		app->render->DrawTextureFlip(playerData.texture, playerData.position.x -15, playerData.position.y - (rectPlayer.h - 10), &rectPlayer);
	
	endUpdate = true;
	return true;
}

// Implements to gravity fall down
void Player::GravityDownCollision(float dt)
{

	tmp = playerData.position;	
	lastState = playerData.state;

	playerData.position.y += velY;

	iPoint PlayerCurrPos = { playerData.position.x  ,playerData.position.y};
	bool fallingCollision = false;
	bool feedCollision = false;
	for (int i = 0; i < playerData.numPoints; i++)
	{
		if (CollisionJumping({ PlayerCurrPos.x + playerData.pointsCollision[i].x ,PlayerCurrPos.y + playerData.pointsCollision[i].y }))
			fallingCollision = true;		
		if (fallingCollision && (i == 0 || i == 1))feedCollision = true;
	}
	if (fallingCollision)
	{	
		if (feedCollision)
		{
			jumpHit = false;
			playerData.isJumped = false;
			playerData.isJumpedAgain = false;
			if (playerData.state != JUMP)playerData.state = lastState;
			else playerData.state = IDLE;
		}
		playerData.position = tmp;
		velY = 0.0f;
		fallingCollision = false;
	}
	else{
		if(app->GetFramerate()==60)velY += 0.55f;
		else if(app->GetFramerate()==30) velY += 2.1f;
	}
}

bool Player::CleanUp() 
{
	if (!active)
		return true;

	app->audio->Unload1Fx(bonfireFx);
	app->audio->Unload1Fx(damageFx);
	
	app->tex->UnLoad(playerData.texture);
	active = false;
	pendingToDelete = true;

	delete idleAnim;
	delete walkAnim;
	delete atakAnim;
	delete damageAnim;
	delete runAnim;
	delete jumpAnim;

	checkPoints.Clear();
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
		// Concvert position player WorldToMap 
		positionMapPlayer = app->map->WorldToMap(x+playerData.pointsCollision[i].x, y+playerData.pointsCollision[i].y);
		if (CheckCollision(positionMapPlayer)== COLLISION) return true;
	}
	return false;
}

bool Player::CollisionJumping(iPoint nextPosition)
{
	iPoint positionMapPlayer;
	int y = nextPosition.y;
	positionMapPlayer = app->map->WorldToMap(nextPosition.x, nextPosition.y);
	if (CheckCollision(positionMapPlayer)== COLLISION) return true;
	return false;
}

void Player::Jump()
{
	lastState = playerData.state;
	if (playerData.isJumped && !playerData.isJumpedAgain)
	{
		// Generate second impulse
		if (app->GetFramerate() == 60)velY = -9.75f;
		else if (app->GetFramerate() == 30) velY = -19.50;
		playerData.isJumpedAgain = true;
	}
	if (!playerData.isJumped)
	{
		// Generate first impulse
		if (app->GetFramerate() == 60)	velY = -10.5f;
		else if (app->GetFramerate() == 30) velY = -21;
		playerData.isJumped = true;
	}
	playerData.state = State::JUMP;
}


bool Player::CheckGameOver(int level)
{
	if (playerData.state==DEAD)
	{
		playerData.currentAnimation = deadAnim;
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

