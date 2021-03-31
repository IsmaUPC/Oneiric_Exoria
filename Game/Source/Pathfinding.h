#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "PQueue.h"
#include "Point.h"
#include "DynArray.h"
#include "List.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

class PathFinding : public Module
{
public:

	PathFinding();

	// Destructor
	~PathFinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);
	// To request all tiles involved in the last generated path
	DynArray<iPoint>* GetLastPath();
	PQueue<iPoint>* GetFrontier();
	List<iPoint>* GetVisited();
	List<iPoint>* GetBreadcrumbs();

	void ResetPath(iPoint start);
	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	bool PropagateAStar(const iPoint&);
	void ComputePathAStar(const iPoint& origin, const iPoint& destination);

private:

	// size of the map
	uint width;
	uint height;
	PQueue<iPoint> frontier;
	List<iPoint> visited;
	List<iPoint> breadcrumbs;
	// all map walkability values [0..255]
	uchar* map;
	bool destinationIsFind;
	// we store the created path here
	DynArray<iPoint> lastPath;
};

#endif // __PATHFINDING_H__


