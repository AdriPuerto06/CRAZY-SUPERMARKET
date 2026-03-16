#include "DialogueManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"

DialogueManager::DialogueManager() : Module() 
{
	name = "DialogueManager";
}

DialogueManager::~DialogueManager() {}

bool DialogueManager::Awake() 
{
	return true;
}

bool DialogueManager::Start() 
{
	return true;
}

bool DialogueManager::Update(float dt) 
{
	return true;
}

bool DialogueManager::CleanUp() 
{
	dialogsFileXML.empty();
	return true;
}

bool DialogueManager::LoadDialogs(std::string path, std::string fileName)
{
	dialogsFileName = fileName;
	dialogsPath = path;
	std::string mapPathName = dialogsPath + dialogsFileName;

	//L15 TODO 2: make mapFileXML an attribute of the Map class
	pugi::xml_parse_result result = dialogsFileXML.load_file(mapPathName.c_str());
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		return false;
	}
	LOG("Dialogs.xml loaded successfully.");
	return true;
}

bool DialogueManager::ShowOptions(int node_value) {
	if (node_value == 0) return 0;

	SDL_Rect bt1Pos = { 520, 350, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, GetTextFromNode(node_value), bt1Pos, this));

	SDL_Rect bt2Pos = { 720, 350, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, GetTextFromNode(node_value+1), bt2Pos, this));

	SDL_Rect bt3Pos = { 520, 400, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, GetTextFromNode(node_value+2), bt3Pos, this));

	SDL_Rect bt4Pos = { 720, 400, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, GetTextFromNode(node_value+3), bt4Pos, this));
}

const char* DialogueManager::GetTextFromNode(int node_value) {
	const char* ret = "Couldn't find the text.";
	for (pugi::xml_node node = dialogsFileXML.child("dialogs").child("text"); node != NULL; node = node.next_sibling("text"))
	{
		LOG("Node id value: %i", node.attribute("id").as_int());
		if (node.attribute("id").as_int() == node_value)
		{
			ret = (const char*)node.attribute("content").as_string();
			LOG("Text from node assigned.");
		}
	}
	return ret;
}