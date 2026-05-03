#pragma once
#include "UIButton.h"
#include <vector>

struct Quest {
	bool active;
	const char* name;
	bool completed;
	std::string reward;
	int reward_Value;
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

	Quest GetQuest(const char* name);
	void ActivateQuest(const char* name);

private:
	pugi::xml_document questsFileXML;
	std::vector<Quest>* quests;
	std::string questsFileName;
	std::string questsPath;

};