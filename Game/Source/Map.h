#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "List.h"
#include "Point.h"
#include "PQueue.h"
#include "DynArray.h"
#include "Animation.h"
#include "TeleportNode.h"

#include "PugiXml\src\pugixml.hpp"

#define COST_MAP_SIZE	100


struct TileSet
{
	SString	name = "";
	int	firstgid = 1;
	int margin = 0;
	int	spacing = 0;
	int	tileWidth = 0;
	int	tileHeight = 0;

	int idFloor = 0;

	SDL_Texture* texture = nullptr;
	int	texWidth = 0;
	int	texHeight = 0;
	int	numTilesWidth = 0;
	int	numTilesHeight = 0;
	int	offsetX = 0;
	int	offsetY = 0;
	int tilecount = 0;

	SDL_Rect GetTileRect(int id) const;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

struct Properties
{
	struct Property
	{
		SString	name = "";
		int value = 0;
	};
	
	~Properties()
	{
		ListItem<Property*>* listD;
		listD = list.start;
		while (listD != nullptr)
		{
			RELEASE(listD->data);
			listD = listD->next;
		}
		list.Clear();
	}

	// Method to ask for the value of a custom property
	int GetProperty(const char* name, int defaultValue = 0) const;
	int GetPropertyValue(const char* name);

	List<Property*> list;
};

struct MapLayer
{
	SString	name = "";
	int width = 0;
	int height = 0;	
	int mapWidth = 0;
	int mapHeight = 0;
	uint* data = nullptr;

	Properties properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}
	
	// Get the value of x,y
	inline uint Get(int x, int y) const
	{
		if ((x <= mapWidth && x >= 0) && (y <= mapHeight && y >= 0))
			return data[(y * width) + x];
		else return 0;
	}
};

struct MapData
{
	int width = 0;
	int	height = 0;
	int	tileWidth = 0;
	int	tileHeight = 0;
	SDL_Color backgroundColor;
	MapTypes type;
	List<TileSet*> tilesets;

	List<MapLayer*> layers;
};


struct CheckPoints{
	struct CP
	{
		CP(iPoint pos) : pos(pos)
		{
			active = false;
		}
		iPoint pos;
		bool active;
	};

	~CheckPoints()
	{
		ListItem<CP*>* listD;
		listD = list.start;
		while (listD != nullptr)
		{
			RELEASE(listD->data);
			listD = listD->next;
		}
		list.Clear();
	}

	Animation* checkPointOffAnim = new Animation();
	Animation* checkPointOnAnim = new Animation();
	SDL_Texture* texture = nullptr;
	List<CP*> list;
};

class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called each loop iteration, post player draw
	void DrawUp();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;
	iPoint MapToWorld(iPoint position) const;

	// Translates x,y coordinates from  world positions to map positions 
	iPoint WorldToMap(int x, int y) const;
	iPoint WorldToMap(iPoint position) const;

	// Get dimensions of map
	iPoint GetDimensionsMap();

	// BFS Pathfinding methods
	void ResetPath(iPoint start);
	void DrawPath();

	// More pathfinding methods
	int MovementCost(int x, int y) const;
	void ComputePath(int x, int y);

	// Propagation methods
	void PropagateDijkstra();

	// AStar pathfinding
	void ComputePathAStar(int x, int y);
	// AStar propagation
	void PropagateAStar(int heuristic);

	int CalculateDistanceToDestiny(iPoint node);
	int CalculateDistanceToStart(iPoint node);

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	void CheckPointActive(iPoint position);

	void SetDrawColl() 
	{ 
		drawColl = !drawColl; 
		drawColl2 = !drawColl2;
	};
private:

	// Methods to load all required map data
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tilesetNode, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	int LoadCheckPoint();
	void LoadCollectable();
	void LoadTpNodes();

	// Load a group of properties 
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	// Pick the right Tileset based on a tile id
	TileSet* GetTilesetFromTileId(int id) const;


public:

	MapData data;
	iPoint tileDestiny = { 0,0 };
	CheckPoints checKpointsMap;


	List<TeleportNode*> tpNodeUpLadder;
	List<TeleportNode*> tpNodeDownLadder;
	List<TeleportNode*> tpNodeUpHall;
	List<TeleportNode*> tpNodeDownHall;
private:

	TeleportNode* nodeTp = nullptr;

	pugi::xml_document mapFile;
	SString folder;
	bool mapLoaded = false;

	bool drawColl=false;
	bool drawColl2=false;

	// BFS Pathfinding variables
	PQueue<iPoint> frontier;
	List<iPoint> visited;

	// Additional variables
	List<iPoint> breadcrumbs;
	uint costSoFar[COST_MAP_SIZE][COST_MAP_SIZE];
	DynArray<iPoint> path;


	// AStar (A*) variables
	iPoint goalAStar = { 0,0 };	// Store goal target tile
	bool finishAStar = false;	// Detect when reached goal

	SDL_Texture* tileX = nullptr;

	List<MapLayer*>* layerDrawUp;

	iPoint vec = { 0,0 };
	int idFloor = 0;
};

#endif // __MAP_H__