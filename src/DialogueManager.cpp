#include "DialogueManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"

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
	tree = new DialogueTree;
	dialogue = new CurrentDialogue;
	return true;
}

bool DialogueManager::Update(float dt) 
{

	return true;
}

bool DialogueManager::PostUpdate() {
	can_be_clicked = true;
	return true;
}

bool DialogueManager::CleanUp() 
{
	in_conversation = false;
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
		LOG("Could not load dialogue xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		return false;
	}
	LOG("Dialogs.xml loaded successfully.");
	return true;
}

void DialogueManager::UnloadDialogueUI()
{
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();
}

bool DialogueManager::OnUIMouseClickEvent(UIElement* uiElement)
{

	switch (uiElement->id)
	{
	case 1: // Button MyButton
		if (dialogue->node_id == -1) return true;
		if (can_be_clicked) {
			ButtonAction(1);
		}
		break;
	case 2: // Button MyButton
		if (dialogue->node_id == -1) return true;
		if (can_be_clicked) {
			ButtonAction(2);
		}
		break;
	case 3: // Button MyButton
		UnloadDialogueUI();
		Engine::GetInstance().render->StartTextDisplay("", 0.0f);
		in_conversation = false;
		tree->Clear();
		LOG("Cleaned dialogue UI.");
		break;
	case 4: // Button MyButton
		UnloadDialogueUI();
		showingButtonStart = false;
		StartDialogue(dialogue->dialogue_tree_ID, dialogue->dialogue_tree_NPC);
		LOG("Dialogue starts.");
		break;
	default:
		break;
	}

	return true;
}

void DialogueManager::ButtonAction(int ID)
{
	//update values of dialogue
	dialogue->choice = ID;
	dialogue->node_id = tree->choices_next_node[dialogue->node_id][ID-1];
	LOG("Dialogs: Choice %i. Current node: %i", ID, dialogue->node_id);
	if (dialogue->node_id != -1 && !showing_continue)
	{
		
		ShowOptions(dialogue->node_id);
		Engine::GetInstance().render->StartTextDisplay(tree->nodes_text[dialogue->node_id], 100.0f);
	}
	can_be_clicked = false;

	if (dialogue->node_id == -1 && !showing_continue) //create "Continue" button
	{
		UnloadDialogueUI();
		SDL_Rect bt5Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 35, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 + 100, 180,30 };
		std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, "Continue", bt5Pos, this));
		showing_continue = true;
	}
}

void DialogueManager::ShowButtonStart(Vector2D position, int dialogue_tree_ID, int npc_id)
{
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, "Start talk", { (int)position.getX(), (int)position.getY(), 120, 20 }, this));
	LOG("Start dialogue button created at %i, %i.", (int)position.getX(), (int)position.getY());
	
	//GetTreeAttributes(dialogue_tree_ID, npc_id); //get dialogue_tree from xml
	dialogue->dialogue_tree_ID = dialogue_tree_ID;
	dialogue->dialogue_tree_NPC = npc_id;
	//dialogue->node_id = tree->nodes_id[0];
	//
	//dialogue->dialogue_tree_ID = dialogue_tree_ID;
	//dialogue->dialogue_tree_NPC = npc_id;
	showingButtonStart = true;
}

bool DialogueManager::StartDialogue(int dialogue_tree_ID, int npc_id)
{
	in_conversation = true;
	GetTreeAttributes(dialogue_tree_ID, npc_id); //get dialogue_tree from xml
	dialogue->dialogue_tree_ID = dialogue_tree_ID;
	dialogue->dialogue_tree_NPC = npc_id;
	dialogue->node_id = tree->nodes_id[0];
	
	showing_continue = false;
	/*can_be_clicked = true;*/
	Engine::GetInstance().render->StartTextDisplay(tree->nodes_text[dialogue->node_id], 100.0f);
	ShowOptions(dialogue->node_id);
		
	return true;
}

bool DialogueManager::ShowOptions(int node_value) {
	if (node_value == -1) return true;
	LOG("ShowOptions called");
	UnloadDialogueUI();
	if (tree->choices_text[node_value].size() == 1)
	{
		SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
		std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, tree->choices_text[node_value][0], bt1Pos, this));
	}
	else {
		SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
		std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, tree->choices_text[node_value][0], bt1Pos, this));

		SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 + 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
		std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, tree->choices_text[node_value][1], bt2Pos, this));
	}
	return true;
}

//const char* DialogueManager::GetTextFromNode(int dialogue_tree_ID, int node_value) {
//	const char* ret = "Couldn't find the text.";
//	for (pugi::xml_node dialogue_tree_node = dialogsFileXML.child("dialogs").child("dialogue_tree"); dialogue_tree_node != NULL; dialogue_tree_node = dialogue_tree_node.next_sibling("dialogue_tree"))
//	{
//		if (dialogue_tree_node.attribute("ID").as_int() == dialogue_tree_ID)
//		{
//			for(pugi::xml_node node = dialogue_tree_node.child("node"); node != NULL; node = node.next_sibling("node"))
//			{
//				if (node.attribute("id").as_int() == node_value) ret = (const char*)node.attribute("text").as_string();
//			}
//		}
//	}
//	return ret;
//}

void DialogueManager::GetTreeAttributes(int dialogue_tree_ID, int npc_id)
{
	int current_node_counter = 0;
	for (pugi::xml_node dialogue_tree_node = dialogsFileXML.child("dialogs").child("dialogue_tree"); dialogue_tree_node != NULL; dialogue_tree_node = dialogue_tree_node.next_sibling("dialogue_tree"))
	{
		if (dialogue_tree_node.attribute("ID").as_int() == dialogue_tree_ID && dialogue_tree_node.attribute("NPC").as_int() == npc_id)
		{
			for (pugi::xml_node current_node = dialogue_tree_node.child("node"); current_node != NULL; current_node = current_node.next_sibling("node"))
			{
				current_node_counter++;
				tree->nodes_text.push_back((const char*)current_node.attribute("text").as_string());
				tree->nodes_id.emplace_back(current_node.attribute("id").as_int());
				for (pugi::xml_node current_choice = current_node.child("choice"); current_choice != NULL; current_choice = current_choice.next_sibling("choice"))
				{
					//add vectors so it doesn't crash
					std::vector<int> newVec;
					tree->choices_id.push_back(newVec);
					tree->choices_next_node.push_back(newVec);
					std::vector<const char*> newVec2;
					tree->choices_text.push_back(newVec2);
					tree->rewards.push_back(newVec2);
					//add attributes
					bool has_Reward = !current_choice.attribute("reward").empty();
					if (has_Reward) { tree->rewards[current_node_counter - 1].emplace_back((const char*)current_choice.attribute("reward").as_string()); }
					else { tree->rewards[current_node_counter - 1].emplace_back("none"); }
					tree->choices_id[current_node_counter-1].emplace_back(current_choice.attribute("id").as_int());
					tree->choices_text[current_node_counter - 1].emplace_back((const char*)current_choice.attribute("option").as_string());
					tree->choices_next_node[current_node_counter - 1].emplace_back(current_choice.attribute("next_node").as_int());
				}
			}
		}
	}
}