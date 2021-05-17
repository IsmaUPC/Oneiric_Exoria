#include "Player.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Audio.h"
#include "ModuleFadeToBlack.h"
#include "GuiMenuPause.h"
#include "SceneManager.h"
#include "DialogSystem.h"
#include "GuiManager.h"

Player::Player() : Entity()
{
	name.Create("player");
	itemManager = new ItemManager();
	entityData.type = KENZIE_;
	partners[0].entityData.type = KEILER_;
	partners[1].entityData.type = ISRRA_;
	partners[2].entityData.type = BRENDA_;
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
	itemManager->LoadItems(itemsFile, LIST_ITEMS_FILENAME);

	idleAnimR = new Animation();
	idleAnimL = new Animation();
	idleAnimUp = new Animation();
	idleAnimDown = new Animation();

	bookAnim = new Animation();

	walkAnimR = new Animation();
	walkAnimL = new Animation();
	walkAnimUp = new Animation();
	walkAnimDown = new Animation();

	iPoint pathInit = app->map->WorldToMap(positionInitial->x, positionInitial->y);
	app->map->ResetPath(pathInit);

	// Load texPartners of Characters
	LoadTexCharacters();

	playerData.position = *positionInitial;
	playerData.state = playerData.state;
	playerData.velocity = 1;
	
	playerData.direction = playerData.direction;
	lastDirection = playerData.direction;

	if (play)
	{
		playerData.level = 1;
		playerData.exp = 0;
		playerData.health = 8;
		playerData.mana = 11;

		stats.health = 8;
		stats.maxHealth = stats.health;
		stats.mana = 11;
		stats.maxMana = stats.mana;
		stats.attack = 3.5 + 9.5;
		stats.defense = 1.5 + 6.5;
		stats.speed = 2.5 + 7.5;
	}

	radiusCollision = app->entity->CalculateDistance(playerData.pointsCollision[0], playerData.pointsCollision[2]) / 2;
	playerData.centerPoint.x = app->entity->CalculateDistance(playerData.pointsCollision[0], playerData.pointsCollision[1]) / 2;
	playerData.centerPoint.y = app->entity->CalculateDistance(playerData.pointsCollision[0], playerData.pointsCollision[3]) / 2;

	playerData.respawns = 3;
	playerData.coins = 0;

	inCheckPoint = false;
	checkpointMove = false;
	endUpdate = true;
	win = false;
	debugCheckPoints = false;

	idleAnimL->speed = 0.05f;
	idleAnimR->speed = 0.05f;
	idleAnimUp->speed = 0.05f;
	idleAnimDown->speed = 0.05f;

	bookAnim->speed = 0.05f;
	bookAnim->loop = false;

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
		idleAnimUp->PushBack({ (32 * i) + (32 * 6), 78, 32, 50 });
		walkAnimUp->PushBack({ (32 * i) + (32 * 6), 142, 32, 50 });
	}
	for (int i = 0; i < 6; i++)
	{
		idleAnimDown->PushBack({ (32 * i) + (32 * 18), 78, 32, 50 });
		walkAnimDown->PushBack({ (32 * i) + (32 * 18), 142, 32, 50 });
	}
	for (int i = 0; i < 9; i++)
	{
		bookAnim->PushBack({ 32 * i, 400, 32, 48 });
	}
	   
	playerData.currentAnimation = idleAnimR;

	LoadPartners();

	// Camera follow the player
	/*iPoint posCamera;
	posCamera.x = playerData.position.x - (WINDOW_W / 2);
	posCamera.y = playerData.position.y - (WINDOW_H / 2);

	posCamera = app->map->WorldToMap(posCamera);

	app->render->camera.x = -posCamera.x;
	app->render->camera.y = -posCamera.y;*/

	lerpCamera.x = playerData.position.x;
	lerpCamera.y = playerData.position.y;

	app->entityManager->AddEntity(HUD, app->render->camera.x, app->render->camera.y, 0);

	fxBookOpen = app->audio->LoadFx("Audio/Fx/open_book.wav");
	fxStairs = app->audio->LoadFx("Audio/Fx/stairs.wav");
	
	return true;
}

