#pragma once
#include "UIButton.h"
#include <vector>

struct Quest {
	bool active;
	const char* name;
	int id;
	bool completed;
	std::string reward;
	int reward_value;
};

class QuestManager : public Module {
public:

	QuestManager();
	virtual ~QuestManager();

	bool Awake();
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	bool LoadQuests(std::string path, std::string fileName);
	void InitQuests();

	Quest GetQuest(const char* name);
	const char* GetQuestName(int id);

	void ActivateQuest(const char* name);
	bool IsQuestActive(const char* name);

	void CompleteQuest(const char* name);
	bool IsQuestCompleted(const char* name);

	void ViewQuest();

private:
	Vector2D WindowSize;
	pugi::xml_document questsFileXML;
	std::vector<Quest>* quests;
	std::string questsFileName;
	std::string questsPath;

};