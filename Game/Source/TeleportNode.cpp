#include "TeleportNode.h"



TeleportNode::TeleportNode(iPoint pos, int idFloor, int nextIdFloor, TpNodesTypes typeTpNode)
{
	position = pos;
	this->idFloor = idFloor;
	this->nextIdFloor = nextIdFloor;
	this->typeTpNode = typeTpNode;
}


TeleportNode::~TeleportNode()
{}




bool TeleportNode::Awake(pugi::xml_node& config)
{
	LOG("Loading TeleportNode Parser");
	bool ret = true;


	return ret;
}

bool TeleportNode::CleanUp()
{

	return true;
}
