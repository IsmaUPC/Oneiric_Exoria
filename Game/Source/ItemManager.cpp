#include "ItemManager.h"



ItemManager::ItemManager()
{

}

ItemManager::~ItemManager()
{}

bool ItemManager::LoadItems(pugi::xml_document& file, const char* filePath)
{
	pugi::xml_parse_result result = file.load_file(LIST_ITEMS_FILENAME);

	bool ret = true;

	Potion* item = new Potion;

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
			if (n.attribute("use").as_int() == 1)
			{
				int type = n.attribute("type").as_int();
				if (type == 1)
				{
					item->attribute = n.attribute("attribute").as_string("");
					item->value = n.attribute("value").as_int();
					DefineItem(item, n);
					inventory.Add(item);
				}
				else if (type == 3)
				{
					/*Ring* item = new Ring;
					item->attribute = n.attribute("attribute").as_string("");
					item->value = n.attribute("value").as_int();
					DefineItem(item, n);
					inventory.Add(item);*/
				}
			}
			else if (n.attribute("use").as_int() == 2)
			{

			}
		}
	}

	return ret;
}

void ItemManager::DefineItem(GameItem* item, pugi::xml_node& n)
{
	item->name = n.attribute("name").as_string();
	item->description = n.attribute("description").as_string();
	item->cost = n.attribute("cost").as_int();
	item->type = static_cast<Type>(n.attribute("type").as_int());
	item->obtention = static_cast<Obtention>(n.attribute("obtention").as_int());
}

bool ItemManager::UseItem(int id)
{

	

	return true;

}

bool ItemManager::AddItem(int id)
{



	return true;

}

bool ItemManager::DelItem(int id)
{

	if (inventory.At(id) == nullptr)
	{
		LOG("Does not exist that item at inventory.");
		return false;
	}
	else
	{
		inventory.Del(inventory.At(id));
	}

	return true;

}
