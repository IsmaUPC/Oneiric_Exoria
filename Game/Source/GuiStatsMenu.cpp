#include "App.h"
#include "GuiStatsMenu.h"
#include "Audio.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "GuiManager.h"
#include "Fonts.h"
#include "Player.h"

#include "Defs.h"
#include "Log.h"

GuiStatsMenu::GuiStatsMenu(iPoint position, SceneControl* moduleObserver, SDL_Texture* textureAtlas)
{
	screenRect = { 0, 0, WINDOW_W, WINDOW_H };

	padding = 30;

	initialPos = position;

	observer = moduleObserver;
	color = { 60, 43, 13 };

	//MenuMagic
	menuMagic = new GuiMenuMagic({ 300, WINDOW_H / 2 +140 }, observer);

	//Item options buttons
	btnUseItem = new GuiButton(7, { -app->render->camera.x + position.x + WINDOW_W/4 - 40, -app->render->camera.y + position.y + WINDOW_H/2 + padding * 1, 183, 91 }, "Equip", RECTANGLE, textureAtlas);
	btnUseItem->SetObserver(moduleObserver);
	btnUseItem->active = false;
	btnUseItem->state = GuiControlState::DISABLED;
	app->guiManager->AddGuiButton(btnUseItem);

	btnUnEquipItem = new GuiButton(8, { -app->render->camera.x + position.x + WINDOW_W / 4 - 40, -app->render->camera.y + position.y + WINDOW_H / 2 + padding * 2, 183, 91 }, "Unequip", RECTANGLE, textureAtlas);
	btnUnEquipItem->SetObserver(moduleObserver);
	btnUnEquipItem->active = false;
	btnUnEquipItem->state = GuiControlState::DISABLED;
	app->guiManager->AddGuiButton(btnUnEquipItem);

	btnDelItem = new GuiButton(9, { -app->render->camera.x + position.x + WINDOW_W / 4 - 40, -app->render->camera.y + position.y + WINDOW_H / 2 + padding * 2, 183, 91 }, "Delete", RECTANGLE, textureAtlas);
	btnDelItem->SetObserver(moduleObserver);
	btnDelItem->active = false;
	btnDelItem->state = GuiControlState::DISABLED;
	app->guiManager->AddGuiButton(btnDelItem);

	active = false;
}

GuiStatsMenu::~GuiStatsMenu()
{
}

bool GuiStatsMenu::Update(float dt_)
{
	bool ret = true;
	dt = dt_;

	if (active)
	{
		//Update stuff

		if ((app->guiManager->openBookAnim->HasFinished() || app->guiManager->leftBook->HasFinished() || app->guiManager->rightBook->HasFinished()) && !closingBook)
		{
			introBook = false;
			changingPage = false;
			app->guiManager->openBookAnim->Reset();
			app->guiManager->rightBook->Reset();
			app->guiManager->leftBook->Reset();
			if (pageType == STATS)
			{
				menuMagic->AbleDisableMagic();
				menuMagic->close->active = false;
				menuMagic->MovePosition();
				InicializeStats();
			}
			if (pageType == INVENTORY && app->player->inventory.start != nullptr)
			{
				btnUseItem->active = true;
				btnDelItem->active = true;
				btnUnEquipItem->active = true;
				currentItem = app->player->inventory.start->data;
			}
			currentAnim = app->guiManager->idleBook;
			app->fonts->ResetH();

			btnUseItem->bounds.x = -app->render->camera.x + initialPos.x + WINDOW_W / 2 - 100;
			btnUseItem->bounds.y = -app->render->camera.y + initialPos.y + WINDOW_H / 2 + padding * 1 + 205;

			btnUnEquipItem->bounds.x = -app->render->camera.x + initialPos.x + WINDOW_W / 2 - 100;
			btnUnEquipItem->bounds.y = -app->render->camera.y + initialPos.y + WINDOW_H / 2 + padding * 2 + 205;

			btnDelItem->bounds.x = -app->render->camera.x + initialPos.x + WINDOW_W / 2 - 100;
			btnDelItem->bounds.y = -app->render->camera.y + initialPos.y + WINDOW_H / 2 + padding * 3 + 205;

		}
		
		if (btnUseItem->state == GuiControlState::DISABLED && !introBook && !changingPage && !closingBook && !selectingPlayer)
		{
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || app->input->pads[0].l1)
			{
				pageType = static_cast<PageType>(pageType - 1);
				ChangePages();
				currentAnim = app->guiManager->leftBook;
				btnUseItem->active = false;
				btnDelItem->active = false;
				btnUnEquipItem->active = false;
			}
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || app->input->pads[0].r1)
			{
				pageType = static_cast<PageType>(pageType + 1);
				ChangePages();
				currentAnim = app->guiManager->rightBook;
				btnUseItem->active = false;
				btnDelItem->active = false;
				btnUnEquipItem->active = false;
			}
			if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b)
			{
				currentAnim = app->guiManager->closeBook;
				closingBook = true;
				if (pageType == STATS) menuMagic->AbleDisableMagic();
				app->audio->PlayFx(app->guiManager->fxBookClose);
				btnUseItem->active = false;
				btnDelItem->active = false;
				btnUnEquipItem->active = false;
			}
		}

		switch (pageType)
		{
		case STATS:
			UpdateStats();
			break;
		case INVENTORY:
			UpdateInventory();
			break;
		default:
			break;
		}

		CloseBook();

		currentAnim->Update();
	}

	return ret;
}