void Player::LoadTexCharacters()
{

	playerData.texture = app->tex->Load("Textures/Characters/kenzie.png");
	texPartners.Add(app->tex->Load("Textures/Characters/keiler.png"));
	texPartners.Add(app->tex->Load("Textures/Characters/isrra.png"));
	texPartners.Add(app->tex->Load("Textures/Characters/brenda.png"));
}

void Player::LoadPartners()
{
	//Init position partner
	for (int i = 0; i < numPartners; i++)
	{
		partners[i].texture = texPartners.At(i)->data;

		partners[i].breadcrumb = 0;
		if (play)
		{
			partners[i].level = 1;
			partners[i].exp = 0;
			if (i == 0) partners[i].health = 14, partners[i].mana = 10;
			if (i == 1) partners[i].health = 10, partners[i].mana = 12;
			if (i == 2) partners[i].health = 13, partners[i].mana = 8;

			partners[i].stats.maxHealth = partners[i].health;
			partners[i].stats.health = partners[i].health;
			partners[i].stats.maxMana = partners[i].mana;
			partners[i].stats.mana = partners[i].mana;
			switch (i)
			{
			case 0:
				partners[i].stats.attack = 1.5 * partners[i].level + 6.5;
				partners[i].stats.defense = 1.5 * partners[i].level + 5.5;
				partners[i].stats.speed = 2.5 * partners[i].level + 8.5;
				break;
			case 1:
				partners[i].stats.attack = 2 * partners[i].level + 7;
				partners[i].stats.defense = 2 * partners[i].level + 7;
				partners[i].stats.speed = 2.5 * partners[i].level + 7.5;
				break;
			case 2:
				partners[i].stats.attack = 1.5 * partners[i].level + 6.5;
				partners[i].stats.defense = 3.5 * partners[i].level + 9.5;
				partners[i].stats.speed = 1.5 * partners[i].level + 6.5;
				break;
			default:
				break;
			}
		}
		if (i == 0)partners[i].type = KEILER;
		else if (i == 1)partners[i].type = ISRRA;
		else partners[i].type = BRENDA;		
	}
	play = false;
	load = true;
}

void Player::RePositionPartners()
{
	int x;
	for (int i = 0; i < numPartners; i++)
	{
		switch (playerData.direction)
		{
		case WALK_L:

			partners[i].position.x = playerData.position.x + (40 * i) + 40;
			partners[i].position.y = playerData.position.y;
				break;
		case WALK_R:
			partners[i].position.x = playerData.position.x - (40 * i) - 40;
			partners[i].position.y = playerData.position.y;
			break;
		case WALK_UP:
			partners[i].position.x = playerData.position.x ;
			partners[i].position.y = playerData.position.y+ ((40 * i) + 40);
			break;
		case WALK_DOWN:
			partners[i].position.x = playerData.position.x;
			partners[i].position.y = playerData.position.y - (40 * i) - 40;
			break;
		default:
			break;
		}

		partners[i].state = playerData.state;
		partners[i].direction = playerData.direction;
		partners[i].currentAnimation = playerData.currentAnimation;

	}
}

bool Player::Awake(pugi::xml_node& config)
{
	LOG("Loading Player Parser");
	bool ret = true;
	
	return true;
}


