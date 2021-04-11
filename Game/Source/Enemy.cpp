#include "App.h"
#include "Enemy.h"
#include "Player.h"
#include "GUI.H"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

Enemy::Enemy() : Entity()
{
	name.Create("Enemy");
}

Enemy::Enemy(Entity* entity, SDL_Texture* tex)
{
	entityData = entity->entityData;
	entityData.texture = tex;

	name.Create("Enemy");
}

Enemy::~Enemy()
{}

bool Enemy::Start()
{
	active = true;

	entityData.currentAnimation = app->entityManager->idleAnim;

	// Enemy Path
	destination = entityData.positionInitial;

	// Collisons
	if (entityData.type == BANDIT)
	{
		entityData.pointsCollision[0] = { 6, 18 };
		entityData.pointsCollision[1] = { 24, 18 };
		entityData.pointsCollision[2] = { 24, 32 };
		entityData.pointsCollision[3] = { 6, 32 };
	}

	radiusCollision = app->entity->CalculateDistance(entityData.pointsCollision[0], entityData.pointsCollision[2]) / 2;
	entityData.centerPoint.x = app->entity->CalculateDistance(entityData.pointsCollision[0], entityData.pointsCollision[1]) / 2;
	entityData.centerPoint.y = app->entity->CalculateDistance(entityData.pointsCollision[0], entityData.pointsCollision[3]) / 2;

	return true;
}

bool Enemy::Awake(pugi::xml_node& config)
{
	LOG("Loading Enemy Parser");
	bool ret = true;
	
	return ret;
}

void Enemy::CreatePathEnemy(iPoint mapPositionEnemy, iPoint mapPositionDestination)
{
	// Create the path for enemies
}

int Enemy::GetCurrentPositionInPath(iPoint mapPositionEnemy)
{
	int i;
	for (i = 0; i < lastPath->Count(); i++)
	{
		if (mapPositionEnemy == iPoint({ lastPath->At(i)->x, lastPath->At(i)->y })) break;
	}
	return i;
}

void Enemy::CheckCollisionEnemyToPlayer()
{
	iPoint enemyCenter;
	enemyCenter.x = entityData.position.x + entityData.centerPoint.x;
	enemyCenter.y = entityData.position.y + entityData.centerPoint.y;

	iPoint playerCenter;
	playerCenter.x = app->player->playerData.position.x + app->player->playerData.centerPoint.x;
	playerCenter.y = app->player->playerData.position.y + app->player->playerData.centerPoint.y;

	if (radiusCollision + app->player->radiusCollision > app->entity->CalculateDistance(playerCenter, enemyCenter) 
		&& !app->sceneManager->GetEnemeyDetected())
	{
		app->sceneManager->SetEnemeyDetected(true);
		LOG("Collision Detected");
	}
		
}
bool Enemy::CheckCollisionEnemy(fPoint nextPosition)
{
	iPoint positionMapEnemy;
	int y = nextPosition.y;
	int x = nextPosition.x;

	for (int i = 0; i < entityData.numPoints; i++)
	{
		// Convert position player WorldToMap 
		positionMapEnemy = app->map->WorldToMap(x + entityData.pointsCollision[i].x, y + entityData.pointsCollision[i].y);
		if (CheckCollision(positionMapEnemy) == COLLISION) return true;
	}
	return false;
}
bool Enemy::Radar(iPoint distance, int range)
{
	fPoint distanceToPlayer;
	distanceToPlayer.x = distance.x;
	distanceToPlayer.y = distance.y;
	if (entityData.position.DistanceManhattan(distanceToPlayer) < range) return true;

	return false;
}
void Enemy::MoveEnemy()
{
	tmp = entityData.position;
	if (returning)
	{
		float angle = atan2(entityData.positionInitial.y - entityData.position.y, entityData.positionInitial.x - entityData.position.x);
		entityData.position.x += entityData.velocity * cos(angle);
		entityData.position.y += entityData.velocity * sin(angle);

		iPoint enemyPosition;
		enemyPosition.x = entityData.position.x;
		enemyPosition.y = entityData.position.y;
		if (app->map->WorldToMap(enemyPosition).x == app->map->WorldToMap(entityData.positionInitial).x)
			returning = false;
	}
	else if (isDetected)
	{
		float angle = atan2(app->player->playerData.position.y - entityData.position.y, app->player->playerData.position.x - entityData.position.x);
		entityData.position.x += entityData.velocity * cos(angle);
		entityData.position.y += entityData.velocity * sin(angle);
	}
	else
	{
		if (id == 0)
		{
			int numTiles = 5;

			if (entityData.position.x < destination.x)
			{
				entityData.position.x += entityData.velocity;
				entityData.direction = WALK_R;
			}
			else if (entityData.position.x > destination.x)
			{
				entityData.position.x -= entityData.velocity;
				entityData.direction = WALK_L;
			}
			iPoint enemyPosition;
			enemyPosition.x = entityData.position.x;
			enemyPosition.y = entityData.position.y;
			if (app->map->WorldToMap(enemyPosition).x == app->map->WorldToMap(destination).x)
			{
				if (destination.x == entityData.positionInitial.x)
					destination.x = entityData.positionInitial.x + numTiles * app->map->data.tileWidth;
				else destination.x = entityData.positionInitial.x;
			}
		}
	}
	
	if (CheckCollisionEnemy(entityData.position)) entityData.position = tmp;
	
}
bool Enemy::PreUpdate()
{
	return true;
}

bool Enemy::Update(float dt)
{
	entityData.velocity = floor(1000 * dt) / 16;

	if (Radar(app->player->playerData.position, range)) isDetected = true;
	else isDetected = false;

	if (!Radar(entityData.positionInitial, rangeMax)) returning = true;
	else if (isDetected == true && app->player->playerData.state !=IDLE) returning = false;

	MoveEnemy();

	if (isDetected) CheckCollisionEnemyToPlayer();
		
	entityData.currentAnimation->Update();
	return true;
}

bool Enemy::PostUpdate()
{
	SDL_Rect rectEnemy;
	rectEnemy = entityData.currentAnimation->GetCurrentFrame();

	// Draw player in correct direction
	if (entityData.direction == MoveDirection::WALK_R)
		app->render->DrawTexture(entityData.texture, entityData.position.x, entityData.position.y, &rectEnemy);
	else if (entityData.direction == MoveDirection::WALK_L)
		app->render->DrawTextureFlip(entityData.texture, entityData.position.x - (rectEnemy.w - app->entityManager->idleAnim->frames->w), entityData.position.y, &rectEnemy);

	return true;
}

bool Enemy::CleanUp()
{

	if (!active)
		return true;

	delete entityData.pointsCollision;
	entityData.pointsCollision = nullptr;

	delete lastPath;
	lastPath = nullptr;

	pendingToDelete = true;
	active = false;

	return true;
}
