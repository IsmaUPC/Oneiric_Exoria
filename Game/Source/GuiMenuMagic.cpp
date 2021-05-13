#include "App.h"
#include "Render.h"
#include "SceneManager.h"
#include "SceneControl.h"
#include "SceneBattle.h"
#include "GuiMenuMagic.h"
#include "GuiManager.h"

#include "Defs.h"
#include "Log.h"

GuiMenuMagic::GuiMenuMagic(iPoint Position, SceneControl* moduleObserver)
{
	active = false;
	initialPos = Position;
	SDL_Texture* btnTextureAtlas = app->guiManager->btnTextureAtlas;

	btnW = 160;
	btnH = 50;
	padding = btnH + 10;
	magic1 = new GuiButton(31, { initialPos.x, initialPos.y, btnW, btnH }, "magia1", RECTANGLE, btnTextureAtlas);
	magic1->SetObserver(moduleObserver);
	magic1->active = false;
	app->guiManager->AddGuiButton(magic1);
	btns.Add(magic1);

	magic2 = new GuiButton(32, { initialPos.x + btnW+10, initialPos.y, btnW, btnH }, "magia2", RECTANGLE, btnTextureAtlas);
	magic2->SetObserver(moduleObserver);
	magic2->active = false;
	app->guiManager->AddGuiButton(magic2);
	btns.Add(magic2);

	magic3 = new GuiButton(33, { initialPos.x, initialPos.y + padding * 1, btnW, btnH }, "magia3",RECTANGLE, btnTextureAtlas);
	magic3->SetObserver(moduleObserver);
	magic3->active = false;
	app->guiManager->AddGuiButton(magic3);
	btns.Add(magic3);

	magic4 = new GuiButton(34, { initialPos.x + btnW + 10, initialPos.y + padding * 1, btnW, btnH }, "magia4", RECTANGLE, btnTextureAtlas);
	magic4->SetObserver(moduleObserver);
	magic4->active = false;
	app->guiManager->AddGuiButton(magic4);
	btns.Add(magic4);

	close = new GuiButton(30, { initialPos.x+55, initialPos.y + padding * 2, btnW, btnH }, "close", RECTANGLE, btnTextureAtlas);
	close->SetObserver(moduleObserver);
	close->active = false;
	app->guiManager->AddGuiButton(close);

	LoadMagics(LIST_MAGIC_FILENAME);
}

GuiMenuMagic::~GuiMenuMagic()
{
}

void GuiMenuMagic::LoadMagics(const char* file)
{
	pugi::xml_parse_result result = magicDoc.load_file(file);

	if (result == NULL)
	{
		LOG("Could not load the file %s. pugi error: %s", file, result.description());
	}
	else
	{
		pugi::xml_node character = magicDoc.first_child().child("character");

		for (character; character != NULL; character = character.next_sibling("character"))
		{
			for (pugi::xml_node n = character.child("magic"); n != NULL; n = n.next_sibling("magic"))
			{
				Magic* magic = new Magic;
				magic->id = character.attribute("ID").as_int();
				magic->level = n.attribute("level").as_int();
				magic->type = n.attribute("type").as_int();
				magic->name = n.attribute("name").as_string("");
				magic->damage = n.attribute("damage").as_int();
				magic->mana = n.attribute("mana").as_int();
				magic->description = n.attribute("description").as_string("");
				magic->element = n.attribute("element").as_int();
				magics.Add(magic);
			}
		}
	}
}

Magic* GuiMenuMagic::GetMagic(int button)
{
	button --;
	if (magicsCharacterTurn.At(button) == nullptr)
	{
		return nullptr;
	}
	else
	{
		return magicsCharacterTurn.At(button)->data;
	}
}

bool GuiMenuMagic::Update(float dt)
{
	return true;
}

bool GuiMenuMagic::Draw()
{
	return true;
}

bool GuiMenuMagic::CleanUp()
{
	magics.Clear();
	return true;
}

void GuiMenuMagic::AbleDisableMagic()
{
	active = !active;
	magic1->active = active;
	magic2->active = active;
	magic3->active = active;
	magic4->active = active;
	close->active = active;
	if (!close->active) {
		close->state = GuiControlState::NORMAL;
		magic1->state = GuiControlState::NORMAL;
		magic2->state = GuiControlState::NORMAL;
		magic3->state = GuiControlState::NORMAL;
		magic4->state = GuiControlState::NORMAL;
	}

}

void GuiMenuMagic::MovePosition()
{
	int x = -app->render->camera.x + initialPos.x;
	int y = -app->render->camera.y + initialPos.y;

	magic1->bounds.x = x;
	magic1->bounds.y = y;

	magic2->bounds.x = x + 110;
	magic2->bounds.y = y;

	magic3->bounds.x = x;
	magic3->bounds.y = y + padding * 1;

	magic4->bounds.x = x + 110;
	magic4->bounds.y = y + padding * 1;

	close->bounds.x = x + 70;
	close->bounds.y = y + padding * 2 -10;
}

void GuiMenuMagic::SetIdTurn(int turn)
{
	IdCharacterTurn = turn;
	magicsCharacterTurn.Clear();
	int j = 0;
	for (int i = 0; i < magics.Count(); i++)
	{
		if (magics.At(i)->data->id == IdCharacterTurn)
		{
			magicsCharacterTurn.Add(magics.At(i)->data);
			j++;
		}
	}
	//--------------------
	for (int i = 0; i < 4; i++)
	{
		if (i >= j) {
			btns.At(i)->data->text = "   ---";
		}
		else
		{
			btns.At(i)->data->text = magicsCharacterTurn.At(i)->data->name;
		}
	}
}

/*
for (int i = 0; i < 4; i++)
	{
		if (i >= magics.Count()){
			btns.At(i)->data->text = " ";
		}
		else
		{
			btns.At(i)->data->text = magics.At(i)->data->name;
		}
	}
*/
void GuiMenuMagic::NextMagics()
{
}

void GuiMenuMagic::PrevMagics()
{
}
