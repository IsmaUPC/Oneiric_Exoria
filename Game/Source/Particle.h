//#ifndef _PARTICLE_H
//#define _PARTICLE_H
//
//
//#include "Module.h"
//#include "Render.h"
//#include "List.h"
//
//
//enum class TypeParticle
//{
//	LIGHTNING,
//	FIRE_BALL,
//
//	//Add the new types in the line above
//	COUNT,
//	UNKNOWN,
//	
//};
//
//// only store non-repeated properties
//struct ParticleData
//{
//	ParticleData(iPoint* pos, iPoint* vMax, int lifetime, TypeParticle type)
//		:pos(pos), vMax(vMax),lifetime(lifetime), type(type){};
//public:
//	// time spent alive, max time permitted alive
//	int lifetime, lifespan=0;
//	// position
//	iPoint* pos = nullptr;
//	// velocity
//	iPoint* vMax=nullptr;
//
//	TypeParticle type= TypeParticle::UNKNOWN;
//};
//
//// utilility struct to save xml file loaded content
//struct ParticleProperties
//{
//	ParticleProperties(float _min_lifespan, float _max_lifespan,
//		iPoint* _vMin, iPoint* _vMax, float _width, float _height) 
//	{
//		lifespanMin=_min_lifespan;
//		lifespanMax=_max_lifespan;
//		vMin=_vMin;
//		vMax=_vMax;
//		width=_width;
//		height=_height;
//
//		widthMax=_width;
//		heightMax=_height;
//
//		widthMin=_width;
//		heightMin=_height;
//	};
//	ParticleProperties();
//
//	// margin values for random assignation when creating a ParticleData
//	float lifespanMin, lifespanMax=0;
//	iPoint* vMin=nullptr;
//	iPoint* vMax=nullptr;
//
//	//float width height;
//	float width, height;
//	float widthMax, heightMax;
//	float widthMin, heightMin;
//};
//
//
//
//class Particle : Module
//{
//
//public:
//
//	Particle();
//
//	Particle(iPoint* pos, iPoint* vMax, int lifetime, TypeParticle type, bool isActive=true)
//	{
//		particleData = new ParticleData(pos, vMax, lifetime, type);
//		active = isActive;
//	}
//	~Particle();
//
//	// Called before render is available
//	virtual bool Awake(pugi::xml_node&);
//	// Called before the first frame
//	virtual bool Start();
//	// Called each loop iteration
//	virtual bool PreUpdate();
//
//	bool Update(float dt);
//	// Called each loop iteration
//	virtual bool PostUpdate();
//	// Called before quitting
//	virtual bool CleanUp();
//
//
//	// Virtual method to Load state
//	bool LoadState(pugi::xml_node&);
//	// Virtual method to Save state
//	bool SaveState(pugi::xml_node&) const;
//
//public:
//
//	// whether to draw debug info or not
//	bool debugDraw = false;
//
//	unsigned int particles_count = 0;
//	ParticleProperties* properties;
//	ParticleData* particleData;
//
//};
//
//#endif // _PARTICLE_H
