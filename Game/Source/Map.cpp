#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "SceneLevel2.h"
#include "Map.h"
#include "EntityManager.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>

Map::Map() : Module(), mapLoaded(false)
{
	name.Create("map");
}

// Destructor
Map::~Map()
{}


void Map::ResetPath(iPoint start)
{
	frontier.Clear();
	visited.Clear();
	breadcrumbs.Clear();

	frontier.Push(start, 0);
	visited.Add(start);
	breadcrumbs.Add(start);

	memset(costSoFar, 0, sizeof(uint) * COST_MAP_SIZE * COST_MAP_SIZE);
}

void Map::DrawPath()
{
	iPoint pointV;
	iPoint pointF;
	iPoint pointPath;

	// Draw visited
	ListItem<iPoint>* itemVisited = visited.start;
	PQueueItem<iPoint>* itemFrontier = frontier.start;


	while (itemVisited)
	{
		pointV = itemVisited->data;
		
		TileSet* tileset = GetTilesetFromTileId(260);

		SDL_Rect rec = tileset->GetTileRect(260);
		iPoint pos = MapToWorld(pointV.x, pointV.y);

		app->render->DrawTexture(tileset->texture, pos.x, pos.y, &rec);
		itemVisited = itemVisited->next;
		
	}
	while (itemFrontier)
	{
		TileSet* tileset = GetTilesetFromTileId(259);

		SDL_Rect rec = tileset->GetTileRect(259);

		pointF = itemFrontier->data;
		tileset = GetTilesetFromTileId(259);
		iPoint pos = MapToWorld(pointF.x, pointF.y);
		app->render->DrawTexture(tileset->texture, pos.x, pos.y, &rec);
		itemFrontier = itemFrontier->next;
	}
	int pathSize = path.Count();
	for (size_t i = 0; i < pathSize; i++)
	{
		TileSet* tileset = GetTilesetFromTileId(259);

		SDL_Rect rec = tileset->GetTileRect(259);

		pointPath = { path.At(i)->x,path.At(i)->y };
		tileset = GetTilesetFromTileId(259);
		iPoint pos = MapToWorld(pointPath.x, pointPath.y);
		app->render->DrawTexture(tileset->texture, pos.x, pos.y, &rec);
	
	}
}

int Map::MovementCost(int x, int y) const
{
	int ret = -1;

	if ((x >= 0) && (x < data.width) && (y >= 0) && (y < data.height))
	{
// Coje el layer de las colisiones que en nuestro caso es el tercero
		int id = data.layers.start->next->next->data->Get(x, y);

		if (id != 0)
		{
			int fisrtGid = GetTilesetFromTileId(id)->firstgid;

			if (id == fisrtGid) ret = 1;
			else if (id == fisrtGid + 1) ret =0;
			else if (id == fisrtGid + 2) ret = 3;
		}
		else ret = 1;
	}

	return ret;
}
void Map::ComputePath(int x, int y)
{
	path.Clear();
	iPoint goal = { x, y };
	int size = breadcrumbs.Count()-1;
	path.PushBack(goal);

	ListItem<iPoint>* iterator= visited.end;
	ListItem<iPoint>* tmp = breadcrumbs.At(size);

	for (iterator; iterator; iterator=iterator->prev)
	{
		size--;
		if (iterator->data == tmp->data) 
		{
			path.PushBack(iterator->data);
			tmp=breadcrumbs.At(size);
		}
	}

}
void Map::PropagateDijkstra()
{

	iPoint curr;
	curr = frontier.GetLast()->data;
	if (frontier.Pop(curr) && curr != tileDestiny)
	{
		iPoint neighbors[4];
		neighbors[0].Create(curr.x + 1, curr.y + 0);
		neighbors[1].Create(curr.x + 0, curr.y + 1);
		neighbors[2].Create(curr.x - 1, curr.y + 0);
		neighbors[3].Create(curr.x + 0, curr.y - 1);

		for (uint i = 0; i < 4; ++i)
		{
			if (MovementCost(neighbors[i].x, neighbors[i].y) > 0)
			{
				if (visited.Find(neighbors[i]) == -1)
				{
					frontier.Push(neighbors[i], 0);
					visited.Add(neighbors[i]);
					costSoFar[i][0] = MovementCost(neighbors[i].x, neighbors[i].y);
					breadcrumbs.Add(curr);
				}
			}
		}
	}
	else 
	{
		breadcrumbs.Add(curr);
		ComputePath(tileDestiny.x, tileDestiny.y);
		app->map->ResetPath(app->map->tileDestiny);
	}
}

