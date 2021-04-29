#include "App.h"
#include "GuiStatsMenu.h"
#include "Audio.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiManager.h"
#include "Fonts.h"

#include "Defs.h"
#include "Log.h"

GuiStatsMenu::GuiStatsMenu(iPoint position, SceneControl* moduleObserver, SDL_Texture* textureAtlas)
{
	screenRect = { 0, 0, WINDOW_W, WINDOW_H };

	initialPos = position;

	observer = moduleObserver;
	color = { 60, 43, 13 };

	active = false;
}

GuiStatsMenu::~GuiStatsMenu()
{
}

bool GuiStatsMenu::Update(float dt)
{
	bool ret = true;

	if (active)
	{
		//Update stuff
		if (app->guiManager->openBookAnim->HasFinished() || app->guiManager->leftBook->HasFinished() || app->guiManager->rightBook->HasFinished())
		{
			introBook = false;
			changingPage = false;
			app->guiManager->openBookAnim->Reset();
			app->guiManager->rightBook->Reset();
			app->guiManager->leftBook->Reset();
			currentAnim = app->guiManager->idleBook;
		}

		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			app->audio->PlayFx(app->guiManager->bookClose);
			app->sceneManager->SetPause(false);
			AbleDisableMenu();
		}
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN && !introBook && !changingPage)
		{
			page.numPage++;
			ChangePage();
			currentAnim = app->guiManager->rightBook;
		}
		if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN && !introBook && !changingPage)
		{
			page.numPage--;
			ChangePage();
			currentAnim = app->guiManager->leftBook;
		}

		if (currentAnim == app->guiManager->openBookAnim) app->guiManager->openBookAnim->Update();
		if (currentAnim == app->guiManager->leftBook) app->guiManager->leftBook->Update();
		if (currentAnim == app->guiManager->rightBook) app->guiManager->rightBook->Update();
	}

	return ret;
}

bool GuiStatsMenu::PostUpdate()
{

	if (active)
	{
		app->render->DrawRectangle({ screenRect.x, screenRect.y, WINDOW_W, WINDOW_H }, 0, 0, 0, 200);

		app->render->DrawTexture(app->guiManager->bookMenu, relativePosition.x, relativePosition.y, &currentAnim->GetCurrentFrame(), 5);

		if (!introBook && !changingPage)
		{
			int posX = -app->render->camera.x + 220;
			int posY = -app->render->camera.y + 120;

			DrawTitleStats(posX, posY);			
		}
	}

	return true;
}

void GuiStatsMenu::DrawTitleStats(int posX, int& posY)
{
	sprintf_s(textStats, 15, "Stats");
	app->fonts->BlitText(posX, posY, 2, textStats, color);

	posY += 45;
	sprintf_s(textStats, 15, "HP");
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	rectBar = { posX +10, posY+25, wRectBar,20 };
	rectBar.w = page.health * wRectBar / page.maxHealth;
	DrawBarLive();

	posY += 60;
	sprintf_s(textStats, 15, "MP");
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	rectBar = { posX + 10, posY + 25, wRectBar,20 };
	rectBar.w = page.mana * wRectBar / page.maxMana;
	//

	posY += 60;
	sprintf_s(textStats, 15, "Attack");
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	posY += 60;
	sprintf_s(textStats, 15, "Defense");
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	posY += 60;
	sprintf_s(textStats, 15, "Speed");
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	posY += 60;
	sprintf_s(textStats, 15, "Spells");
	app->fonts->BlitText(posX, posY, 2, textStats, color);

	// Draw Numbers
	posX = -app->render->camera.x + 220 + 280;
	posY = -app->render->camera.y + 120 + 35;

	sprintf_s(textStats, 15, "%d/%d", page.health, page.maxHealth);
	app->fonts->BlitText(posX, posY, 0, textStats, color);

}

bool GuiStatsMenu::CleanUp()
{
	active = false;

	return true;
}

bool GuiStatsMenu::Event(GuiControl* control)
{

	//Button stuff

	return true;
}

