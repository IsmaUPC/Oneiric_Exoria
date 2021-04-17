#ifndef __LIVES_H__
#define __LIVES_H__

#include "Entity.h"
#include "PugiXml\src\pugixml.hpp"

class Lives : public Entity
{
public:

	Lives(iPoint pos);
	Lives(TypeEntity pTypeEntity, iPoint pPosition, float pVelocity, SDL_Texture* pTexture, int dropScore, uint deadFx);

	virtual ~Lives();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();
	void CurrentLiveAnimation();


public:

	bool isCollected = false;

private:
	iPoint position = { 0,0 };
	int numPoints = 0;
	iPoint* pointsCollision = nullptr;

	Animation* liveAnimation = new Animation();
	Animation* particleAnimation = new Animation();

	uint liveFx = 0;

	SDL_Texture* texLive = nullptr;

};
#endif // _LIVES_H_