void Map::PropagateAStar(int heuristic)
{
	// Consider the different heuristics
	iPoint curr;
	curr = frontier.GetLast()->data;
	if (frontier.Pop(curr) && curr != tileDestiny)
	{
		if (true)
		{
			iPoint neighbors[4];
			neighbors[0].Create(curr.x + 1, curr.y + 0);
			neighbors[1].Create(curr.x + 0, curr.y + 1);
			neighbors[2].Create(curr.x - 1, curr.y + 0);
			neighbors[3].Create(curr.x + 0, curr.y - 1);

			for (uint i = 0; i < 4; ++i)
			{
				if (MovementCost(neighbors[i].x, neighbors[i].y) > 0)
				{
					if (visited.Find(neighbors[i]) == -1)
					{
						frontier.Push(neighbors[i], CalculateDistanceToDestiny(neighbors[i])+ CalculateDistanceToStart(neighbors[i]));
						visited.Add(neighbors[i]);
						costSoFar[i][0] = MovementCost(neighbors[i].x, neighbors[i].y);
						breadcrumbs.Add(curr);
					}
				}
			}
		}

	}
	else
	{
		breadcrumbs.Add(curr);
		ComputePath(tileDestiny.x, tileDestiny.y);
		app->map->ResetPath(app->map->tileDestiny);
	}
}

void Map::ComputePathAStar(int x, int y)
{
}

int Map::CalculateDistanceToDestiny(iPoint node)
{
	iPoint distance= tileDestiny-node ;
	return distance.x + distance.y;
}

int Map::CalculateDistanceToStart(iPoint node)
{
	iPoint distance;
	distance =  node -visited.start->data ;
	return distance.x + distance.y;

}

void Map::CheckPointActive(iPoint position)
{
	for (int i = 0; i < checKpointsMap.list.Count(); i++)
	{	
		if (checKpointsMap.list.At(i)->data->pos == position)
		{
			checKpointsMap.list.At(i)->data->active = true;
		}

	}
}

// Ask for the value of a custom property
int Properties::GetProperty(const char* value, int defaultValue) const
{
	for (int i = 0; i < list.Count(); i++)
	{
		if (strcmp(list.At(i)->data->name.GetString(), value)==0)
		{
			if (list.At(i)->data->value != defaultValue) return list.At(i)->data->value;
			else return defaultValue;
		}
	}
	
	return defaultValue;
}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.Create(config.child("folder").child_value());

	return ret;
}

// Draw the map (all requried layers)
void Map::Draw()
{
	if (mapLoaded == false) return;

	// Make sure we draw all the layers and not just the first one
	for (ListItem<MapLayer*>* layer = data.layers.start; layer; layer = layer->next)
	{
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tileId = layer->data->Get(x, y);
				if (tileId > 0)
				{	
					iPoint vec = MapToWorld(x, y);
					for (int i = 0; i < data.tilesets.Count(); i++)
					{
						if(data.layers.At(i)->data->properties.GetProperty("Nodraw",0)==0 || drawColl)
							app->render->DrawTexture(GetTilesetFromTileId(tileId)->texture, vec.x, vec.y, &data.tilesets.At(i)->data->GetTileRect(tileId));
						else if (data.layers.At(i)->data->properties.GetProperty("Nodraw", 0) == 0 || drawColl2)
							app->render->DrawTexture(GetTilesetFromTileId(tileId)->texture, vec.x, vec.y, &data.tilesets.At(i)->data->GetTileRect(tileId));
					}
				}
			}
		}
	}
	
	// CheckPoints
	for (int i = 0; i < checKpointsMap.list.Count(); i++)
	{
		iPoint pos = checKpointsMap.list.At(i)->data->pos;
		pos = MapToWorld(pos.x,pos.y);

		CheckPoints::CP* actCP = checKpointsMap.list.At(i)->data;
		SDL_Rect rectCP;

		if (actCP->active)
			rectCP = checKpointsMap.checkPointOnAnim->GetCurrentFrame();
		else
			rectCP = checKpointsMap.checkPointOffAnim->GetCurrentFrame();
		
		app->render->DrawTexture(checKpointsMap.texture, pos.x, pos.y-2, &rectCP);
	}


	if(drawColl)app->map->DrawPath();
	if(drawColl2)app->map->DrawPath();
}

// Translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tileWidth;
		ret.y = y * data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tileWidth / 2);
		ret.y = (x + y) * (data.tileHeight / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

// Add orthographic world to map coordinates
iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float halfWidth = data.tileWidth * 0.5f;
		float halfHeight = data.tileHeight * 0.5f;
		ret.x = int((x / halfWidth + y / halfHeight) / 2);
		ret.y = int((y / halfHeight - (x / halfWidth)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}
iPoint Map::WorldToMap(iPoint position) const
{
	int x = position.x;
	int y = position.y;
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tileWidth;
		ret.y = y / data.tileHeight;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float halfWidth = data.tileWidth * 0.5f;
		float halfHeight = data.tileHeight * 0.5f;
		ret.x = int((x / halfWidth + y / halfHeight) / 2);
		ret.y = int((y / halfHeight - (x / halfWidth)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}
// Pick the right Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int id) const
{
	ListItem<TileSet*>* item = data.tilesets.start;
	TileSet* set = item->data;
	
	for (set; set; item=item->next, set = item->data)
	{
		if (id >= set->firstgid && id < set->firstgid + set->tilecount)return set;
	}

	return set;
}

// Get relative Tile rectangle
SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect = { 0 };

	int relativeId = id - firstgid;
	rect.w = tileWidth;
	rect.h = tileHeight;
	rect.x = margin + ((rect.w + spacing) * (relativeId % numTilesWidth));
	rect.y = margin + ((rect.h + spacing) * (relativeId / numTilesWidth));
	
	return rect;
}
iPoint Map::GetDimensionsMap()
{
	return { data.tileWidth,data.tileHeight };
}
// Called before quitting
bool Map::CleanUp()
{
	if (!active)
		return true;
	LOG("Unloading map");

	ListItem<TileSet*>* item;
	item = data.tilesets.start;

	while (item != NULL)
	{
		app->tex->UnLoad(item->data->texture);
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.Clear();

	ListItem<MapLayer*>* item2;
	item2 = data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.Clear();

	checKpointsMap.~CheckPoints();
	
	// Clean up the pugui tree
	mapFile.reset();
	active = false;
	return true;
}

// Load new map
bool Map::Load(const char* filenameGame)
{
	bool ret = true;
	SString tmp("%s%s", folder.GetString(), filenameGame);

	pugi::xml_parse_result result = mapFile.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", filenameGame, result.description());
		ret = false;
	}

	// Load general info
	if(ret == true)
	{
		ret = LoadMap();
	}

	// remember to support more any number of tilesets!
	pugi::xml_node tileset;
	for (tileset = mapFile.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true) ret = LoadTilesetDetails(tileset, set);

		if (ret == true) ret = LoadTilesetImage(tileset, set);

		data.tilesets.Add(set);
	}
	ret = true;

	// Load layer info
	pugi::xml_node layer;
	for (layer = mapFile.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.Add(lay);

		pugi::xml_node propertiesNode;
		for (propertiesNode = layer.child("properties"); propertiesNode && ret; propertiesNode = propertiesNode.next_sibling("properties"))
		{
			Properties* property = new Properties();

			ret = LoadProperties(propertiesNode, *property);
			
			lay->properties = *property;
		}
	}

	LoadCollectable();

	if(ret == true)
	{
		
		LOG("Successfully parsed map XML file: %s", filenameGame);
		LOG("Width: %d	Hight: %d", data.width, data.height);
		LOG("TileWidth: %d	TileHight: %d", data.tileWidth, data.tileHeight);
		for (int i = 0; i < data.tilesets.Count(); i++)
		{
			LOG("TileSet ----");
			LOG("Name: %s	FirstGid: %d", data.tilesets.At(i)->data->name.GetString(), data.tilesets.At(i)->data->firstgid);
			LOG("Tile width: %d", data.tilesets.At(i)->data->tileWidth);
			LOG("Tile Height: %d", data.tilesets.At(i)->data->tileHeight);
			LOG("Spacing: %d", data.tilesets.At(i)->data->spacing);
			LOG("Margin: %d", data.tilesets.At(i)->data->margin);
			LOG("NumTilesWidth: %d", data.tilesets.At(i)->data->numTilesWidth);
			LOG("NumTilesHeight: %d", data.tilesets.At(i)->data->numTilesHeight);
		}
		// LOG("CheckPoint count: %d", LoadCheckPoint());
		LoadCheckPoint();
		for (int i = 0; i < data.layers.Count(); i++)
		{
			/*
			LOG("Layer ----");
			LOG("Name: %s", data.layers.At(i)->data->name.GetString());
			LOG("Tile width: %d", data.layers.At(i)->data->width);
			LOG("Tile Height: %d", data.layers.At(i)->data->height);*/
		}
	}

	mapLoaded = ret;

	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = mapFile.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int(0);
		data.height = map.attribute("height").as_int(0);
		data.tileWidth = map.attribute("tilewidth").as_int(0);
		data.tileHeight = map.attribute("tileheight").as_int(0);
		if (strcmp(map.attribute("orientation").as_string("MAPTYPE_UNKNOWN"), "orthogonal")==0)data.type = MAPTYPE_ORTHOGONAL;
		else if (strcmp(map.attribute("orientation").as_string("MAPTYPE_UNKNOWN"), "isometric")==0)data.type = MAPTYPE_ISOMETRIC;
		else if (strcmp(map.attribute("orientation").as_string("MAPTYPE_UNKNOWN"), "staggered")==0)data.type = MAPTYPE_STAGGERED;
	}

	return ret;
}

// Load Tileset attributes
bool Map::LoadTilesetDetails(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;
	
	set->name = tilesetNode.attribute("name").as_string("");
	set->firstgid = tilesetNode.attribute("firstgid").as_int(0);
	set->tileWidth = tilesetNode.attribute("tilewidth").as_int(0);
	set->tileHeight = tilesetNode.attribute("tileheight").as_int(0);
	set->spacing = tilesetNode.attribute("spacing").as_int(0);
	set->margin = tilesetNode.attribute("margin").as_int(0);
	set->numTilesWidth = tilesetNode.attribute("columns").as_int(1);
	set->tilecount= tilesetNode.attribute("tilecount").as_int(0);
	set->numTilesHeight = set->tilecount / set->numTilesWidth;

	return ret;
}

// Load Tileset image
bool Map::LoadTilesetImage(pugi::xml_node& tilesetNode, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tilesetNode.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = app->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string("")));
		set->texWidth = image.attribute("width").as_int(0);
		set->texHeight = image.attribute("height").as_int(0);
	}

	return ret;
}