void GuiStatsMenu::UpdateInventory()
{
	if (!introBook && !changingPage && !closingBook && currentItem != nullptr)
	{
		if (btnUseItem->state == GuiControlState::DISABLED && !selectingPlayer)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || app->input->pads[0].up)
			{
				if (currentItem == app->player->inventory.start->data) currentItem = app->player->inventory.end->data;
				else currentItem = app->player->inventory.At(app->player->inventory.Find(currentItem))->prev->data;
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || app->input->pads[0].down)
			{
				if (currentItem == app->player->inventory.end->data) currentItem = app->player->inventory.start->data;
				else currentItem = app->player->inventory.At(app->player->inventory.Find(currentItem))->next->data;
			}

			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || app->input->pads[0].a)
			{
				app->guiManager->missClick = true;
				btnUseItem->state = GuiControlState::FOCUSED;
				btnDelItem->state = GuiControlState::NORMAL;
				btnUnEquipItem->state = GuiControlState::NORMAL;
				CheckSelectPlayer();
			}
			if(currentItem->equiped) btnUnEquipItem->state = GuiControlState::NORMAL;
			else btnUnEquipItem->state = GuiControlState::DISABLED;
		}
		if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b && !selectingPlayer)
		{
			btnUseItem->state = GuiControlState::DISABLED;
			btnDelItem->state = GuiControlState::DISABLED;
			btnUnEquipItem->state = GuiControlState::DISABLED;
		}
		if (selectingPlayer)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || app->input->pads[0].up)
			{
				if (characterSelected == 1) characterSelected = 4;
				else characterSelected--;
				CheckSelectPlayer();
			}
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || app->input->pads[0].down)
			{
				if (characterSelected == 4) characterSelected = 1;
				else characterSelected++;
				CheckSelectPlayer();
			}
			if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || app->input->pads[0].a)
			{
				app->player->itemManager->UseItem(currentItem, selectPlayer);
				currentItem = app->player->inventory.start->data;
				selectingPlayer = false;
			}
			if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->pads[0].b)
			{
				btnUseItem->state = GuiControlState::FOCUSED;
				btnDelItem->state = GuiControlState::NORMAL;
				btnUnEquipItem->state = GuiControlState::NORMAL;
				selectingPlayer = false;
			}
		}
	}
	//debugg
	if (app->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	{
		if (app->player->inventory.start != nullptr)
		{
			app->player->itemManager->UseItem(app->player->inventory.start->data, app->player);
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		app->player->itemManager->AddItem(3);
		app->player->itemManager->AddItem(4);
		app->player->itemManager->AddItem(5);
		app->player->itemManager->AddItem(6);
	}
}

void GuiStatsMenu::CheckSelectPlayer()
{
	switch (characterSelected)
	{
	case 1:
		selectPlayer = app->player;
		break;
	case 2:
		selectPlayer = &app->player->GetPartners()[0];
		break;
	case 3:
		selectPlayer = &app->player->GetPartners()[1];
		break;
	case 4:
		selectPlayer = &app->player->GetPartners()[2];
		break;
	default:
		break;
	}
}

void GuiStatsMenu::UpdateStats()
{
	if ((app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || app->input->pads[0].down) && !introBook && !changingPage && !closingBook)
	{
		page.numPage++;
		selectingMagic = false;
		ChangeStatCharacter();
		currentAnim = app->guiManager->rightBook;
	}
	if ((app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || app->input->pads[0].up) && !introBook && !changingPage && !closingBook)
	{
		page.numPage--;
		selectingMagic = false;
		ChangeStatCharacter();
		currentAnim = app->guiManager->leftBook;
	}

	if ((app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN || app->input->pads[0].y) && !introBook && !changingPage && !closingBook)
	{
		if (menuMagic->magic1->state != GuiControlState::NORMAL) menuMagic->magic1->state = GuiControlState::NORMAL;
		if (menuMagic->magic2->state != GuiControlState::NORMAL) menuMagic->magic2->state = GuiControlState::NORMAL;
		if (menuMagic->magic3->state != GuiControlState::NORMAL) menuMagic->magic3->state = GuiControlState::NORMAL;
		if (menuMagic->magic4->state != GuiControlState::NORMAL) menuMagic->magic4->state = GuiControlState::NORMAL;
		selectingMagic = false;
	}
	if (menuMagic->magic1->state != GuiControlState::NORMAL) magicSelected = 1, selectingMagic = true;
	if (menuMagic->magic2->state != GuiControlState::NORMAL) magicSelected = 2, selectingMagic = true;
	if (menuMagic->magic3->state != GuiControlState::NORMAL) magicSelected = 3, selectingMagic = true;
	if (menuMagic->magic4->state != GuiControlState::NORMAL) magicSelected = 4, selectingMagic = true;
}

void GuiStatsMenu::CloseBook()
{
	if (app->guiManager->closeBook->HasFinished())
	{
		AbleDisableMenu();
		app->sceneManager->SetPause(false);
		closingBook = false;
		app->guiManager->closeBook->Reset();
	}
}

bool GuiStatsMenu::PostUpdate()
{
	if (active)
	{
		app->render->DrawRectangle({ screenRect.x, screenRect.y, WINDOW_W, WINDOW_H }, 0, 0, 0, 200);

		app->render->DrawTexture(app->guiManager->bookMenu, relativePosition.x, relativePosition.y, &currentAnim->GetCurrentFrame(), 5);

		int posX = -app->render->camera.x + 220;
		int posY = -app->render->camera.y + 120;

		int posY2 = -app->render->camera.y + 120;

		if (!introBook && !closingBook)
		{
			SDL_Rect bookMarkRect = { 80,374,50,36 };
			switch (pageType)
			{
			case STATS:
				//Draw bookmarks
				DrawBookMarks(posX, posY2, page.numPage);
				break;
			case INVENTORY:
				if (selectingPlayer)
				{
					DrawBookMarks(posX, posY2, characterSelected);
					switch (characterSelected)
					{
					case 1:
						app->render->DrawTextureFlip(app->guiManager->handCursor, posX + 912, posY + 200, &app->guiManager->handAnim->GetCurrentFrame());
						break;
					case 2:
						app->render->DrawTextureFlip(app->guiManager->handCursor, posX + 912, posY + 240, &app->guiManager->handAnim->GetCurrentFrame());
						break;
					case 3:
						app->render->DrawTextureFlip(app->guiManager->handCursor, posX + 912, posY + 280, &app->guiManager->handAnim->GetCurrentFrame());
						break;
					case 4:
						app->render->DrawTextureFlip(app->guiManager->handCursor, posX + 912, posY + 320, &app->guiManager->handAnim->GetCurrentFrame());
						break;
					default:
						break;
					}
				}
				else for (int i = 0; i < 4; i++) app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY2 + 200 + i * 40, &bookMarkRect);
				break;
			default:
				break;
			}
		}

		if (!introBook && !changingPage && !closingBook)
		{
			switch (pageType)
			{
			case STATS:
				DrawTitleStats(posX, posY);
				break;
			case INVENTORY:
				DrawInventory(posX, posY);
				break;
			default:
				break;
			}
			
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
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);

	rectBar = { posX +10, posY+25, wRectBar,20 };
	rectBar.w = page.health * wRectBar / page.maxHealth;
	if (liveW < rectBar.w)
	{
		liveW += dt * 150;
		if (liveW < rectBar.w) rectBar.w = liveW;
	}
	DrawBarLive();

	posY += 60;
	sprintf_s(textStats, 15, "MP");
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);

	rectBar = { posX + 10, posY + 25, wRectBar,20 };
	rectBar.w = page.mana * wRectBar / page.maxMana;
	if (manaW < rectBar.w)
	{
		manaW += dt * 150;
		if (manaW < rectBar.w) rectBar.w = manaW;
	}
	app->render->DrawRectangle(rectBar, 157,0,208);
	rectBar.w = wRectBar;
	app->render->DrawRectangle(rectBar, 87,0,115,255,false);

	posY += 60;
	sprintf_s(textStats, 15, "Attack");
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);
	rectBar.y += 60;
	rectBar.h = 10;
	app->render->DrawRectangle(rectBar, 0, 3, 111);

	posY += 50;
	sprintf_s(textStats, 15, "Defense");
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);
	rectBar.y += 50;
	app->render->DrawRectangle(rectBar, 0, 3, 111);

	posY += 50;
	sprintf_s(textStats, 15, "Speed");
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);
	rectBar.y += 50;
	app->render->DrawRectangle(rectBar, 0, 3, 111);

	posY += 60;
	sprintf_s(textStats, 15, "Spells");
	app->fonts->BlitText(posX, posY, 2, textStats, color);

	// Draw Numbers
	posX = -app->render->camera.x + 220 + 280;
	posY = -app->render->camera.y + 120 + 45;

	sprintf_s(textStats, 15, "%d/%d", page.health, page.maxHealth);
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);

	posY += 60;
	sprintf_s(textStats, 15, "%d/%d", page.mana, page.maxMana);
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);

	posY += 60;
	int boostPosY = posY;
	posX -= 50;
	sprintf_s(textStats, 15, "%d", page.attack);
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);

	posY += 50;
	sprintf_s(textStats, 15, "%d", page.defense);
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);

	posY += 50;
	sprintf_s(textStats, 15, "%d", page.speed);
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);
	
	// Draw Rectangles
	posX = WINDOW_W / 2 - app->render->camera.x + 80;
	posY = -app->render->camera.y + 120;
	app->render->DrawTextBox(posX, posY, wRectBar, 200, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	app->render->DrawTextBox(posX, posY + 280, wRectBar, 200, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	app->render->DrawRectangle({ posX + wRectBar / 2 - 9, posY + 30,120 +14,128+8 }, 60, 43, 13);
	app->render->DrawRectangle({ posX + wRectBar / 2 - 4, posY + 35,120 +9,128+3 }, 227, 207, 127);

	// Draw Avatar
	if (page.numPage == 3 || page.numPage == 4) posX += 10;
	app->render->DrawTexture(app->sceneManager->GetPlayerTexture(), posX + wRectBar / 2-15, posY+30, &page.avatar);
	if (page.numPage == 3 || page.numPage == 4) posX -= 10;

	// Draw Name, Class, Id
	posY += 40;
	app->fonts->BlitText(posX + 35, posY, 0, textName, color, dt * velocityDraw);
	posY += 35;
	app->fonts->BlitText(posX + 35, posY, 0, textClass, color, dt * velocityDraw);
	posY += 20;
	app->fonts->BlitText(posX + 35, posY, 0, "Magic", color, dt * velocityDraw);
	posY += 35;
	app->fonts->BlitText(posX + 35, posY, 3, textMagicRunes, color, dt * velocityDraw);

	// Draw Bar Exp
	posX = WINDOW_W / 2 - app->render->camera.x + 80;
	posY += 80;
	sprintf_s(textStats, 15, "Lvl.%d", page.level);
	app->fonts->BlitText(posX, posY, 0, textStats, color, dt * velocityDraw);
	sprintf_s(textStats, 15, "%d/%d", page.exp, page.maxExp);
	app->fonts->BlitText(posX + 250, posY, 0, textStats, color, dt * velocityDraw);

	rectBar = { posX + 10, posY + 25, wRectBar - 30,20 };
	app->render->DrawRectangle(rectBar, 227, 207, 127);
	rectBar.w = page.exp * wRectBar / page.maxExp;
	if (expW < rectBar.w)
	{
		expW += dt * 150;
		if (expW < rectBar.w) rectBar.w = expW;
	}
	app->render->DrawRectangle(rectBar, 0, 94, 221);
	rectBar.w = wRectBar - 30;
	app->render->DrawRectangle(rectBar, 0, 47, 111, 255, false);

	if(!selectingMagic) app->fonts->BlitMarginText(posX + 30, posY + 102, 0, textDescription, color, 270);
	else if (menuMagic->GetMagic(magicSelected) != nullptr) app->fonts->BlitMarginText(posX + 40, posY + 110, 0, menuMagic->GetMagic(magicSelected)->description.GetString(), color, 270);

	DrawBoost(boostPosY, posX);
}

void GuiStatsMenu::DrawBoost(int& posY, int posX)
{
	posX -= 210;
	switch (page.numPage)
	{
	case 1:
		if (app->player->entityData.equipedItem != nullptr)
		{
			if (app->player->entityData.equipedItem->attribute == "defense") posY += 50;
			else if (app->player->entityData.equipedItem->attribute == "speed") posY += 100;

			sprintf_s(textStats, 15, "(+%d)", app->player->entityData.equipedItem->value);
			app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, }, dt * velocityDraw);
		}
		break;
	case 2:
		if (app->player->GetPartners()[0].entityData.equipedItem != nullptr)
		{
			if (app->player->GetPartners()[0].entityData.equipedItem->attribute == "defense") posY += 50;
			else if (app->player->GetPartners()[0].entityData.equipedItem->attribute == "speed") posY += 100;

			sprintf_s(textStats, 15, "(+%d)", app->player->GetPartners()[0].entityData.equipedItem->value);
			app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, }, dt * velocityDraw);
		}
		break;
	case 3:
		if (app->player->GetPartners()[1].entityData.equipedItem != nullptr)
		{
			if (app->player->GetPartners()[1].entityData.equipedItem->attribute == "defense") posY += 50;
			else if (app->player->GetPartners()[1].entityData.equipedItem->attribute == "speed") posY += 100;

			sprintf_s(textStats, 15, "(+%d)", app->player->GetPartners()[1].entityData.equipedItem->value);
			app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, }, dt * velocityDraw);
		}
		break;
	case 4:
		if (app->player->GetPartners()[2].entityData.equipedItem != nullptr)
		{
			if (app->player->GetPartners()[2].entityData.equipedItem->attribute == "defense") posY += 50;
			else if (app->player->GetPartners()[2].entityData.equipedItem->attribute == "speed") posY += 100;

			sprintf_s(textStats, 15, "(+%d)", app->player->GetPartners()[2].entityData.equipedItem->value);
			app->fonts->BlitText(posX, posY, 0, textStats, { 0, 98, 0, }, dt * velocityDraw);
		}
		break;
	default:
		break;
	}
}

