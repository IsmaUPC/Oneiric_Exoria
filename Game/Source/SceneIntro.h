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

	SDL_Texture* bgIntro;
	SDL_Texture* logoIntro;
	SDL_Texture* bigCloud;
	SDL_Texture* smallCloud;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	bool transition;

	fPoint bCloudPos;
	fPoint bCloudPos2;
	fPoint sCloudPos;
	fPoint sCloudPos2;

	pugi::xml_document sceneFile;
	pugi::xml_node sceneStateFile;

	GuiButton* btnPlay;
	GuiButton* btnContinue;
	GuiButton* btnRemove;
	GuiButton* btnSettings;
	GuiButton* btnCredits;
	GuiButton* btnExit;

	GuiSettings* menuSettings;
	uint startFx;
};

#endif // __SCENEINTRO_H__



