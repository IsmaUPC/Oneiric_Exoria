#ifndef __GUIMENUMAGIC_H__
#define __GUIMENUMAGIC_H__

#include "Point.h"
#include "SString.h"

#include "GuiButton.h"

struct Magic;

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

	void MovePosition();

	void LoadMagics(const char* file);
	Magic* GetMagic(int button);


	void SetIdTurn(int turn);

	void NextMagics();
	void PrevMagics();

	GuiButton* magic1;
	GuiButton* magic2;
	GuiButton* magic3;
	GuiButton* magic4;

	GuiButton* close;

private:
	
	int IdCharacterTurn = 0;

	pugi::xml_document magicDoc;
	List<GuiButton*> btns;
	List<Magic*> magics;
	List<Magic*> magicsCharacterTurn;

	int btnW, btnH;
	int padding;
	iPoint initialPos;
	bool active;
};


#endif // !__GUIMENUMAGIC_H__