void GuiStatsMenu::DrawBookMarks(int posX, int& posY, int switcher)
{
	SDL_Rect bookMarkRect;
	switch (switcher)
	{
	case 1:
		bookMarkRect = { 0,374,68,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 840, posY + 200, &bookMarkRect);
		bookMarkRect = { 80,374,50,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 240, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 280, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 320, &bookMarkRect);
		bookMarkRect = { 178,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 204, &bookMarkRect);
		bookMarkRect = { 146,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 244, &bookMarkRect);
		bookMarkRect = { 208,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 284, &bookMarkRect);
		bookMarkRect = { 240,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 324, &bookMarkRect);
		break;
	case 2:
		bookMarkRect = { 0,374,68,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 840, posY + 240, &bookMarkRect);
		bookMarkRect = { 80,374,50,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 200, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 280, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 320, &bookMarkRect);
		bookMarkRect = { 178,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 204, &bookMarkRect);
		bookMarkRect = { 146,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 244, &bookMarkRect);
		bookMarkRect = { 208,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 284, &bookMarkRect);
		bookMarkRect = { 240,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 324, &bookMarkRect);
		break;
	case 3:
		bookMarkRect = { 0,374,68,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 840, posY + 280, &bookMarkRect);
		bookMarkRect = { 80,374,50,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 200, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 240, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 320, &bookMarkRect);
		bookMarkRect = { 178,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 204, &bookMarkRect);
		bookMarkRect = { 146,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 244, &bookMarkRect);
		bookMarkRect = { 208,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 284, &bookMarkRect);
		bookMarkRect = { 240,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 324, &bookMarkRect);
		break;
	case 4:
		bookMarkRect = { 0,374,68,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 840, posY + 320, &bookMarkRect);
		bookMarkRect = { 80,374,50,36 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 200, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 280, &bookMarkRect);
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 858, posY + 240, &bookMarkRect);
		bookMarkRect = { 178,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 204, &bookMarkRect);
		bookMarkRect = { 146,370,28,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 244, &bookMarkRect);
		bookMarkRect = { 208,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 284, &bookMarkRect);
		bookMarkRect = { 240,370,32,28 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 870, posY + 324, &bookMarkRect);
		break;
	default:
		break;
	}
}

