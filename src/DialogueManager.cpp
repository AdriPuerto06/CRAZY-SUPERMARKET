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

bool DialogueManager::OnUIMouseClickEvent(UIElement* uiElement)
{
	switch (uiElement->id)
	{
	case 1: // Button MyButton
		LOG("Dialogs: Choice 1.");
		break;
	case 2: // Button MyButton
		LOG("Dialogs: Choice 2.");
		//choice made id = 2
		break;
	case 3: // Button MyButton
		LOG("Dialogs: Choice 3.");
		//choice made id = 3
		break;
	case 4: // Button MyButton
		LOG("Dialogs: Choice 4.");
		//choice made id = 4
		break;
	default:
		break;
	}

	return true;
}

bool DialogueManager::StartDialog(int dialogue_tree_ID, int npc_id)
{
	DialogTree tree;
	GetTreeAttributes(&tree, dialogue_tree_ID, npc_id);
	Engine::GetInstance().render->StartTextDisplay(tree.nodes_text[0], 100.0f);
	

	return true;
}

bool DialogueManager::ShowOptions(int dialogue_tree_ID, int node_value) {
	if (node_value == 0) return 0;

	/*SDL_Rect bt1Pos = { 520, 350, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, , bt1Pos, this));

	SDL_Rect bt2Pos = { 720, 350, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, , bt2Pos, this));

	SDL_Rect bt3Pos = { 520, 400, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, , bt3Pos, this));

	SDL_Rect bt4Pos = { 720, 400, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, , bt4Pos, this));*/

	return 1;
}

const char* DialogueManager::GetTextFromNode(int dialogue_tree_ID, int node_value) {
	const char* ret = "Couldn't find the text.";
	for (pugi::xml_node dialogue_tree_node = dialogsFileXML.child("dialogs").child("dialogue_tree"); dialogue_tree_node != NULL; dialogue_tree_node = dialogue_tree_node.next_sibling("dialogue_tree"))
	{
		if (dialogue_tree_node.attribute("ID").as_int() == dialogue_tree_ID)
		{
			for(pugi::xml_node node = dialogue_tree_node.child("node"); node != NULL; node = node.next_sibling("node"))
			{
				if (node.attribute("id").as_int() == node_value) ret = (const char*)node.attribute("text").as_string();
			}
		}
	}
	return ret;
}

//error: there are not vectors instantiated previouly
void DialogueManager::GetTreeAttributes(DialogTree* tree, int dialogue_tree_ID, int npc_id)
{
	int current_node_counter = 0;
	for (pugi::xml_node dialogue_tree_node = dialogsFileXML.child("dialogs").child("dialogue_tree"); dialogue_tree_node != NULL; dialogue_tree_node = dialogue_tree_node.next_sibling("dialogue_tree"))
	{
		if (dialogue_tree_node.attribute("ID").as_int() == dialogue_tree_ID && dialogue_tree_node.attribute("NPC").as_int() == npc_id)
		{
			for (pugi::xml_node current_node = dialogue_tree_node.child("node"); current_node != NULL; current_node = current_node.next_sibling("node"))
			{
				current_node_counter++;
				tree->nodes_text.emplace_back((const char*)current_node.attribute("text").as_string());
				tree->nodes_id.emplace_back(current_node.attribute("id").as_int());
				for (pugi::xml_node current_choice = current_node.child("choice"); current_choice != NULL; current_choice = current_choice.next_sibling("choice"))
				{
					tree->choices_id[current_node_counter-1].emplace_back(current_choice.attribute("id").as_int());
					tree->choices_text[current_node_counter - 1].emplace_back((const char*)current_choice.attribute("option").as_string());
					tree->choices_next_node[current_node_counter - 1].emplace_back(current_choice.attribute("next_node").as_int());
				}
			}
		}
	}
}