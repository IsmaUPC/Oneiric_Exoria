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

	// Enemy Path, change the destination!
	destination = app->player->playerData.position;
	return true;
}

bool Enemy::Awake(pugi::xml_node& config)
{
	LOG("Loading Enemy Parser");
	bool ret = true;
	
	return ret;
}
bool Enemy::Radar(iPoint distance)
{
	if (entityData.position.DistanceManhattan(distance) < range) return true;

	return false;
}
int Enemy::CalculateDistance(iPoint origin, iPoint destination)
{
	return abs(origin.x - destination.x) + abs(origin.y - destination.y);;
}
void Enemy::CreatePathEnemy(iPoint mapPositionEnemy, iPoint mapPositionDestination)
{
	// Create the path for enemies
}
bool Enemy::CheckAllPoints(iPoint& mapPositionDestination, TypeCollision typeCollision)
{
	int y = mapPositionDestination.y;
	int x = mapPositionDestination.x;
	iPoint positionMapEntity;

	for (int i = 0; i < entityData.numPoints; i++)
	{
		// Concvert position player WorldToMap 
		positionMapEntity = app->map->WorldToMap(x + entityData.pointsCollision[i].x, y + entityData.pointsCollision[i].y);
		if (CheckCollision(positionMapEntity) == typeCollision) return true;
	}
	return false;
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
void Enemy::MoveEnemy(iPoint nextAuxPositionEenemy, iPoint mapPositionEnemy, TypeEntity type)
{
	
}
void Enemy::CheckCollisionEnemyToPlayer()
{

}

void Enemy::CheckCollisions()
{

}
bool Enemy::PreUpdate()
{
	return true;
}

bool Enemy::Update(float dt)
{
	entityData.velocity = floor(1000 * dt) / 8;
	CheckCollisions();
		
	entityData.currentAnimation->Update();
	return true;
}

bool Enemy::PostUpdate()
{
	SDL_Rect rectEnemy;
	rectEnemy = entityData.currentAnimation->GetCurrentFrame();

	// Draw player in correct direction
	if (entityData.direction == MoveDirection::WALK_L)
		app->render->DrawTexture(entityData.texture, entityData.position.x, entityData.position.y, &rectEnemy);
	else if (entityData.direction == MoveDirection::WALK_R)
		app->render->DrawTextureFlip(entityData.texture, entityData.position.x - (rectEnemy.w - app->entityManager->idleAnim->frames->w), entityData.position.y, &rectEnemy);

	return true;
}

bool Enemy::CleanUp()
{

	if (!active)
		return true;

	pendingToDelete = true;
	active = false;

	return true;
}
