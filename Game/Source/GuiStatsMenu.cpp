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

	//MenuMagic
	menuMagic = new GuiMenuMagic({ 0, WINDOW_H / 2 +240 }, observer);

	//menuMagic->SetIdTurn(turnSort[turn].entityData.type);

	//menuMagic->AbleDisableMagic();
	//menuMagic->MovePosition();

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
			menuMagic->AbleDisableMagic();
			menuMagic->close->active = false;
		}

		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			app->audio->PlayFx(app->guiManager->bookClose);
			app->sceneManager->SetPause(false);
			AbleDisableMenu();
			menuMagic->AbleDisableMagic();
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
	app->render->DrawRectangle(rectBar, 157,0,208);
	rectBar.w = wRectBar;
	app->render->DrawRectangle(rectBar, 87,0,115,255,false);

	posY += 60;
	sprintf_s(textStats, 15, "Attack");
	app->fonts->BlitText(posX, posY, 0, textStats, color);
	rectBar.y += 60;
	rectBar.h = 10;
	app->render->DrawRectangle(rectBar, 0, 3, 111);

	posY += 50;
	sprintf_s(textStats, 15, "Defense");
	app->fonts->BlitText(posX, posY, 0, textStats, color);
	rectBar.y += 50;
	app->render->DrawRectangle(rectBar, 0, 3, 111);

	posY += 50;
	sprintf_s(textStats, 15, "Speed");
	app->fonts->BlitText(posX, posY, 0, textStats, color);
	rectBar.y += 50;
	app->render->DrawRectangle(rectBar, 0, 3, 111);

	posY += 60;
	sprintf_s(textStats, 15, "Spells");
	app->fonts->BlitText(posX, posY, 2, textStats, color);

	// Draw Numbers
	posX = -app->render->camera.x + 220 + 280;
	posY = -app->render->camera.y + 120 + 45;

	sprintf_s(textStats, 15, "%d/%d", page.health, page.maxHealth);
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	posY += 60;
	sprintf_s(textStats, 15, "%d/%d", page.mana, page.maxMana);
	app->fonts->BlitText(posX, posY, 0, textStats, color);

	posY += 60;
	posX -= 50;
	sprintf_s(textStats, 15, "%d", page.attack);
	app->fonts->BlitText(posX, posY, 0, textStats, color);
	posX += 60;
	sprintf_s(textStats, 15, "(+0)"); // Change for boost %d
	app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, });

	posY += 50;
	posX -= 60;
	sprintf_s(textStats, 15, "%d", page.defense);
	app->fonts->BlitText(posX, posY, 0, textStats, color);
	posX += 60;
	sprintf_s(textStats, 15, "(+0)"); // Change for boost %d
	app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, });

	posY += 50;
	posX -= 60;
	sprintf_s(textStats, 15, "%d", page.speed);
	app->fonts->BlitText(posX, posY, 0, textStats, color);
	posX += 60;
	sprintf_s(textStats, 15, "(+0)"); // Change for boost %d
	app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, });

	// Draw Spells

	// Draw Avatar
	posX = WINDOW_W / 2 - app->render->camera.x + 80;
	posY = -app->render->camera.y + 120;
	app->render->DrawTextBox(posX, posY, wRectBar, 200, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	app->render->DrawRectangle({ posX + wRectBar / 2 - 9, posY + 30,120 +14,128+8 }, 227, 207, 127);
	app->render->DrawTexture(app->sceneManager->GetPlayerTexture(), posX + wRectBar / 2-15, posY+30, &page.avatar);
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
	menuMagic->AbleDisableMagic();
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
		page.mana = app->player->playerData.mana;
		page.exp = app->player->playerData.exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(KENZIE_);
		page.avatar = { 0,372,145,145 };

		break;
	case 2: // Keiler
		level = app->player->GetPartners()[0].level;
		page.attack = 1.5 * level + 6.5;
		page.defense = 1.5 * level + 5.5;
		page.speed = 2.5 * level + 8.5;
		page.maxHealth = 4 * level + 10;
		page.health = app->player->GetPartners()[0].health;
		page.maxMana = 2.5 * level + 7.5;
		page.mana = app->player->GetPartners()[0].mana;
		page.exp = app->player->GetPartners()[0].exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(KEILER_);
		page.avatar = { 145,372,145,145 };

		break;
	case 3: // Isrra
		level = app->player->GetPartners()[1].level;
		page.attack = 2 * level + 7;
		page.defense = 2 * level + 7;
		page.speed = 2.5 * level + 7.5;
		page.maxHealth = 2.5 * level + 7.5;
		page.health = app->player->GetPartners()[1].health;
		page.maxMana = 3.5 * level + 8.5;
		page.mana = app->player->GetPartners()[1].mana;
		page.exp = app->player->GetPartners()[1].exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(ISRRA_);
		page.avatar = { 0,517,145,145 };

		break;
	case 4: // Brenda
		level = app->player->GetPartners()[2].level;
		page.attack = 1.5 * level + 6.5;
		page.defense = 3.5 * level + 9.5;
		page.speed = 1.5 * level + 6.5;
		page.maxHealth = 3.5 * level + 9.5;
		page.health = app->player->GetPartners()[2].health;
		page.maxMana = 1.5 * level + 6.5;
		page.mana = app->player->GetPartners()[2].mana;
		page.exp = app->player->GetPartners()[2].exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(BRENDA_);
		page.avatar = { 145,517,145,145 };

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
