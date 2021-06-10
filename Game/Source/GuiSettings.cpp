#include "App.h"
#include "Render.h"
#include "Fonts.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiSettings.h"
#include "GuiManager.h"


GuiSettings::GuiSettings(iPoint Position, SceneControl* moduleObserver)
{
	active = false;
		
	initialPos = Position;
	Position.y -= 10;

	SDL_Texture* btnTextureAtlas = app->guiManager->uiAtlas;

	sldMusic = new GuiSlider(11, { initialPos.x, Position.y, 65, 20 }, "Music", 0, MIX_MAX_VOLUME, btnTextureAtlas);
	sldMusic->SetObserver(moduleObserver);
	sldMusic->active = false;
	app->guiManager->AddGuiSlider(sldMusic);

	sldFx = new GuiSlider(12, { initialPos.x, Position.y + padding, 80, 20 }, "Fx", 0, MIX_MAX_VOLUME, btnTextureAtlas);
	sldFx->SetObserver(moduleObserver);
	sldFx->active = false;
	app->guiManager->AddGuiSlider(sldFx);

	chBxFullScreen = new GuiCheckBox(13, { initialPos.x, Position.y + padding * 2, 120, 95 }, "FullScreen", app->fullScreen, btnTextureAtlas);
	chBxFullScreen->SetObserver(moduleObserver);
	chBxFullScreen->active = false;
	app->guiManager->AddGuiCheckBox(chBxFullScreen);

	chBxVSync = new GuiCheckBox(14, { initialPos.x, Position.y + padding * 3, 120, 95 }, "V-Sync",false, btnTextureAtlas);
	chBxVSync->SetObserver(moduleObserver);
	chBxVSync->active = false;
	app->guiManager->AddGuiCheckBox(chBxVSync);

	chBxTextSpeed1 = new GuiCheckBox(15, { initialPos.x -25, Position.y + padding * 4 + 20, 120, 95 }, "", false, btnTextureAtlas);
	chBxTextSpeed1->SetObserver(moduleObserver);
	chBxTextSpeed1->active = false;
	app->guiManager->AddGuiCheckBox(chBxTextSpeed1);

	chBxTextSpeed0 = new GuiCheckBox(16, { initialPos.x, Position.y + padding * 4 + 20, 137, 95 }, "Text Speed", false, btnTextureAtlas);
	chBxTextSpeed0->SetObserver(moduleObserver);
	chBxTextSpeed0->active = false;
	app->guiManager->AddGuiCheckBox(chBxTextSpeed0);

	chBxTextSpeed2 = new GuiCheckBox(17, { initialPos.x + 65, Position.y + padding * 4 + 20, 120, 95 }, "", false, btnTextureAtlas);
	chBxTextSpeed2->SetObserver(moduleObserver);
	chBxTextSpeed2->active = false;
	app->guiManager->AddGuiCheckBox(chBxTextSpeed2);

	btnBack = new GuiButton(10, { initialPos.x, Position.y + padding * 5, 183, 91 }, "Back", BACK,btnTextureAtlas);
	btnBack->SetObserver(moduleObserver);
	btnBack->active = false;
	app->guiManager->AddGuiButton(btnBack);
}

GuiSettings::~GuiSettings()
{
}

bool GuiSettings::Update(float dt)
{
	if (!pendingDesactive)
	{
		if (currentIteration < totalIterations)
		{
			hight = EaseCubicIn(currentIteration, 20, 400, totalIterations);
			currentIteration++;
		}
	}
	else
	{
		if (currentIteration < totalIterations)
		{
			hight = EaseCubicIn(currentIteration, 400, -380, totalIterations);
			currentIteration++;
		}
		else
		{
			if(app->sceneManager->GetCurrentScene()->name != "sceneIntro")
				app->guiManager->GetMenuPause()->CloaseMenuSettings();
			pendingDesactive = false;
			currentIteration = 0;
		}
	}
	
	AbleDisableButtons();
	
	return true;
}

bool GuiSettings::Draw()
{
	app->render->DrawTextBox(initialPos.x - app->render->camera.x, initialPos.y - app->render->camera.y, 307, hight, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	if(initialPos.y + padding * 4 - app->render->camera.y + 35 +20 < initialPos.y - app->render->camera.y + hight)
		app->fonts->BlitText(initialPos.x - app->render->camera.x + 150, initialPos.y + padding * 4 - app->render->camera.y + 35, 0, textSpeedOptions, { 60, 43, 13 });
	return true;
}

bool GuiSettings::CleanUp()
{
	RELEASE(sldFx);
	RELEASE(sldMusic);

	RELEASE(chBxFullScreen);
	RELEASE(chBxVSync);
	RELEASE(chBxTextSpeed0);
	RELEASE(chBxTextSpeed1);
	RELEASE(chBxTextSpeed2);

	RELEASE(btnBack);

	return true;
}

void GuiSettings::AbleDisableSetting()
{
	active = !active;
}

void GuiSettings::AbleDisableButtons()
{
	if (sldMusic->bounds.y + sldMusic->bounds.h < initialPos.y - app->render->camera.y + hight) sldMusic->active = true;
	else sldMusic->active = false;
	if (sldFx->bounds.y + sldFx->bounds.h < initialPos.y - app->render->camera.y + hight) sldFx->active = true;
	else sldFx->active = false;
	if (chBxFullScreen->bounds.y + chBxFullScreen->bounds.h-75 < initialPos.y - app->render->camera.y + hight) chBxFullScreen->active = true;
	else chBxFullScreen->active = false;
	if (chBxVSync->bounds.y + chBxVSync->bounds.h-75 < initialPos.y - app->render->camera.y + hight) chBxVSync->active = true;
	else chBxVSync->active = false;
	if (chBxTextSpeed0->bounds.y + chBxTextSpeed0->bounds.h-75 < initialPos.y - app->render->camera.y + hight) chBxTextSpeed0->active = true, chBxTextSpeed1->active = true, chBxTextSpeed2->active = true;
	else chBxTextSpeed0->active = false, chBxTextSpeed1->active = false, chBxTextSpeed2->active = false;
	if (btnBack->bounds.y + btnBack->bounds.h < initialPos.y - app->render->camera.y + hight) btnBack->active = true;
	else btnBack->active = false;
}

void GuiSettings::DesactiveSettingMenu()
{
	pendingDesactive = true;
	currentIteration = 0;

	app->audio->PlayFx(app->guiManager->fxBtnPressed);
}

void GuiSettings::MovePosition()
{
	
	int x = initialPos.x - app->render->camera.x + 30;
	int y = initialPos.y - app->render->camera.y + 38;

	sldMusic->bounds.x = x;
	sldMusic->bounds.y = y;

	sldFx->bounds.x = x - 15;
	sldFx->bounds.y = y + padding * 1;

	chBxFullScreen->bounds.x = x;
	chBxFullScreen->bounds.y = y + padding * 2;

	chBxVSync->bounds.x = x;
	chBxVSync->bounds.y = y + padding * 3;

	chBxTextSpeed0->bounds.x = x;
	chBxTextSpeed0->bounds.y = y + padding * 4 + 20;

	chBxTextSpeed1->bounds.x = x - 25;
	chBxTextSpeed1->bounds.y = y + padding * 4 + 20;

	chBxTextSpeed2->bounds.x = x + 65;
	chBxTextSpeed2->bounds.y = y + padding * 4 + 20;
	
	btnBack->bounds.x = x;
	btnBack->bounds.y = y + padding * 5;

}