void GuiStatsMenu::DrawInventory(int posX, int& posY)
{

	sprintf_s(textStats, 15, "Inventory");
	app->fonts->BlitText(posX, posY, 2, textStats, color);

	sprintf_s(textStats, 15, "Name");
	app->fonts->BlitText(posX + 25, posY + 45, 0, textStats, color, dt * 20);
	sprintf_s(textStats, 15, "Stock");
	app->fonts->BlitText(posX + 320, posY + 45, 0, textStats, color, dt * 20);

	app->render->DrawRectangle({ posX, posY + 70, 390, 5 },color.r,color.g, color.b);
	SDL_Rect itemTextRect;
	//Draw item info
	if (currentItem != nullptr)
	{
		DrawItemInfo(posX, posY, itemTextRect);
	}

	DrawItemList(posX, posY, itemTextRect);

	if (currentItem != nullptr)
	{
		int w, h;
		posY += 100 + app->player->inventory.Find(currentItem) * 30;
		TTF_SizeText(app->sceneManager->guiFont, currentItem->name.GetString(), &w, &h);
		app->render->DrawRectangle({ posX + 20, posY, w, 4 }, color.r, color.g, color.b);
	}

}

void GuiStatsMenu::DrawItemList(int posX, int& posY, SDL_Rect& itemTextRect)
{
	for (int i = 0; i < app->player->inventory.Count(); i++)
	{
		sprintf_s(textItemName, 30, app->player->inventory.At(i)->data->name.GetString());
		app->fonts->BlitText(posX + 20, posY + 80 + (i * 30), 0, textItemName, color, dt * 20);

		sprintf_s(itemQuantity, 3, "%d", app->player->inventory.At(i)->data->multi);
		app->fonts->BlitText(posX + 340, posY + 80 + (i * 30), 0, itemQuantity, color, dt * 20);

		switch (app->player->inventory.At(i)->data->type)
		{
		case Type::POTION:
			itemTextRect = { 272, 0, 16, 16 };
			app->render->DrawTexture(app->guiManager->uiAtlas, posX - 8, posY + 80 + (i * 30), &itemTextRect, 1.5f);
			break;
		case Type::RING:
			itemTextRect = { 272, 16, 16, 16 };
			app->render->DrawTexture(app->guiManager->uiAtlas, posX - 8, posY + 80 + (i * 30), &itemTextRect, 1.5f);
			break;
		default:
			break;
		}
		if (app->player->inventory.At(i)->data->equiped)
		{
			app->fonts->BlitText(posX + 220, posY + 80 + (i * 30), 0, "E", color, dt * 20);
		}
	}
}

