#ifndef _ITEMS_H_
#define _ITEMS_H_

#include "Point.h"
#include "Log.h"
#include "SString.h"
#include "App.h"

enum Type
{

	POTION = 1,
	STONE = 2,
	RING = 3,
	AMULET = 4,
	WISH = 5

};

enum Usage
{

	CONSUMABLE = 1,
	EQUIPABLE = 2

};

enum  Obtention
{

	SHOP = 1,
	EXPLORING = 2,
	DROP = 3,
	REWARD = 4

};

class Item
{
public:

	Type type;
	Usage use;
	SString name = "";
	SString description = "";
	Obtention obtention;
	int cost;

};

class Items
{
public:

	Items();

	virtual ~Items();
	bool AddItem(int id);
	bool DelItem(int id);
	bool UseItem(int id);

public:

	pugi::xml_document	items;
	List<Item*> inventory;

};
#endif // _ITEMS_H_