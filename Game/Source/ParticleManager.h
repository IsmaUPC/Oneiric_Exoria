//#ifndef __PARTICLE_MANAGER_H__
//#define __PARTICLE_MANAGER_H__
//
//
//#include "Module.h"
//#include "Render.h"
//#include "App.h"
////#include "Textures.h"
////#include "Animation.h"
//#include "Particle.h"
//#include "SceneManager.h"
//
//
//class ParticleManager: public Module
//{
//
//	ParticleManager();
//	~ParticleManager();
//
//	// Called before render is available
//	bool Awake(pugi::xml_node&);
//
//	// Called before the first frame
//	bool Start();
//	// Called each loop iteration
//	bool PreUpdate();
//
//	// Called each loop iteration
//	bool Update(float dt);
//
//	// Called each loop iteration
//	bool PostUpdate();
//	// Called before quitting
//	bool CleanUp();
//
//	void ClearList(bool& ret);
//
//	// Virtual method to Load state
//	bool LoadState(pugi::xml_node&);
//	// Virtual method to Save state
//	bool SaveState(pugi::xml_node&) const;
//
//public:
//
//	void SpeedAnimationCheck(float dt);
//	bool AddParticle(iPoint* pos, iPoint* vMax, int lifetime, TypeParticle type, bool isActive = true);
//	bool LoadParticleProperties();
//
//
//private:
//	SDL_Texture* texParticles = nullptr;
//	List<Particle*>* particles;
//	List<ParticleProperties*>* typeProperties;
//	List<Entity*>* spawnQueue;
//	List<Animation*>* animations;
//
//	pugi::xml_document propertiesFile;
//	pugi::xml_node rootNode;
//public:
//
//
//};
//
//
//#endif // __PARTICLE_MANAGER_H__