#ifndef __QUESTMANAGER_H__
#define __QUESTMANAGER_H__

#include "Module.h"
#include "Point.h"
#include "SString.h"


enum QuestState {
    INACTIVE = 0,
    ACTIVE = 1,
    COMPLETE = 2
};

class Quest {
public:
    uint id = 0;
    uint requires = 0;

    SString description;

    int state = INACTIVE;
};


class QuestManager : public Module
{
public:

    QuestManager();

    ~QuestManager(){
        CleanUp();
    };
    bool CleanUp();

    bool Awake(pugi::xml_node& config);
    bool Start();

    bool Update(float dt);

    bool ActiveQuest(int id);
    bool FinishQuest(int id);

	void ChangeNPC(int id);

    void LoadQuestList(const char* file);
    bool SaveQuestList();

    void ResetQuestList();

    List<Quest*>* GetActiveQuestList() { return &activeQuestList; }
    List<Quest*>* GetCompleteQuestList() { return &completeQuestList; }

    // 0 = INACTIVE | 1 = ACTIVE | 2 = COMPLETE
    int QuestState(int id);

private:

    ListItem<Quest*> FindQuestForID(int id);
    void PrintQuest(int id);

    pugi::xml_document questDoc;

    List<Quest*> questList;
    List<Quest*> activeQuestList;
    List<Quest*> completeQuestList;
    
};

#endif // !__QUESTMANAGER_H__