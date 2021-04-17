#ifndef __SCENEINTRO_H__
#define __SCENEINTRO_H__

#include "Scene.h"
#include "Animation.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiSettings.h"

struct SDL_Texture;

class SceneIntro : public SceneControl
{
public:

	SceneIntro();

	// Destructor
	virtual ~SceneIntro();

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

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

	void CloaseMenuSettings();

	bool LoadState(pugi::xml_node& data);
	bool SaveState(pugi::xml_node& data)const;

	void ComprobeState(int id);

private:

	SDL_Texture* bgIntro = nullptr;
	SDL_Texture* logoIntro = nullptr;
	SDL_Texture* bigCloud = nullptr;
	SDL_Texture* smallCloud = nullptr;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	bool transition = false;

	fPoint bCloudPos = { 0,0 };
	fPoint bCloudPos2 = { 0,0 };
	fPoint sCloudPos = { 0,0 };
	fPoint sCloudPos2 = { 0,0 };

	pugi::xml_document sceneFile;
	pugi::xml_node sceneStateFile;

	GuiButton* btnPlay = nullptr;
	GuiButton* btnContinue = nullptr;
	GuiButton* btnRemove = nullptr;
	GuiButton* btnSettings = nullptr;
	GuiButton* btnCredits = nullptr;
	GuiButton* btnExit = nullptr;

	GuiSettings* menuSettings = nullptr;
	uint startFx = -1;
};

#endif // __SCENEINTRO_H__



