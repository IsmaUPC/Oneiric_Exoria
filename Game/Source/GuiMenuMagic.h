#ifndef __GUIMENUMAGIC_H__
#define __GUIMENUMAGIC_H__

#include "SString.h"
#include "GuiButton.h"

struct Magic
{
	int id = 0;
	int type = 0;
	int level = 0;
	SString name = "";
	int damage = 0;
	int mana = 0;
	SString description = "";
	int element = 0;
	int particle = 0;
};

class GuiMenuMagic
{
public:
	GuiMenuMagic(iPoint Position, SceneControl* moduleObserver);
	~GuiMenuMagic();

	bool Update(float dt);
	bool Draw();

	bool CleanUp();

	void AbleDisableMagic();
	bool GetActiveMagic() { return active; };
	void SetActiveMagic(bool state) { active = state; };

	void MovePosition();

	void LoadMagics(const char* file);
	Magic* GetMagic(int button);
	void SetInitialPosition(iPoint pos) { initialPos = pos; };

	void SetIdTurn(int turn);

	void NextMagics();
	void PrevMagics();

	GuiButton* magic1 = nullptr;
	GuiButton* magic2 = nullptr;
	GuiButton* magic3 = nullptr;
	GuiButton* magic4 = nullptr;

	GuiButton* close = nullptr;

private:
	bool active = false;
	
	int IdCharacterTurn = 0;

	pugi::xml_document magicDoc;
	List<GuiButton*> btns;
	List<Magic*> magics;
	List<Magic*> magicsCharacterTurn;

	int btnW, btnH = 0;
	int padding = 0;
	iPoint initialPos = { 0,0 };
};


#endif // !__GUIMENUMAGIC_H__