#include "App.h"
#include "Render.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiSettings.h"


GuiSettings::GuiSettings(iPoint Position, SceneControl* moduleObserver)
{
	active = false;

	initialPos = Position;

	SDL_Texture* btnTextureAtlas = app->sceneManager->btnTextureAtlas;

	sldMusic = new GuiSlider(11, { initialPos.x, initialPos.y, 183, 91 }, "MUSIC", 0, 100, btnTextureAtlas);
	sldMusic->SetObserver(moduleObserver);

	sldFx = new GuiSlider(12, { initialPos.x, initialPos.y + padding, 183, 91 }, "FX", 0, 100, btnTextureAtlas);
	sldFx->SetObserver(moduleObserver);

	chBxFullScreen = new GuiCheckBox(13, { initialPos.x, initialPos.y + padding * 2, 183, 91 }, "FULLSC", app->fullScreen, btnTextureAtlas);
	chBxFullScreen->SetObserver(moduleObserver);

	chBxVSync = new GuiCheckBox(14, { initialPos.x, initialPos.y + padding * 3, 183, 91 }, "VSYNC",false, btnTextureAtlas);
	chBxVSync->SetObserver(moduleObserver);

	chBxVSync->state = GuiControlState::DISABLED;

	btnBack = new GuiButton(10, { initialPos.x, initialPos.y + padding * 4, 88, 88 }, "", BACK,btnTextureAtlas);
	btnBack->SetObserver(moduleObserver);

}

GuiSettings::~GuiSettings()
{

}

bool GuiSettings::Update(float dt)
{
	if (active)
	{
		sldMusic->Update(dt);
		sldFx->Update(dt);
		chBxFullScreen->Update(dt);
		chBxVSync->Update(dt);
		btnBack->Update(dt);
	}

	return true;
}

bool GuiSettings::Draw()
{
	if (active)
	{
		sldMusic->Draw();
		sldFx->Draw();
		chBxFullScreen->Draw();
		chBxVSync->Draw();
		btnBack->Draw();
	}

	return true;
}

bool GuiSettings::CleanUp()
{
	active = false;

	delete sldMusic;
	delete sldFx;
	delete chBxFullScreen;
	delete chBxVSync;
	delete btnBack;

	sldMusic = NULL;
	sldFx = NULL;
	chBxFullScreen = NULL;
	chBxVSync = NULL;
	btnBack = NULL;

	return true;
}

void GuiSettings::MovePosition()
{
	
	int x = initialPos.x - app->render->camera.x;
	int y = initialPos.y - app->render->camera.y;

	sldMusic->bounds.x = x;
	sldMusic->bounds.y = y;

	sldFx->bounds.x = x;
	sldFx->bounds.y = y + padding * 1;

	chBxFullScreen->bounds.x = x;
	chBxFullScreen->bounds.y = y + padding * 2;

	chBxVSync->bounds.x = x;
	chBxVSync->bounds.y = y + padding * 3;

	btnBack->bounds.x = x+(183/4);
	btnBack->bounds.y = y + padding * 4;

}

