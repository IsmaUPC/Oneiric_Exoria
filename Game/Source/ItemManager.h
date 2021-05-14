#ifndef _ITEMS_H_
#define _ITEMS_H_

#include "Point.h"
#include "Log.h"
#include "SString.h"
#include "App.h"
#include "List.h"

enum class Type
{

	POTION = 1,
	STONE = 2,
	RING = 3,
	AMULET = 4,
	WISH = 5,
	ABSTRACT = 6,
	UNKNOW = 7

};

enum class  Obtention
{

	SHOP = 1,
	EXPLORING = 2,
	DROP = 3,
	REWARD = 4,
	UNKNOW = 5

};

struct GameItem
{

	Type type = Type::UNKNOW;
	SString name = "";
	SString description = "";
	Obtention obtention = Obtention::UNKNOW;
	int cost = 0;

};

struct Consumable : public GameItem
{

	bool used = false;

};

struct Equipable : public GameItem
{

	bool equiped = false;

};

struct Potion : public Consumable
{

	SString attribute = "";
	int value;

};

struct Ring : public Equipable
{

	SString attribute = "";
	int value;

};

class ItemManager
{
public:

	ItemManager();
	~ItemManager();

	bool LoadItems(pugi::xml_document& file, const char* filePath);
	void DefineItem(GameItem* item, pugi::xml_node& n);

	bool AddItem(int id);
	bool DelItem(int id);
	bool UseItem(int id);

public:

	List<GameItem*> inventory;

};
#endif // _ITEMS_H_