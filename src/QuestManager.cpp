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
	return true;
}

bool QuestManager::Start()
{
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

	//L15 TODO 2: make mapFileXML an attribute of the Map class
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

void QuestManager::ActivateQuest(const char* name)
{
	for (Quest q : *quests)
	{
		if (q.name == name) { q.active = true; LOG("Quest: %s activated.", q.name); return; };
	}
	LOG("QuestManager: ActivateQuest() has not found the quest.");
}
