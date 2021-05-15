#ifndef __TP_NODE_MANAGER_H__
#define __TP_NODE_MANAGER_H__

#include "Module.h"
#include "SceneControl.h"
#include "Window.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include "TeleportNode.h"

#include "App.h"
#include "Player.h"
#include "Audio.h"

class TpNodeManager : public Module
{
public:

	TpNodeManager();

	// Destructor
	virtual ~TpNodeManager();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:
	

	void SpawnPlayerTpNode(TeleportNode* originTpNode);
	List<TeleportNode*>* FindTpNodeList(uint typeNode);
	TeleportNode* FindNodeTpInPlayer(uint typeTilePlayer);
	TeleportNode* FindNodeTpById(uint typeTilePlayer, int nodeId);

private:

public:


};

#endif // __TP_NODE_MANAGER_H__