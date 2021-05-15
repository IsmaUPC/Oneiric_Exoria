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


bool TeleportNode::CleanUp()
{

	return true;
}

bool TeleportNode::Clear()
{

	return true;
}