void GuiStatsMenu::DrawItemInfo(int posX, int& posY, SDL_Rect& itemTextRect)
{
	app->render->DrawTextBox(posX + 600, posY + 280, 230, 200, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);
	app->render->DrawTextBox(posX + 477, posY + 280, 110, 200, { 251, 230, 139 }, { 227, 207, 127 }, { 60, 43, 13 }, app->guiManager->moonCorner);

	int w, h;
	TTF_SizeText(app->sceneManager->itemFont, currentItem->name.GetString(), &w, &h);
	sprintf_s(textItemName, 30, currentItem->name.GetString());
	app->fonts->BlitText(posX + 650 - w / 2, posY, 4, textItemName, color);

	sprintf_s(textItemName, 30, "Type: %s", app->player->itemManager->TypeToString(currentItem->type));
	app->fonts->BlitText(posX + 473, posY + 250, 0, textItemName, color);

	sprintf_s(textDescription, 200, currentItem->description.GetString());
	app->fonts->BlitMarginText(posX + 620, posY + 335, 0, textDescription, color, 190);

	switch (currentItem->type)
	{
	case Type::POTION:
		itemTextRect = { 256, 0, 16, 16 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 570, posY + 60, &itemTextRect, 10.0f);
		break;
	case Type::RING:
		itemTextRect = { 256, 16, 16, 16 };
		app->render->DrawTexture(app->guiManager->uiAtlas, posX + 570, posY + 60, &itemTextRect, 10.0f);
		break;
	default:
		break;
	}
}