bool Player::LoadState(pugi::xml_node& player) 
{
	bool ret = true;
	
	if (loadStats)
	{
		playerData.level = player.child("data").attribute("level").as_int(playerData.level);
		playerData.exp = player.child("data").attribute("exp").as_int(playerData.exp);
		playerData.health = player.child("data").attribute("health").as_int(playerData.health);
		playerData.mana = player.child("data").attribute("mana").as_int(playerData.mana);
		stats.attack = player.child("data").attribute("attack").as_int(stats.attack);
		stats.defense = player.child("data").attribute("defense").as_int(stats.defense);
		stats.speed = player.child("data").attribute("speed").as_int(stats.speed);
		stats.health = playerData.health;
		stats.maxHealth = 2 * playerData.level + 6;
		stats.mana = playerData.mana;
		stats.maxHealth = 3 * playerData.level + 8;
		loadStats = false;
	}
	if (!app->sceneManager->GetLoseBattle())
	{
		playerData.position.x = player.child("data").attribute("x").as_int(playerData.position.x);
		playerData.position.y = player.child("data").attribute("y").as_int(playerData.position.y);
		playerData.direction = (MoveDirection)player.child("data").attribute("direction").as_int(playerData.direction);
	}

	playerData.respawns = player.child("lives").attribute("num_respawns").as_int(playerData.respawns);
	playerData.coins = player.child("coins").attribute("count").as_int(playerData.coins);

	pugi::xml_node positionPartners = player.child("partners").child("partner");
	int i = 0;
	while (positionPartners)
	{
		if (app->sceneManager->GetCurrentScene()->isContinue)
		{
			partners[i].level = positionPartners.attribute("level").as_int();
			partners[i].exp = positionPartners.attribute("exp").as_int();
			partners[i].health = positionPartners.attribute("health").as_int();
			partners[i].mana = positionPartners.attribute("mana").as_int();
			partners[i].stats.attack = positionPartners.attribute("attack").as_int();
			partners[i].stats.defense = positionPartners.attribute("defense").as_int();
			partners[i].stats.speed = positionPartners.attribute("speed").as_int();
			switch (i)
			{
			case 0:
				partners[i].stats.maxHealth = 4 * partners[i].level + 10;
				partners[i].stats.maxMana = 2.5 * partners[i].level + 7.5;
				break;
			case 1:
				partners[i].stats.maxHealth = 2.5 * partners[i].level + 7.5;
				partners[i].stats.maxMana = 3.5 * partners[i].level + 8.5;
				break;
			case 2:
				partners[i].stats.maxHealth = 3.5 * partners[i].level + 9.5;
				partners[i].stats.maxMana = 1.5 * partners[i].level + 6.5;
				break;
			default:
				break;
			}
		}
		if (!app->sceneManager->GetLoseBattle())
		{
			partners[i].position.x = positionPartners.attribute("x").as_int();
			partners[i].position.y = positionPartners.attribute("y").as_int();
			partners[i].direction = (MoveDirection)positionPartners.attribute("direction").as_int();
			partners[i].breadcrumb = positionPartners.attribute("breadcrumb").as_int();
		}
		else partners[i].breadcrumb = 0;

		positionPartners = positionPartners.next_sibling();
		i++;
	}

	path.Clear();
	pugi::xml_node breadcrumbs = player.child("path").first_child();
	if (!app->sceneManager->GetLoseBattle())
	{
		while (breadcrumbs)
		{
			iPoint* pos = new iPoint;
			pos->x = breadcrumbs.attribute("x").as_int();
			pos->y = breadcrumbs.attribute("y").as_int();
			path.Add(pos);

			breadcrumbs = breadcrumbs.next_sibling();
		}
	}	

	app->sceneManager->SetLoseBattle(false);
	return ret;
}

bool Player::SaveState(pugi::xml_node& player) const
{
	pugi::xml_node positionPlayer = player.child("data");
	pugi::xml_node coinsPlayer = player.child("coins");
	pugi::xml_node respawnsPlayer = player.child("lives");

	player.remove_child("path");
	player.append_child("path").set_value(0);
	player.remove_child("partners");
	player.append_child("partners").set_value(0);

	pugi::xml_node partnersData = player.child("partners");

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
			positionPlayer.attribute("direction").set_value(0);
		}
		coinsPlayer.attribute("count").set_value(0);
		respawnsPlayer.attribute("num_respawns").set_value(3);
	}
	else
	{
		positionPlayer.attribute("x").set_value(playerData.position.x);
		positionPlayer.attribute("y").set_value(playerData.position.y);
		positionPlayer.attribute("direction").set_value(playerData.direction);
		coinsPlayer.attribute("count").set_value(playerData.coins);
		respawnsPlayer.attribute("num_respawns").set_value(playerData.respawns);

		for (int i = 0; i < numPartners; i++)
		{
			partnersData.append_child("partner").append_attribute("x").set_value(partners[i].position.x);
			partnersData.last_child().append_attribute("y").set_value(partners[i].position.y);
			partnersData.last_child().append_attribute("breadcrumb").set_value(partners[i].breadcrumb);
			partnersData.last_child().append_attribute("direction").set_value(partners[i].direction);
		}

		SaveLevel(player);

		partnersData = player.child("path");
		for (int i = 0; i < path.Count(); i++)
		{
			partnersData.append_child("breadcrumb").append_attribute("x").set_value(path.At(i)->data->x);
			partnersData.last_child().append_attribute("y").set_value(path.At(i)->data->y);
		}		
	}

	return true;
}

