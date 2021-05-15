#include "ItemManager.h"
#include "Player.h"
#include "SceneManager.h"


ItemManager::ItemManager()
{

}

ItemManager::~ItemManager()
{}

bool ItemManager::LoadItems(pugi::xml_document& file, const char* filePath)
{
	pugi::xml_parse_result result = file.load_file(LIST_ITEMS_FILENAME);

	bool ret = true;

	if (result == NULL)
	{
		LOG("Could not load xml file %s. pugi error: %s", LIST_ITEMS_FILENAME, result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node n = file.first_child().child("item");

		for (n; n != NULL; n = n.next_sibling("item"))
		{
			int use = n.attribute("use").as_int();
			if (use == 1)
			{
				int type = n.attribute("type").as_int();
				if (type == 1)
				{
					Potion* item = new Potion;
					item->attribute = n.attribute("attribute").as_string("");
					item->value = n.attribute("value").as_int();
					DefineItem(item, n);
					potionList.Add(item);
					itemList.Add(item);
				}
				else if (type == 3)
				{
					Ring* item = new Ring;
					item->attribute = n.attribute("attribute").as_string("");
					item->value = n.attribute("value").as_int();
					DefineItem(item, n);
					ringList.Add(item);
					itemList.Add(item);
				}
			}
			else if (use == 2)
			{

			}
		}
	}

	return ret;
}

void ItemManager::DefineItem(GameItem* item, pugi::xml_node& n)
{
	item->id = n.attribute("id").as_int();
	item->name = n.attribute("name").as_string();
	item->description = n.attribute("description").as_string();
	item->cost = n.attribute("cost").as_int();
	item->type = static_cast<Type>(n.attribute("type").as_int());
	item->obtention = static_cast<Obtention>(n.attribute("obtention").as_int());
}

bool ItemManager::UseItem(GameItem* id, Entity* entity)
{
	bool ret = true;
	if (app->player->inventory.At(app->player->inventory.Find(id)) == nullptr)
	{
		LOG("There is no item with that id.");
		ret = false;
	}
	else
	{
		switch (id->type)
		{
		case Type::POTION:
			potionList.At((id->id)-1)->data->Heal(potionList.At((id->id)-1)->data->attribute, potionList.At((id->id)-1)->data->value, entity);
			//DelItem(app->player->inventory.At(app->player->inventory.Find(id))->data);
			break;
		case Type::STONE:
			break;
		case Type::RING:
			break;
		case Type::AMULET:
			break;
		case Type::WISH:
			break;
		case Type::ABSTRACT:
			break;
		case Type::UNKNOW:
			break;
		default:
			break;
		}
	}

	return ret;

}

bool ItemManager::AddItem(int id)
{
	bool ret = true;
	int itemCount = potionList.Count() + ringList.Count();
	if (id < 0 || id > itemCount)
	{
		LOG("There is no item with that id.");
		ret = false;
	}
	else
	{
		app->player->inventory.Add(itemList.At(id)->data);
	}

	return ret;

}

bool ItemManager::DelItem(GameItem* id)
{

	if (app->player->inventory.At(app->player->inventory.Find(id)) == nullptr)
	{
		LOG("Does not exist that item at inventory.");
		return false;
	}
	else
	{
		app->player->inventory.Del(app->player->inventory.At(app->player->inventory.Find(id)));
	}

	return true;

}

void Potion::Heal(SString att, int val, Entity* entity)
{
	if (att == "health")
	{
		entity->stats.health += val;
		if (entity->stats.health > entity->stats.maxHealth) entity->stats.health = entity->stats.maxHealth;
	}
	else if (att == "mana")
	{
		entity->stats.mana += val;
		if (entity->stats.mana > entity->stats.maxMana) entity->stats.mana = entity->stats.maxMana;
	}
	if (app->sceneManager->GetCurrentScene()->name != "sceneBattle")
	{
		app->player->UpdatePlayerStats(entity, entity->entityData.type);
	}
}