// Load a single layer
bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string("");
	layer->width = node.attribute("width").as_int(0);
	layer->height = node.attribute("height").as_int(0);
	layer->mapHeight = data.height* data.tileWidth;
	layer->mapWidth = data.width * data.tileWidth;
	int size = layer->width * layer->height;
	layer->data = new uint[size];
	pugi::xml_node tile = node.child("data").child("tile");
	for (int i = 0; i < size; i++)
	{
		layer->data[i] = tile.attribute("gid").as_uint(0);
		tile = tile.next_sibling("tile");
	}

	return ret;
}

int Map::LoadCheckPoint()
{
	if (checKpointsMap.texture != nullptr) app->tex->UnLoad(checKpointsMap.texture);
	checKpointsMap.texture = app->tex->Load("Assets/Textures/checkpoint.png");

	int texW, texH;
	SDL_QueryTexture(checKpointsMap.texture, NULL, NULL, &texW, &texH);
	texW = texW / 9;

	checKpointsMap.checkPointOnAnim->loop = true;
	checKpointsMap.checkPointOnAnim->speed = 0.1f;

	checKpointsMap.checkPointOffAnim->PushBack({ 0,0, texW, texH });
	for (int i = 1; i < 8; i++)
		checKpointsMap.checkPointOnAnim->PushBack({ texW * i,0, texW, texH });
	

	int checkPointCount = 0;
	for (ListItem<MapLayer*>* layer = data.layers.start; layer; layer = layer->next)
	{
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tileId = layer->data->Get(x, y);
				if (tileId == data.tilesets.At(2)->data->firstgid + 2)
				{
					checkPointCount++;
					iPoint checkPointPos(x, y);
					CheckPoints::CP* newCP = new CheckPoints::CP(checkPointPos);
					checKpointsMap.list.Add(newCP);
				}
			}
		}
	}
	return checkPointCount;
}

void Map::LoadCollectable()
{
	for (ListItem<MapLayer*>* layer = data.layers.start; layer; layer = layer->next)
	{
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int tileId = layer->data->Get(x, y);
				if (tileId == data.tilesets.At(3)->data->firstgid)
				{
					app->entityManager->AddEntity(COIN,x,y);
				}
				if (tileId == data.tilesets.At(3)->data->firstgid+3)
				{
					app->entityManager->AddEntity(LIVE, x, y);
				}
			}
		}
	}
}


// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = true;
	pugi::xml_node propertyNode = node.child("property");

	
	for (propertyNode; propertyNode && ret; propertyNode = propertyNode.next_sibling("property"))
	{
		Properties::Property *propertyID = new Properties::Property();
		propertyID->name = propertyNode.attribute("name").as_string("");
		propertyID->value = propertyNode.attribute("value").as_int(0);
		properties.list.Add(propertyID);
	}
	
	return ret;
}
bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	ListItem<MapLayer*>* item;
	item = data.layers.start;

	for (item = data.layers.start; item != NULL; item = item->next)
	{
		MapLayer* layer = item->data;

		if (layer->properties.GetProperty("Nodraw", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width * layer->height];
		memset(map, 1, layer->width * layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y * layer->width) + x;

				int tileId = layer->Get(x, y);
				TileSet* tileset = (tileId > 0) ? GetTilesetFromTileId(tileId) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tileId - tileset->firstgid) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}
