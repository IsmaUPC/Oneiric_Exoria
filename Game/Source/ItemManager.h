#ifndef _ITEMS_H_
#define _ITEMS_H_

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

	int id = 0;
	Type type = Type::UNKNOW;
	SString name = "";
	SString description = "";
	Obtention obtention = Obtention::UNKNOW;
	int cost = 0;
	int multi = 0;
	bool equiped = false;
};

struct Consumable : public GameItem
{

	bool used = false;

};

struct Equipable : public GameItem
{
public:

	void EquipItem(Equipable* item, Entity* entity);
	void UnEquipItem(Equipable* item);

	SString attribute = "";
	int value = 0;

	Entity* currentOwner = nullptr;

};

struct Potion : public Consumable
{

	SString attribute = "";
	int value = 0;
	void Heal(SString att, int val, Entity* entity);

};

struct Ring : public Equipable
{

};

class ItemManager
{
public:

	ItemManager();
	~ItemManager();

	bool LoadItems(pugi::xml_document& file, const char* filePath);
	void DefineItem(GameItem* item, pugi::xml_node& n);

	bool AddItem(int id);
	bool DelItem(GameItem* id);
	bool UseItem(GameItem* id, Entity* entity);
	void UnEquipItem(GameItem* item);

	const char* TypeToString(Type type);
	void SetItemEquip(Equipable* item);

	void CleanUp();

public:

	List<GameItem*> itemList;
	List<Potion*> potionList;
	List<Ring*> ringList;

};
#endif // _ITEMS_H_