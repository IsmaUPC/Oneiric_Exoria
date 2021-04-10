#include "Enemy.h"
#include "Player.h"
#include "GUI.H"
#include "EntityManager.h"
#include "Pathfinding.h"

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
	
	return true;
}

bool Enemy::Awake(pugi::xml_node& config)
{
	LOG("Loading Enemy Parser");
	bool ret = true;
	
	return ret;
}

int Enemy::CalculateDistance(iPoint origin, iPoint destination)
{
	return abs(origin.x - destination.x) + abs(origin.y - destination.y);;
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
				LOG("Velocity R: %f", entityData.velocity);
			}
			else if (entityData.position.x > destination.x)
			{
				entityData.position.x -= entityData.velocity;
				entityData.direction = WALK_L;
				LOG("Velocity L: %f", entityData.velocity);
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
	pendingToDelete = true;
	active = false;

	return true;
}
