#include "Items.h"



Items::Items()
{

}

Items::~Items()
{}

bool Items::AddItem(int id)
{
	bool ret = true;

	pugi::xml_parse_result result = items.load_file(LIST_ITEMS_FILENAME);

	if (result == NULL)
	{
		LOG("Could not load xml file %s. pugi error: %s", LIST_ITEMS_FILENAME, result.description());
		ret = false;
	}
	else
	{
		pugi::xml_node n = items.first_child().child("item");

		for (n; n != NULL; n = n.next_sibling("item"))
		{
			if (n.attribute("id").as_int() == id)
			{
				Item* item = new Item;

				item->name = n.attribute("name").as_string();
				item->description = n.attribute("description").as_string();
				item->use = static_cast<Usage>(n.attribute("use").as_int());
				item->cost = n.attribute("cost").as_int();
				item->type = static_cast<Type>(n.attribute("type").as_int());
				item->obtention = static_cast<Obtention>(n.attribute("obtention").as_int());

				inventory.Add(item);

				break;
			}
		}
	}

	return ret;
}

bool Items::UseItem(int id)
{

	return true;

}

bool Items::DelItem(int id)
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
