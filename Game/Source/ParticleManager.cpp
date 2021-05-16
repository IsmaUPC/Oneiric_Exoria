//#include "ParticleManager.h"
//
//#include "Defs.h"
//#include "Log.h"
//
//ParticleManager::ParticleManager() : Module()
//{
//	name.Create("particle_manager");
//}
//
//// Destructor
//ParticleManager::~ParticleManager()
//{}
//
//// Called before ParticleManager is available
//bool ParticleManager::Awake(pugi::xml_node& config)
//{
//	bool ret = true;
//
//	return ret;
//}
//
//// Called before the first frame
//bool ParticleManager::Start()
//{
//	LOG("ParticleManager start");
//	active = true;
//
//	// Load Fx
//
//	// Load Textures
//	texParticles = app->tex->Load("Textures/");
//
//	// Animations
//
//
//	return true;
//}
//
//// Called each loop iteration
//bool ParticleManager::PreUpdate()
//{
//	for (ListItem<Particle*>* particle = particles->start; particle; particle = particle->next)
//		particle->data->PreUpdate();
//
//	return true;
//}
//
//bool ParticleManager::Update(float dt)
//{
//	if (!app->sceneManager->GetIsPause())
//	{
//		SpeedAnimationCheck(dt);
//
//		for (ListItem<Particle*>* particle = particles->start; particle; particle = particle->next)
//			particle->data->Update(dt);
//	}
//
//	return true;
//}
//void ParticleManager::SpeedAnimationCheck(float dt)
//{
//	for (int i = 0; i < animations->Count(); i++)
//	{
//		animations->At(i)->data->speed = dt * 6;
//	}
//}
//
//bool ParticleManager::AddParticle(iPoint* pos, iPoint* vMax, int lifetime, TypeParticle type, bool isActive = true)
//{
//	Particle* particle;
//	 
//	if(particle = new Particle(pos,vMax,lifetime,type))	particles->Add(particle);
//	else
//	{
//		LOG("Failure detected when creating particle: %c", SDL_GetError());
//		return false;
//	}
//
//	LOG("Successful particle creation");
//	return true;
//}
//
//bool ParticleManager::PostUpdate()
//{
//	if (!app->sceneManager->GetWinBattle())
//	{
//		
//		for (ListItem<Particle*>* particle = particles->start; particle; particle = particle->next)
//			particle->data->PostUpdate();
//	}
//
//	return true;
//}
//
//// Called before quitting
//bool ParticleManager::CleanUp()
//{
//	LOG("Destroying ParticleManager");
//
//	bool ret = true;
//	ClearList(ret);
//
//	// Unload Tx
//	app->tex->UnLoad(texParticles);
//
//	// Unload Animations
//	animations->Clear();
//	//RELEASE(isDetectedAnim);
//
//	active = false;
//
//	return ret;
//}
//
//void ParticleManager::ClearList(bool& ret)
//{
//
//	// Clear list
//	particles->Clear();
//	spawnQueue->Clear();
//}
//
//bool ParticleManager::LoadState(pugi::xml_node&)
//{
//	return false;
//}bool ParticleManager::LoadParticleProperties()
//{
//	bool ret = false;
//	int numTypes = (int)TypeParticle::COUNT;
//	//  Load savegame.xml file using load_file() method from the xml_document class
//	pugi::xml_parse_result result = propertiesFile.load_file(PROPERTIES_FILE);
//
//	// Check result for loading errors
//	if (result == NULL)
//	{
//		LOG("Could not load map xml file %c. pugi error: %s", PROPERTIES_FILE,result.description());
//		ret = false;
//	}
//	else
//	{
//		rootNode = propertiesFile.first_child();
//		ParticleProperties* propertyOfType = new ParticleProperties();
//
//		for (int x = 0; x < numTypes; x++)
//		{
//			propertyOfType->lifespanMin = rootNode.child("lifespan").attribute("min").as_float();
//			propertyOfType->lifespanMax = rootNode.child("lifespan").attribute("max").as_float();
//
//			propertyOfType->vMin->x = rootNode.child("velocity").attribute("min_vx").as_float();
//			propertyOfType->vMin->y= rootNode.child("velocity").attribute("min_vy").as_float();
//			propertyOfType->vMax->x = rootNode.child("velocity").attribute("max_vx").as_float();
//			propertyOfType->vMax->y = rootNode.child("velocity").attribute("max_vy").as_float();
//
//			propertyOfType->width= rootNode.child("draw").attribute("width").as_float();
//			propertyOfType->height = rootNode.child("draw").attribute("height").as_float();
//
//			propertyOfType->widthMin = rootNode.child("draw").attribute("min_w").as_float();
//			propertyOfType->widthMax = rootNode.child("draw").attribute("max_w").as_float();
//
//			propertyOfType->heightMin = rootNode.child("draw").attribute("min_h").as_float();
//			propertyOfType->heightMax = rootNode.child("draw").attribute("max_h").as_float();
//
//			typeProperties->Add(propertyOfType);
//
//		}
//
//		propertiesFile.reset();
//		(ret == true) ? LOG("Carga de modulos existosa \n") : LOG("Fallo en la carga  \n");
//
//	}
//
//	return false;
//}
//
//bool ParticleManager::SaveState(pugi::xml_node&) const
//{
//	return false;
//}