void GuiStatsMenu::AbleDisableMenu()
{
	active = !active;

	app->guiManager->openBookAnim->Reset();
	app->guiManager->rightBook->Reset();
	app->guiManager->leftBook->Reset();

	if (active)
	{
		MovePosition();
	}
}

void GuiStatsMenu::MovePosition()
{
	relativePosition.x = -app->render->camera.x + initialPos.x;
	relativePosition.y = -app->render->camera.y + initialPos.y;
	screenRect.x = -app->render->camera.x;
	screenRect.y = -app->render->camera.y;
	InicializeStats();
}

void GuiStatsMenu::ChangePage()
{
	app->audio->PlayFx(app->guiManager->changePage);
	changingPage = true;
	if (page.numPage > maxNumPages) page.numPage = 1;
	if (page.numPage < 1) page.numPage = maxNumPages;
	InicializeStats();
}
void GuiStatsMenu::InicializeStats()
{
	int level = 1;

	switch (page.numPage)
	{
	case 1: // Kenzie
		level = app->player->playerData.level;
		page.attack = level * 3.5 + 9.5;
		page.defense = level * 1.5 + 6.5;
		page.speed = level * 2.5 + 7.5;
		page.maxHealth = level * 2 + 6;
		page.health = app->player->playerData.health;
		page.maxMana = level * 3 + 8;
		page.exp = app->player->playerData.exp;
		page.maxExp = CalculateExp(level);

		break;
	case 2: // Keiler
		level = app->player->GetPartners()[0].level;
		page.attack = 1.5 * level + 6.5;
		page.defense = 1.5 * level + 5.5;
		page.speed = 2.5 * level + 8.5;
		page.maxHealth = 4 * level + 10;
		page.health = app->player->GetPartners()[0].health;
		page.maxMana = 2.5 * level + 7.5;
		page.exp = app->player->GetPartners()[0].exp;
		page.maxExp = CalculateExp(level);

		break;
	case 3: // Isrra
		level = app->player->GetPartners()[1].level;
		page.attack = 2 * level + 7;
		page.defense = 2 * level + 7;
		page.speed = 2.5 * level + 7.5;
		page.maxHealth = 2.5 * level + 7.5;
		page.health = app->player->GetPartners()[1].health;
		page.maxMana = 3.5 * level + 8.5;
		page.exp = app->player->GetPartners()[1].exp;
		page.maxExp = CalculateExp(level);

		break;
	case 4: // Brenda
		level = app->player->GetPartners()[2].level;
		page.attack = 1.5 * level + 6.5;
		page.defense = 3.5 * level + 9.5;
		page.speed = 1.5 * level + 6.5;
		page.maxHealth = 3.5 * level + 9.5;
		page.health = app->player->GetPartners()[2].health;
		page.maxMana = 1.5 * level + 6.5;
		page.exp = app->player->GetPartners()[2].exp;
		page.maxExp = CalculateExp(level);

		break;
	default:
		break;
	}
}
int GuiStatsMenu::CalculateExp(int level)
{
	return (0.04 * (level * level * level) + 0.8 * (level * level) + 2 * level) * 3.5;
}

void GuiStatsMenu::DrawBarLive()
{
	if (rectBar.w > wRectBar / 2)
	{
		app->render->DrawRectangle(rectBar, green.r, green.g, green.b);
		rectBar.w = wRectBar;
		app->render->DrawRectangle(rectBar, 0, 98, 0, 255, false);
	}
	else if (rectBar.w < wRectBar / 4)
	{
		app->render->DrawRectangle(rectBar, red.r, red.g, red.b);
		rectBar.w = wRectBar;
		app->render->DrawRectangle(rectBar, 115, 0, 0, 255, false);
	}
	else if (rectBar.w <= wRectBar / 2)
	{
		app->render->DrawRectangle(rectBar, yellow.r, yellow.g, yellow.b);
		rectBar.w = wRectBar;
		app->render->DrawRectangle(rectBar, 130, 110, 0, 255, false);
	}
}
