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
			}
			else if (use == 2)
			{
				int type = n.attribute("type").as_int();
				if (type == 3)
				{
					Ring* item = new Ring;
					item->attribute = n.attribute("attribute").as_string("");
					item->value = n.attribute("value").as_int();
					DefineItem(item, n);
					ringList.Add(item);
					itemList.Add(item);
				}
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
			DelItem(app->player->inventory.At(app->player->inventory.Find(id))->data);
			break;
		case Type::STONE:
			break;
		case Type::RING:
			ringList.At(itemList.Find(itemList.At((id->id) - 4)->data))->data->EquipItem(ringList.At(itemList.Find(itemList.At((id->id) - 4)->data))->data, entity);
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

const char* ItemManager::TypeToString(Type type)
{
	switch (type)
	{
	case Type::POTION:
		return "Potion";
		break;
	case Type::STONE:
		return "Stone";
		break;
	case Type::RING:
		return "Ring";
		break;
	case Type::AMULET:
		return "Amulet";
		break;
	case Type::WISH:
		return "Wish";
		break;
	case Type::ABSTRACT:
		return "Abstract";
		break;
	case Type::UNKNOW:
		return "Unknow";
		break;
	default:
		return nullptr;
		break;
	}
}

void ItemManager::SetItemEquip(Equipable* item)
{
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
		for (int i = 0; i < app->player->inventory.Count(); i++)
		{
			if (app->player->inventory.At(i)->data == app->player->itemManager->itemList.At(id)->data)
			{
				app->player->inventory.At(i)->data->multi++;
				return ret;
			}
		}
		app->player->inventory.Add(itemList.At(id)->data)->data->multi++;
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
		if (app->player->inventory.At(app->player->inventory.Find(id))->data->multi > 1)
		{
			app->player->inventory.At(app->player->inventory.Find(id))->data->multi--;
		}
		else
		{
			app->player->inventory.At(app->player->inventory.Find(id))->data->multi--;
			app->player->inventory.Del(app->player->inventory.At(app->player->inventory.Find(id)));
		}
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

void Equipable::EquipItem(Equipable* item, Entity* entity)
{
	if (entity->entityData.equipedItem != nullptr)
	{
		if (entity->entityData.equipedItem->attribute == "attack")
		{
			entity->stats.attack -= entity->entityData.equipedItem->value;
		}
		else if (entity->entityData.equipedItem->attribute == "defense")
		{
			entity->stats.defense -= entity->entityData.equipedItem->value;
		}
		else if (entity->entityData.equipedItem->attribute == "speed")
		{
			entity->stats.speed -= entity->entityData.equipedItem->value;
		}
		entity->entityData.equipedItem->currentOwner = NULL;
		entity->entityData.equipedItem->equiped = false;
		entity->entityData.equipedItem = NULL;
	}
	if (item->currentOwner != entity)
	{
		if (item->attribute == "attack")
		{
			if (item->equiped == true)
			{
				currentOwner->stats.attack -= item->value;
				currentOwner->entityData.equipedItem = NULL;
			}
			entity->stats.attack += item->value;
			entity->entityData.equipedItem = item;
		}
		else if (item->attribute == "defense")
		{
			if (item->equiped == true)
			{
				currentOwner->stats.defense -= item->value;
				currentOwner->entityData.equipedItem = NULL;
			}
			entity->stats.defense += item->value;
			entity->entityData.equipedItem = item;
		}
		else if (item->attribute == "speed")
		{
			if (item->equiped == true)
			{
				currentOwner->stats.speed -= item->value;
				currentOwner->entityData.equipedItem = NULL;
			}
			entity->stats.speed += item->value;
			entity->entityData.equipedItem = item;
		}
		item->currentOwner = entity;
		item->equiped = true;
	}
}
