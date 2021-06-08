#ifndef __OBSTACLE_OBJECTS_H__
#define __OBSTACLE_OBJECTS_H__

#include "Entity.h"

enum TypeObstacle 
{
	OBSTACLE_BOX,
	OBSTACLE_HOLE,

};

class ObstacleObjects :	public Entity
{
public:

	ObstacleObjects();
	ObstacleObjects(Entity* entity, SDL_Texture* tex) ;
	~ObstacleObjects();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();
	// Called each loop iteration
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);
	// Called each loop iteration
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();

	bool LoadState(pugi::xml_node&);
	bool SaveState(pugi::xml_node&) const;

public:


	iPoint tilePosition;
};

#endif // __OBSTACLE_OBJECTS_H__