bool Player::UpdatePlayerStats(Entity* entity, TypeEntity type)
{
	switch (type)
	{
	case KENZIE_:
		playerData.health = entity->stats.health;
		playerData.mana = entity->stats.mana;
		break;
	case KEILER_:
		partners[0].health = entity->stats.health;
		partners[0].mana = entity->stats.mana;
		break;
	case ISRRA_:
		partners[1].health = entity->stats.health;
		partners[1].mana = entity->stats.mana;
		break;
	case BRENDA_:
		partners[2].health = entity->stats.health;
		partners[2].mana = entity->stats.mana;
		break;
	default:
		break;
	}
	app->guiManager->GetStatsMenu()->InicializeStats();
	return false;
}

bool Player::SaveLevel(pugi::xml_node& player) const
{
	pugi::xml_node levelPlayer = player.child("data");
	pugi::xml_node partnersData = player.child("partners").child("partner");

	if (app->removeGame) levelPlayer.attribute("value").set_value(1);
	else
	{
		levelPlayer.attribute("level").set_value(playerData.level);
		levelPlayer.attribute("exp").set_value(playerData.exp);
		levelPlayer.attribute("health").set_value(playerData.health);
		levelPlayer.attribute("mana").set_value(playerData.mana);
		levelPlayer.attribute("attack").set_value(stats.attack);
		levelPlayer.attribute("defense").set_value(stats.defense);
		levelPlayer.attribute("speed").set_value(stats.speed);
		for (int i = 0; i < numPartners; i++)
		{
			partnersData.append_attribute("level").set_value(partners[i].level);
			partnersData.append_attribute("exp").set_value(partners[i].exp);
			partnersData.append_attribute("health").set_value(partners[i].health);
			partnersData.append_attribute("mana").set_value(partners[i].mana);
			partnersData.append_attribute("attack").set_value(partners[i].stats.attack);
			partnersData.append_attribute("defense").set_value(partners[i].stats.defense);
			partnersData.append_attribute("speed").set_value(partners[i].stats.speed);
			partnersData = partnersData.next_sibling();
		}
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

	if (bookAnim->HasFinished())
	{
		app->audio->PlayFx(fxBookOpen);
		app->guiManager->GetStatsMenu()->introBook = true;
		app->guiManager->GetStatsMenu()->currentAnim = app->guiManager->openBookAnim;
		app->guiManager->GetStatsMenu()->AbleDisableMenu();
		bookAnim->Reset();
		playerData.state = IDLE;
	}

	if (!app->sceneManager->GetIsPause())
	{
		/*if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
			DebugCP();
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && endUpdate && debugCheckPoints){
			endUpdate = false;
			DebugCP();
		}*/

		SpeedAnimationCheck(dt);
		playerData.velocity = floor(1000 * dt/ 8) ;

		CameraPlayer(dt);
		// Move player inputs control
		if (!checkpointMove && !app->sceneManager->GetEnemeyDetected() && app->dialogueSystem->onDialog == false)PlayerControls(dt);
		// Move Between CheckPoints

		for (int i = 0; i < numPartners; i++)
		{
			PlayerMoveAnimation(playerData.state, partners[i].direction, partners[i].currentAnimation);
		}
		OffsetPartners();
	}
	PlayerMoveAnimation(playerData.state, playerData.direction, playerData.currentAnimation);

	UpdatePointCheker();
	
	return true;
}

void Player::UpdatePointCheker()
{
	pointCheker = iPoint(playerData.position.x+16, playerData.position.y+35);

	switch (playerData.direction)
	{
	case WALK_L:
		pointCheker.x-= 20;

		break;
	case WALK_R:
		pointCheker.x += 20;

		break;
	case WALK_UP:
		pointCheker.y -= 16;

		break;
	case WALK_DOWN:
		pointCheker.y += 26;

		break;
	default:
		break;
	}
	if (CheckCollisionBoxes(app->map->WorldToMap(pointCheker)))
	{
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) 
		{
			//LOG("TOCADO");
			MoveBox();

		}

	}
	//else
		//LOG("NO TOCADO");

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
	idleAnimL->speed = (dt * 6) ;
	idleAnimR->speed = (dt * 6) ;
	idleAnimUp->speed = (dt * 6) ;
	idleAnimDown->speed = (dt * 6) ;

	walkAnimR->speed = (dt * 6);
	walkAnimL->speed = (dt * 6);
	walkAnimUp->speed = (dt * 6);
	walkAnimDown->speed = (dt * 6);

	bookAnim->speed = (dt * 11);

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

void Player::CameraPlayer(float dt)
{
	// Camera follow the player
	/*int followPositionPalyerX = playerData.position.x - (WINDOW_W / 2);
	int followPositionPalyerY = playerData.position.y - (WINDOW_H / 2);

	app->render->camera.x = -followPositionPalyerX;
	app->render->camera.y = -followPositionPalyerY;*/

	if (!app->dialogueSystem->onDialog)
	{
		float disX = playerData.position.x - lerpCamera.x;
		float disY = playerData.position.y - lerpCamera.y;

		lerpCamera.x += disX * dt * 0.8;
		lerpCamera.y += disY * dt * 0.8;

		if (abs(disX) < 20 && playerData.state == IDLE) lerpCamera.x += disX * dt;
		if (abs(disY) < 20 && playerData.state == IDLE) lerpCamera.y += disY * dt;

		app->sceneManager->onDialog = false;
	}	

	app->render->camera.x = -int(lerpCamera.x) + WINDOW_W / 2;
	app->render->camera.y = -int(lerpCamera.y) + WINDOW_H / 2;
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
	case MOBILE:
		currentAnimation = bookAnim;

		break;

	default:
		break;
	}
}