bool GuiStatsMenu::CleanUp()
{
	active = false;

	return true;
}

bool GuiStatsMenu::Event(GuiControl* control)
{
	//Button stuff

	switch (control->type)
	{
	case GuiControlType::BUTTON:
		if (control->id == 7)
		{
			btnUseItem->state = GuiControlState::DISABLED;
			btnDelItem->state = GuiControlState::DISABLED;
			btnUnEquipItem->state = GuiControlState::DISABLED;
			selectingPlayer = true;
		}
		else if (control->id == 8)
		{
			if (currentItem->equiped) app->player->itemManager->UnEquipItem(currentItem);
			currentItem->equiped = false;
			btnUseItem->state = GuiControlState::DISABLED;
			btnDelItem->state = GuiControlState::DISABLED;
			btnUnEquipItem->state = GuiControlState::DISABLED;
		}
		else if (control->id == 9)
		{
			btnUseItem->state = GuiControlState::DISABLED;
			btnDelItem->state = GuiControlState::DISABLED;
			btnUnEquipItem->state = GuiControlState::DISABLED;
			app->player->itemManager->DelItem(currentItem);
		}
		break;
	default:
		break;
	}

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

void GuiStatsMenu::ChangeStatCharacter()
{
	app->audio->PlayFx(app->guiManager->fxChangePage);
	changingPage = true;
	if (page.numPage > maxNumPages) page.numPage = 1;
	if (page.numPage < 1) page.numPage = maxNumPages;
	InicializeStats();
	menuMagic->AbleDisableMagic();
	app->fonts->ResetH();
	liveW = 0;
	manaW = 0;
	expW = 0;
}
void GuiStatsMenu::InicializeStats()
{
	int level = 1;

	switch (page.numPage)
	{
	case 1: // Kenzie
		level = app->player->playerData.level;
		page.level = level;
		page.attack = app->player->stats.attack;
		page.defense = app->player->stats.defense;
		page.speed = app->player->stats.speed;
		page.maxHealth = level * 2 + 6;
		page.health = app->player->playerData.health;
		page.maxMana = level * 3 + 8;
		page.mana = app->player->playerData.mana;
		page.exp = app->player->playerData.exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(KENZIE_);
		page.avatar = { 0,372,145,145 };
		sprintf_s(textName, 7, "Kenzie");
		sprintf_s(textClass, 10, "Elemental");
		sprintf_s(textMagicRunes, 10, "0468484");

		sprintf_s(textDescription, 200, "The leader of the group. Specializing in high-powered ranged sorcery, she is instrumental in finishing fights with precision.");

		break;
	case 2: // Keiler
		level = app->player->GetPartners()[0].level;
		page.level = level;
		page.attack = app->player->GetPartners()[0].stats.attack;
		page.defense = app->player->GetPartners()[0].stats.defense;
		page.speed = app->player->GetPartners()[0].stats.speed;
		page.maxHealth = 4 * level + 10;
		page.health = app->player->GetPartners()[0].health;
		page.maxMana = 2.5 * level + 7.5;
		page.mana = app->player->GetPartners()[0].mana;
		page.exp = app->player->GetPartners()[0].exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(KEILER_);
		page.avatar = { 145,372,145,145 };
		sprintf_s(textName, 7, "Keiler");
		sprintf_s(textClass, 10, "Nexus");
		sprintf_s(textMagicRunes, 10, "7321044");

		sprintf_s(textDescription, 200, "Despite being somewhat clueless, he is specialized in one of the less frequent aspects of magic and with great potential, voodoo.");

		break;
	case 3: // Isrra
		level = app->player->GetPartners()[1].level;
		page.level = level;
		page.attack = app->player->GetPartners()[1].stats.attack;
		page.defense = app->player->GetPartners()[1].stats.defense;
		page.speed = app->player->GetPartners()[1].stats.speed;
		page.maxHealth = 2.5 * level + 7.5;
		page.health = app->player->GetPartners()[1].health;
		page.maxMana = 3.5 * level + 8.5;
		page.mana = app->player->GetPartners()[1].mana;
		page.exp = app->player->GetPartners()[1].exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(ISRRA_);
		page.avatar = { 10,517,135,145 };
		sprintf_s(textName, 7, "Isrra");
		sprintf_s(textClass, 10, "Light");
		sprintf_s(textMagicRunes, 10, "9046808");

		sprintf_s(textDescription, 200, "The specialist in healing magic. He loves to learn and knows every spell from the Introduction to healing magic word by word.");

		break;
	case 4: // Brenda
		level = app->player->GetPartners()[2].level;
		page.level = level;
		page.attack = app->player->GetPartners()[2].stats.attack;
		page.defense = app->player->GetPartners()[2].stats.defense;
		page.speed = app->player->GetPartners()[2].stats.speed;
		page.maxHealth = 3.5 * level + 9.5;
		page.health = app->player->GetPartners()[2].health;
		page.maxMana = 1.5 * level + 6.5;
		page.mana = app->player->GetPartners()[2].mana;
		page.exp = app->player->GetPartners()[2].exp;
		page.maxExp = CalculateExp(level);

		menuMagic->SetIdTurn(BRENDA_);
		page.avatar = { 155,517,135,145 };
		sprintf_s(textName, 7, "Brenda");
		sprintf_s(textClass, 10, "Aura");
		sprintf_s(textMagicRunes, 10, "6531207");

		sprintf_s(textDescription, 200, "She is capable of standing between the enemy and you, although sometimes she is the cause of the fight by throwing herself into danger without thinking.");

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
		app->render->DrawRectangle(rectBar, 149, 112, 0, 255, false);
	}
}

void GuiStatsMenu::ChangePages()
{
	app->audio->PlayFx(app->guiManager->fxChangePage);
	changingPage = true;

	if (pageType > 2) pageType = static_cast<PageType>(1);
	if (pageType < 1) pageType = static_cast<PageType>(2);
	if (pageType == INVENTORY)
	{
		menuMagic->AbleDisableMagic();
		menuMagic->close->active = false;
	}
}
