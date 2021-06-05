#ifndef __SCENEWIN_H__
#define __SCENEWIN_H__

#include "Scene.h"
#include "GuiButton.h"

struct SDL_Texture;

class SceneWin :public SceneControl
{
public:

	SceneWin(SceneType type);

	// Destructor
	virtual ~SceneWin();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);
	void CloudsUpdate();

	// Called before all Updates
	bool PostUpdate();
	void CloudsDraw();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

	int GetNumThisScene()
	{
		return 0;
	};

	void AbleButtons();

	bool LoadState(pugi::xml_node& data);
	bool SaveState(pugi::xml_node& data)const;


private:

	SDL_Texture* bgIntro = nullptr;
	SDL_Texture* logo = nullptr;
	SDL_Texture* cloud = nullptr;
	SDL_Texture* ending = nullptr;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;

	GuiButton* btnContinue = nullptr;
	GuiButton* btnBackToTitle = nullptr;

	uint fxStart = -1;
	uint fxTittle = -1;
	uint fxFlash = -1;

	fPoint sBackCloudPos = { 0,0 };
	fPoint bBackCloudPos = { 0,0 };

	fPoint bCloudPos = { 0,0 };
	fPoint bCloudPos2 = { 0,0 };
	fPoint sCloudPos = { 0,0 };
	fPoint sCloudPos2 = { 0,0 };

	//Easings title
	float currentIteration;
	float totalIterations;
	float initialPosition;
	float deltaPosition;
	int hight = WINDOW_H / 2;

	bool flash = false;
	float logoAlpha = 0;
	float timeCounter = 0;
	int state = 0;
	float angle = 0;
};

#endif // !__SCENEWIN_H__
