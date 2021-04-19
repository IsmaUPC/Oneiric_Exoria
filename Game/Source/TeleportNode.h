
#ifndef _TELEPORT_NODE_H_
#define _TELEPORT_NODE_H_

#include "Point.h"
#include "Log.h"


#include "PugiXml\src\pugixml.hpp"

enum TpNodesTypes
{
	UNKNOW = 1,
	DOWN_LADDER_NODE = 2,
	UP_LADDER_NODE = 3,
	DOWN_HALL_NODE = 4,
	UP_HALL_NODE = 5,

};

class TeleportNode
{
public:

	TeleportNode(iPoint pos, int idFloor,int nextIdFloor, TpNodesTypes typeTpNode);

	virtual ~TeleportNode();
	bool CleanUp();
	bool Clear();

public:

	iPoint position = { 0,0 };
	int idFloor = 0;
	int idNode = 0;
	int nextIdFloor = 0;
	TpNodesTypes typeTpNode = UNKNOW;
};
#endif // _TELEPORT_NODE_H_

