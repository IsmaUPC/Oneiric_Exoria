#include "App.h"
#include "Input.h"
#include "SceneManager.h"

#include "QuestManager.h"

#include "Defs.h"
#include "Log.h"

QuestManager::QuestManager(){ 
	
	LoadQuestList(QUEST_FILE);

}

bool QuestManager::CleanUp()
{
	SaveQuestList();
	questList.Clear();
	activeQuestList.Clear();
	completeQuestList.Clear();
	return true;
}

bool QuestManager::Awake(pugi::xml_node& config)
{

	return true;
}

bool QuestManager::Start()
{
	return true;
}
bool QuestManager::Update(float dt)
{
	/*
	if (app->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		ResetQuestList();

	if (app->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
		PrintQuest(3) , PrintQuest(4);

	if (app->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		ActiveQuest(3);

	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN)
		FinishQuest(3), ActiveQuest(4);

	if (app->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN)
		FinishQuest(4);

	if (app->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		SaveQuestList();
	*/
	return true;
}

bool QuestManager::ActiveQuest(int id)
{
	bool result = false;

	ListItem<Quest*> node = FindQuestForID(id);

	if (node.data->state == INACTIVE && node.data != NULL) {

		int idRequires = node.data->requires;

		if (idRequires == 0) {
			node.data->state = ACTIVE;
			activeQuestList.Add(node.data);
			result = true;
		}
		else {

			if (completeQuestList.Count() != 0) {
				ListItem<Quest*> nodeComplete = *completeQuestList.start;
				for (int i = 0; i < completeQuestList.Count(); i++) {

					if (nodeComplete.data->id == idRequires) {
						node.data->state = ACTIVE;
						activeQuestList.Add(node.data);
						result = true;
						break;
					}

					nodeComplete = *nodeComplete.next;
				}
			}
		}
	}
	(result == true)? LOG("Mision %d Activada",id) : LOG("La mision %d no pudo ser activada",id);

	return result;
}

bool QuestManager::FinishQuest(int id)
{
	bool result = false;

	if (activeQuestList.Count() != 0)
	{
		ListItem<Quest*>* node = activeQuestList.start;
		for (int i = 0; i < activeQuestList.Count(); i++) {

			if (node->data->id == id) {

				// Quitar quest de la lista de activadas
				node->data->state = COMPLETE;
				completeQuestList.Add(node->data);

				activeQuestList.Del(node);
				result = true;
				break;
			}
			node = node->next;
		}
	}

	(result == true) ? LOG("Mision %d Finalizada",id) : LOG("La mision %d no esta activada",id);

	return result;
}

void QuestManager::LoadQuestList(const char* file)
{
	pugi::xml_parse_result doc = questDoc.load_file(file);

	if (doc == NULL)
	{
		LOG("Could not load the file %s. pugi error: %s", file, doc.description());
	}
	else
	{
		pugi::xml_node quests = questDoc.first_child().child("quests");

		for (quests; quests != NULL; quests = quests.next_sibling("quests"))
		{
			for (pugi::xml_node n = quests.child("quest"); n != NULL; n = n.next_sibling("quest"))
			{
				Quest* loadQuest = new Quest;
				loadQuest->id = n.attribute("id").as_int();
				loadQuest->requires = n.attribute("requires").as_int();
				loadQuest->description = n.attribute("description").as_string("");
				loadQuest->state = n.attribute("state").as_int();
				questList.Add(loadQuest);

				if (loadQuest->state == 1) 
					activeQuestList.Add(loadQuest);
					
				if (loadQuest->state == 2)
					completeQuestList.Add(loadQuest);
			}
		}
	}
}

void QuestManager::ResetQuestList()
{
	ListItem<Quest*>* node = questList.start;
	for (int i = 0; i < questList.Count(); i++) {

		node->data->state = INACTIVE;
		node = node->next;
	}

	activeQuestList.Clear();
	completeQuestList.Clear();
}

bool QuestManager::SaveQuestList()
{
	pugi::xml_parse_result result = questDoc.load_file(QUEST_FILE);

	bool ret = false;

	if (result == NULL)
	{
		LOG("Could not load map xml file savegame.xml. pugi error: %s", result.description());
	}
	else
	{
		pugi::xml_node questSaveFile = questDoc.first_child();
		ListItem<Quest*>* node = questList.start;
		ret = true;
		while (node != NULL && ret)
		{
			pugi::xml_node questNode = questSaveFile.child("quests").child("quest");
			for (int i = 0; i < questList.Count(); i++) {
				questNode.attribute("state").set_value(node->data->state);
				questNode = questNode.next_sibling("quest");
				node = node->next;
			}
		}
	}
	questDoc.save_file(QUEST_FILE);
	questDoc.reset();
	return ret;
}

int QuestManager::QuestState(int id)
{
	ListItem<Quest*> node = FindQuestForID(id);

	return node.data->state;
}

ListItem<Quest*> QuestManager::FindQuestForID(int id)
{
	if (questList.Count() != 0) {

		ListItem<Quest*> node = *questList.start;
		for (int i = 0; i < questList.Count(); i++) {

			if (node.data->id == id) {
				break;
			}
			node = *node.next;
		}

		if (node.data == NULL)
			LOG("Intentando acceder a mision que no existe");

		return node;
	}
	return nullptr;
}

void QuestManager::PrintQuest(int id)
{
	ListItem<Quest*> node = FindQuestForID(id);

	//printf("\nid:%d \nrequire:%d \ndescription:%s \nstate:%d",node.data->id,node.data->requires,node.data->description,node.data->state);
	LOG("\nid:%d \nrequire:%d \ndescription:%s \nstate:%d ", node.data->id, node.data->requires, node.data->description.GetString(), node.data->state);
}
