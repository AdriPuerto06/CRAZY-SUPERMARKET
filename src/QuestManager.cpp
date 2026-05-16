#include "QuestManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"

QuestManager::QuestManager() : Module()
{
	name = "QuestManagerManager";
}

QuestManager::~QuestManager() {}

bool QuestManager::Awake()
{
	quests = new std::vector<Quest>;
	InitQuests();
	return true;
}

bool QuestManager::Start()
{
	WindowSize = { (float)Engine::GetInstance().render->camera.w,
				   (float)Engine::GetInstance().render->camera.h };

	return true;
}

bool QuestManager::Update(float dt)
{

	return true;
}

bool QuestManager::PostUpdate() {
	
	return true;
}

bool QuestManager::CleanUp()
{
	
	return true;
}

bool QuestManager::LoadQuests(std::string path, std::string fileName)
{
	questsFileName = fileName;
	questsPath = path;
	std::string mapPathName = questsPath + questsFileName;

	pugi::xml_parse_result result = questsFileXML.load_file(mapPathName.c_str());
	if (result == NULL)
	{
		LOG("Could not load quests xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		return false;
	}
	LOG("Quests.xml loaded successfully.");
	return true;
	return false;
}

Quest QuestManager::GetQuest(const char* name)
{
	for (Quest q : *quests)
	{
		if (q.name == name) return q;
	}
	LOG("QuestManager: GetQuest() returned an empty Quest.");
	return Quest();
}

const char* QuestManager::GetQuestName(int id)
{
	for (Quest q : *quests)
	{
		if (q.id == id) return q.name;
	}
	LOG("QuestManager: GetQuestName() didn't find the id of the desired quest.");
	return "EMPTY";
}

bool QuestManager::IsQuestActive(const char* name)
{
	for (Quest q : *quests)
	{
		if (std::strcmp(q.name, name) == 0) { return q.active; }
	}
}

void QuestManager::ActivateQuest(const char* name)
{
	for (Quest& q : *quests)
	{
		if (std::strcmp(q.name, name) == 0 && !q.completed) //strcmp -> compares two const char* and if equal returns 0
		{ 
			if (q.active) { LOG("Quest '%s' is already active.", q.name); return; }
			q.active = true; 
			LOG("Quest: '%s' activated.", q.name); 
			SaveQuests(); 
			return;
		}
	}
	LOG("QuestManager: ActivateQuest() has not found the quest.");
}

bool QuestManager::IsQuestCompleted(const char* name)
{
	for (Quest q : *quests)
	{
		if (std::strcmp(q.name, name) == 0) { return q.completed; }
	}
}

void QuestManager::ViewQuest()
{
	int n_quests = 0;
	for (Quest q : *quests)
	{
		if (q.active) {
			Engine::GetInstance().render->DrawTexture(NULL, 100*n_quests, WindowSize.getY() - 150);
			n_quests++;
		}
	}
}

void QuestManager::CompleteQuest(const char* name)
{
	bool isQuestActive = IsQuestActive(name);
	bool isQuestComplete = IsQuestCompleted(name);
	if (!isQuestActive || isQuestComplete) { LOG("Can't complete quest. QuestActive: %i, QuestCompleted: %I.", isQuestActive, isQuestComplete); return; }
	for (Quest& q : *quests)
	{
		if (std::strcmp(q.name, name) == 0) { q.completed = true; SaveQuests(); LOG("Quest: '%s' completed.", q.name); }
	}
}

void QuestManager::InitQuests()
{
	quests->clear();
	for (pugi::xml_node quests_tree_node = questsFileXML.child("quests").child("quest");
		quests_tree_node != NULL;
		quests_tree_node = quests_tree_node.next_sibling("quest"))
	{
		Quest q;
		q.active = quests_tree_node.attribute("active").as_bool();
		q.completed = quests_tree_node.attribute("completed").as_bool();
		q.id = quests_tree_node.attribute("id").as_int();
		q.name = (const char*)quests_tree_node.attribute("name").as_string();
		q.reward = quests_tree_node.attribute("reward").as_int();
		q.reward_value = (const char*)quests_tree_node.attribute("reward_type").as_string();
		quests->push_back(q);
	}
}

void QuestManager::SaveQuests()
{
	int i = 0;
	int l = quests->size();
	for (pugi::xml_node quests_tree_node = questsFileXML.child("quests").child("quest");
		quests_tree_node != NULL;
		quests_tree_node = quests_tree_node.next_sibling("quest"))
	{
		if (i > l) { LOG("More quests in game that in file."); return; }
		Quest q = (*quests)[i];
		quests_tree_node.attribute("active").set_value(q.active);
		quests_tree_node.attribute("completed").set_value(q.completed);
		quests_tree_node.attribute("id").set_value(q.id);
		quests_tree_node.attribute("name").set_value(q.name);
		quests_tree_node.attribute("reward").set_value(q.reward);
		quests_tree_node.attribute("reward_type").set_value(q.reward_value);
		i++;
	}
}

