#include "QuestManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "Textures.h"
#include "Scene.h"
#include "CombatManager.h"
#include "Render.h"

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


	PopUp = Engine::GetInstance().textures->Load("Assets/Textures/exclamation.png");

	return true;
}

bool QuestManager::Update(float dt)
{
	ViewQuest();

	return true;
}

bool QuestManager::PostUpdate() {

	return true;
}

bool QuestManager::CleanUp()
{
	Engine::GetInstance().textures->UnLoad(PopUp);

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
	return false;
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
	return false;
}

void QuestManager::ViewQuest()
{
	//para pillar las scenes
	auto& scene = Engine::GetInstance().scene;
	SceneID currentScene = scene->GetCurrentScene();

	
	if (!(currentScene == SceneID::LEVEL1 || currentScene == SceneID::LEVEL2 || currentScene == SceneID::LEVEL3 || currentScene == SceneID::LEVEL4)) {
		return;
	}
	else if (Engine::GetInstance().combatManager->in_combat) {
		return;
	}

	int Yspacing = 0;
	int winW = (int)WindowSize.getX();
	int winY = (int)WindowSize.getY();

	//posición en Y en la que empieza el texto
	int Ystart = 64;

	//blanco?
	SDL_Color color = { 255, 255, 255, 255 };

	for (const Quest& q : *quests)
	{

		if (q.active && !q.completed) {

			std::string text = std::string(q.name);
			std::string desc = std::string(q.description);

			int x = winW - 300;
			
			int y = Ystart + Yspacing;
			// pasar false en DrawTexture hace que siga la camara en vez de dejar la imagen tiesa ahí (muy loco)
			Engine::GetInstance().render->DrawTexture(PopUp, x - PopUp->w - 5, y, nullptr, 0.0f, 0.0, 0, 0, false);

			Engine::GetInstance().render->DrawText(text.c_str(), x, y, 0, 0, color);
			Engine::GetInstance().render->DrawText(desc.c_str(), x, y + 32, (5*CHAR_LENGTH / 7) * desc.size(), (5*CHAR_HEIGHT / 7), color);
			
			Yspacing += 32;

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
		q.description = (const char*)quests_tree_node.attribute("description").as_string();
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
	//Important: save the modifications to the XML 
	std::string mapPathName = questsPath + questsFileName;
	questsFileXML.save_file(mapPathName.c_str());
}

