
#ifndef _TELEPORT_NODE_H_
#define _TELEPORT_NODE_H_

#include "Point.h"
#include "Log.h"


#include "PugiXml\src\pugixml.hpp"

enum TpNodesTypes
{

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

	bool Awake(pugi::xml_node&);
	bool CleanUp();



public:

	iPoint position;
	int idFloor;
	int idNode;
	int nextIdFloor;
	TpNodesTypes typeTpNode;
};
#endif // _TELEPORT_NODE_H_

