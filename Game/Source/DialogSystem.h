#ifndef __DIALOGSYSTEM_H__
#define __DIALOGSYSTEM_H__

#include "App.h"
#include "Log.h"
#include <string>
#include <iostream>
#include <vector>
#include "Render.h"
#include "Textures.h"
#include "Module.h"
#include "Input.h"
#include "GuiButton.h"
#include "SDL/include/SDL.h"

using namespace std;

class DialogueNode;

class DialogueOption
{
public:
	DialogueOption() {};
	~DialogueOption() {};

	DialogueOption(string Text, int activeID, int NextNode);
	string text = "";
	int activeID = 0;
	int completeID = 0;
	int nextNode = 0;
};

class DialogueNode
{
public:
	DialogueNode() {};
	~DialogueNode() {};

	DialogueNode(string Text);
	string text = "";
	vector <DialogueOption*> dialogueOptions;
	List <string> answersList;
	int nodeId = 0;
};

class DialogueTree
{
public:
	DialogueTree() {};
	~DialogueTree() {};


public:
	int treeId = 0;
	vector <DialogueNode*> dialogueNodes;
};

class DialogueSystem : public Module
{
public:
	DialogueSystem();
	DialogueSystem(Input* input, Render* render, Textures* tex)
	{
		this->input = input;
		this->render = render;
		this->tex = tex;
	}
	~DialogueSystem();

	void UpdateDialog();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	void DrawDialogue();
	void DrawOptions();
	bool CleanUp();
	void PerformDialogue(int treeId, int playerInput);
	bool LoadDialogue(const char*);
	bool LoadNodes(pugi::xml_node& trees, DialogueTree* oak);
	bool LoadOptions(pugi::xml_node& text_node, DialogueNode* npc);
	DialogueNode* currentNode;
	std::vector <DialogueTree*> dialogueTrees;
	bool onDialog = false;
	void SetId(int id_) { id = id_; };
	int id = 0;
	int totalLetters = 0;
	float actualLetter = 0;
	int dialogSpeed = 1;
	bool missClick = false;

	uint fxDialog = -1;

	GuiButton* btn1 = nullptr;
	GuiButton* btn2 = nullptr;
	GuiButton* btn3 = nullptr;

private:
	//int playerInput = 7;
	//DialogueNode* currentNode;

	iPoint point = { 0,0 };
	pugi::xml_document	dialogues;

	Input* input = nullptr;
	Render* render = nullptr;
	Textures* tex = nullptr;

};
#endif // __DIALOGSYSTEM_H__

