#include "ParticleManager.h"

#include "Defs.h"
#include "Log.h"

ParticleManager::ParticleManager() : Module()
{
	name.Create("particle_manager");
}

// Destructor
ParticleManager::~ParticleManager()
{}

// Called before ParticleManager is available
bool ParticleManager::Awake(pugi::xml_node& config)
{
	bool ret = true;

	return ret;
}

// Called before the first frame
bool ParticleManager::Start()
{
	LOG("ParticleManager start");
	active = true;

	animLightning = new Animation();
	animFireBall = new Animation();
	animExplosion = new Animation();
	animExplosionBall = new Animation();
	animFireSmoke = new Animation();
	animShadow = new Animation();
	animEnergy = new Animation();
	animClaw = new Animation();

	// Load Textures
	texParticles = app->tex->Load("Textures/Characters/attaks/atlas_attack.png");

	int counti=0;
	int tmp=0;

	// Animations
	for (int y = 0; y < 4; y++, counti++)
		for (int x = 0; x < 10; x++)
			animLightning->PushBack({ 100 * x,100 * y,100,100 });
	
	animLightning->speed = 1;
	animLightning->loop = false;
	animations.Add(animLightning);

	tmp = counti;
	for (int y = 0; y < 6; y++, counti++)
		for (int x = 0; x < 10; x++)
			animFireBall->PushBack({ 100 * x,100 * (y + tmp),100,100 });
	animFireBall->speed = 1;
	animFireBall->loop = false;
	animations.Add(animFireBall);

	tmp = counti;
	for (int y = 0; y < 5; y++, counti++)
		for (int x = 0; x < 8; x++)
			animExplosion->PushBack({ 100 * x,100 * (y + tmp),100,100 });
	animExplosion->speed = 1;
	animExplosion->loop = false;
	animations.Add(animExplosion);

	tmp = counti;
	for (int y = 0; y < 5; y++, counti++)
		for (int x = 0; x < 8; x++)
			animExplosionBall->PushBack({ 100 * x,100 * (y + tmp),100,100 });
	animExplosionBall->speed = 1;
	animExplosionBall->loop = false;
	animations.Add(animExplosionBall);

	counti = 0;
	tmp = counti;
	for (int y = 0; y < 8; y++, counti++)
		for (int x = 0; x < 8; x++)
			animFireSmoke->PushBack({ 100 * (x+10),100 * (y + tmp),100,100 });
	animFireSmoke->speed = 1;
	animFireSmoke->loop = false;
	animations.Add(animFireSmoke);

	tmp = counti;
	for (int y = 0; y < 8; y++, counti++)
		for (int x = 0; x < 8; x++)
			animShadow->PushBack({ 100 * (x + 10),100 * (y + tmp),100,100 });
	animShadow->speed = 1;
	animShadow->loop = false;
	animations.Add(animShadow);

	counti = 0;
	tmp = counti;
	for (int y = 0; y < 11; y++, counti++)
		for (int x = 0; x < 11; x++)
			animEnergy->PushBack({ 100 * (x + 18),100 * (y + tmp),100,100 });
	animEnergy->speed = 1;
	animEnergy->loop = false;
	animations.Add(animEnergy);

	tmp = counti;
	for (int y = 0; y < 8; y++, counti++)
		for (int x = 0; x < 8; x++)
			animClaw->PushBack({ 100 * (x + 18),100 * (y + tmp),100,100 });
	animClaw->speed = 1;
	animClaw->loop = false;
	animations.Add(animClaw);

	LoadParticleProperties();

	return true;
}

// Called each loop iteration
bool ParticleManager::PreUpdate()

{
	//List<ParticleToDraw*>* tmp;
	if(&particles != nullptr)
	for (ListItem<ParticleToDraw*>* particle = particles.start; particle; particle = particle->next) {

		if (particle->data->pendingToDelete)particles.Del(particle);
	}

	return true;
}

bool ParticleManager::Update(float dt)
{
	if (!app->sceneManager->GetIsPause())
	{
		//SpeedAnimationCheck(dt);
		if (&particles != nullptr)
		for (ListItem<ParticleToDraw*>* particle = particles.start; particle; particle = particle->next)
			particle->data->animation->Update();
	}

	return true;
}
void ParticleManager::SpeedAnimationCheck(float dt)
{
	for (int i = 0; i < animations.Count(); i++)
	{
		animations.At(i)->data->speed = dt * 6;
	}
}

//bool ParticleManager::AddParticleType(iPoint* pos, iPoint* vMax, int lifetime, TypeParticle type, bool isActive)
//{
//	Particle* particle;
//	 
//	if(particle = new Particle(pos,vMax,lifetime,type))	particlesType->Add(particle);
//	else
//	{
//		LOG("Failure detected when creating particle: %c", SDL_GetError());
//		return false;
//	}
//
//	LOG("Successful particle creation");
//	return true;
//}