void Player::PlayerControls(float dt)
{
	diagonal = 0;
	GamePad& pad = app->input->pads[0];
	
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
	// Player Run
	/*if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT
		&& (playerData.state == State::WALK || playerData.state == State::RUN))
	{
		vel = playerData.velocity * 2;
		playerData.state = State::RUN;
	}*/
	

	// If player change direction active boolean
	if (lastDirection != playerData.direction || diagonal == 2)
	{
		lastDirection = playerData.direction;
		AddBreadcrumb();
	}
	if(!playerCollision) MovePartners();

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

	if (CollisionPlayer(playerData.position) && !godMode)
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
	app->audio->Unload1Fx(fxBookOpen);
	app->audio->Unload1Fx(fxStairs);

	// Player
	delete positionInitial;
	positionInitial = nullptr;

	RELEASE(idleAnimL);
	RELEASE(idleAnimR);
	RELEASE (idleAnimUp);
	RELEASE (idleAnimDown);

	RELEASE(walkAnimL);
	RELEASE(walkAnimR);
	RELEASE(walkAnimUp);
	RELEASE(walkAnimDown);


	for (int i = 0; i < texPartners.Count(); i++)
	{
		app->tex->UnLoad(texPartners.At(i)->data);
	}
	texPartners.Clear();
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

		if(app->sceneManager->GetCurrentScene()->type== SceneType::DUNGEON)
			if (CheckCollisionObstacle(positionMapPlayer)) return true;
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
	
	}
}

