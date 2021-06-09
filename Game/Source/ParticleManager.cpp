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

	// Load Fx

	// Load Textures
	texParticles = app->tex->Load("Textures/Characters/attaks/atlas_attack.png");

	int counti=0;

	// Animations
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 10; x++)
			animLightning->PushBack({ 100 * x,100 * y,100,100 }), counti++;

	animLightning->speed = 1;
	animLightning->loop = true;

	for (int y = 0; y < 6; y++)
		for (int x = 0; x < 10; x++)
			animFireBall->PushBack({ 100 * x,100 * (y+counti),100,100 }), counti++;

	animFireBall->speed = 1;

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
	for (int i = 0; i < animations->Count(); i++)
	{
		animations->At(i)->data->speed = dt * 6;
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

bool ParticleManager::AddParticle(iPoint* pos, TypeParticle type)
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
	case TypeParticle::UNKNOWN:

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
				p->data->pendingToDelete = true;
			else 
				app->render->DrawTexture(texParticles,p->data->pos->x, p->data->pos->y, &rect);
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
	if(animations!= NULL)
	animations->Clear();
	//RELEASE(isDetectedAnim);

	active = false;

	return ret;
}

void ParticleManager::ClearList(bool& ret)
{

	// Clear list
	if (typeProperties != NULL)
	typeProperties->Clear();

	if(spawnQueue != NULL)
	spawnQueue->Clear();
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
		for (int x = 0; x < numTypes; x++)
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

			typeProperties->Add(p->properties);

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
	case TypeParticle::UNKNOWN:
		break;
	default:
		break;
	}
}