bool ParticleManager::AddParticle(iPoint pos, TypeParticle type)
{
	ParticleToDraw* pd= new ParticleToDraw();
	pd->pos = pos;
	pd->type = type;
	
	switch (type)
	{
	case TypeParticle::LIGHTNING:
		pd->animation = animLightning;

		break;
	case TypeParticle::FIRE_BALL:
		pd->animation = animFireBall;

		break;
	case TypeParticle::EXPLOSION:
		pd->animation = animExplosion;

		break;
	case TypeParticle::EXPLOSION_BALL:
		pd->animation = animExplosionBall;

		break;
	case TypeParticle::FIRE_SMOKE:
		pd->animation = animFireSmoke;

		break;
	case TypeParticle::SHADOW:
		pd->animation = animShadow;

		break;
	case TypeParticle::ENERGY:
		pd->animation = animEnergy;

		break;
	case TypeParticle::CLAW:
		pd->animation = animClaw;

		break;
	case TypeParticle::UNKNOWN:
		pd->animation = animFireBall;

		break;
	default:
		break;
	}

	if(pd != nullptr)particles.Add(pd);
	else
	{
		LOG("Failure detected when creating particle: %c", SDL_GetError());
		return false;
	}

	LOG("Successful particle creation");
	return true;
}

bool ParticleManager::PostUpdate()
{
	SDL_Rect rect;
	if (&particles != nullptr)
		for (ListItem<ParticleToDraw*>* p = particles.start; p; p = p->next) {
			
			rect = p->data->animation->GetCurrentFrame();

			if (p->data->animation->HasFinished())
			{
				p->data->pendingToDelete = true;
				p->data->animation->Reset();
			}
			else 
				app->render->DrawTexture(texParticles,p->data->pos.x, p->data->pos.y, &rect, 2);
		}
	return true;
}

// Called before quitting
bool ParticleManager::CleanUp()
{
	LOG("Destroying ParticleManager");

	bool ret = true;
	ClearList(ret);

	// Unload Tx
	app->tex->UnLoad(texParticles);

	// Unload Animations
	animations.Clear();

	active = false;

	return ret;
}

void ParticleManager::ClearList(bool& ret)
{
	// Clear list
	typeProperties.Clear();
	//spawnQueue.Clear();
}

bool ParticleManager::LoadState(pugi::xml_node&)
{
	return false;
}
bool ParticleManager::SaveState(pugi::xml_node&) const
{
	return false;
}
bool ParticleManager::LoadParticleProperties()
{
	bool ret = false;
	int numTypes = (int)TypeParticle::COUNT;
	//  Load savegame.xml file using load_file() method from the xml_document class
	pugi::xml_parse_result result = propertiesFile.load_file(PROPERTIES_FILE);

	// Check result for loading errors
	if (result == NULL)
	{
		LOG("Could not load map xml file %c. pugi error: %s", PROPERTIES_FILE,result.description());
		ret = false;
	}
	else
	{
		rootNode = propertiesFile.first_child();
		
		Particle* p = new Particle();

		// Read the atributes of the type particle 
		//for (int x = 0; x < numTypes; x++)
		for (int x = 0; x < 2; x++)
		{
			p->particleData->type = (TypeParticle)rootNode.attribute("type").as_int((int)TypeParticle::UNKNOWN);
			p->properties->lifespanMin = rootNode.child("lifespan").attribute("min").as_float();
			p->properties->lifespanMax = rootNode.child("lifespan").attribute("max").as_float();

			p->properties->vMin->x = rootNode.child("velocity").attribute("min_vx").as_float();
			p->properties->vMin->y= rootNode.child("velocity").attribute("min_vy").as_float();
			p->properties->vMax->x = rootNode.child("velocity").attribute("max_vx").as_float();
			p->properties->vMax->y = rootNode.child("velocity").attribute("max_vy").as_float();

			p->properties->width= rootNode.child("draw").attribute("width").as_float();
			p->properties->height = rootNode.child("draw").attribute("height").as_float();

			p->properties->widthMin = rootNode.child("draw").attribute("min_w").as_float();
			p->properties->widthMax = rootNode.child("draw").attribute("max_w").as_float();

			p->properties->heightMin = rootNode.child("draw").attribute("min_h").as_float();
			p->properties->heightMax = rootNode.child("draw").attribute("max_h").as_float();

			AssignPartAnim(p);

			typeProperties.Add(p->properties);

			rootNode = rootNode.next_sibling();
		}

		propertiesFile.reset();
		(ret == true) ? LOG("Carga de modulos existosa \n") : LOG("Fallo en la carga  \n");

	}

	return false;
}

void ParticleManager::AssignPartAnim(Particle* p)
{


	switch (p->particleData->type)
	{
	case TypeParticle::LIGHTNING:
		p->particleData->anim = animLightning;

		break;
	case TypeParticle::FIRE_BALL:
		p->particleData->anim = animFireBall;

		break;
	case TypeParticle::EXPLOSION:
		p->particleData->anim = animExplosion;

		break;
	case TypeParticle::EXPLOSION_BALL:
		p->particleData->anim = animExplosionBall;

		break;
	case TypeParticle::FIRE_SMOKE:	
		p->particleData->anim = animFireSmoke;

		break;
	case TypeParticle::SHADOW:
		p->particleData->anim = animShadow;

		break;
	case TypeParticle::ENERGY:
		p->particleData->anim = animEnergy;

		break;
	case TypeParticle::CLAW:
		p->particleData->anim = animClaw;

		break;
	case TypeParticle::UNKNOWN:
		p->particleData->anim = animFireBall;

		break;
	default:
		break;
	}
}
