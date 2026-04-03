#include "CombatManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "Scene.h"

CombatManager::CombatManager() : Module()
{
	name = "CombatManagerManager";
}

CombatManager::~CombatManager() {}

bool CombatManager::Awake()
{
	return true;
}

bool CombatManager::Start()
{
	combatData = new CombatData;
	combatState = new CombatState;
	return true;
}

bool CombatManager::Update(float dt)
{

	return true;
}

bool CombatManager::PostUpdate() {
	can_be_clicked = true;
	return true;
}

bool CombatManager::CleanUp()
{
	combatFileXML.empty();
	return true;
}

bool CombatManager::LoadCombatData(std::string path, std::string fileName)
{
	combatFileName = fileName;
	combatPath = path;
	std::string mapPathName = combatPath + combatFileName;

	//L15 TODO 2: make mapFileXML an attribute of the Map class
	pugi::xml_parse_result result = combatFileXML.load_file(mapPathName.c_str());
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		return false;
	}
	LOG("CombatData.xml loaded successfully.");
	return true;
}

void CombatManager::UnloadCombatUI()
{
	auto& uiManager = Engine::GetInstance().uiManager;
	uiManager->CleanUp();
}

bool CombatManager::OnUIMouseClickEvent(UIElement* uiElement)
{

	switch (uiElement->id)
	{
	case 1: // Button MyButton
		if (can_be_clicked) {
			ButtonAction(1);
		}
		break;
	case 2: // Button MyButton
		if (can_be_clicked) {
			ButtonAction(2);
		}
		break;
	case 3: // Button MyButton
		if (can_be_clicked) {
			ButtonAction(3);
		}
		break;
	case 4: // Button MyButton
		if (can_be_clicked) {
			ButtonAction(4);
		}
		break;
	case 5: // Button MyButton
		UnloadCombatUI();
		Engine::GetInstance().render->StartTextDisplay("", 0.0f);
		in_combat = false;
		LOG("Cleaned combat UI.");
		break;
	case 6: // Button MyButton
		UnloadCombatUI();
		showingButtonStart = false;
		StartCombat();
		LOG("Combat starts.");
		break;
	default:
		break;
	}

	return true;
}

void CombatManager::ButtonAction(int ID)
{
	//get damage from the attack id and apply it to the enemy selected
}

void CombatManager::ShowButtonStart(Vector2D position)
{
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Start combat", { (int)position.getX(), (int)position.getY(), 120, 20 }, this));
	LOG("Start dialogue button created at %i, %i.", (int)position.getX(), (int)position.getY());
	
	GetTreeAttributes(); //get dialogue_tree from xml
	
	showingButtonStart = true;
}

bool CombatManager::StartCombat(std::vector<int> player_IDs, std::vector<int> enemies_IDs)
{

	Engine::GetInstance().scene->ChangeScene(SceneID::BATTLE);
	in_combat = true;
	GetTreeAttributes(); //get combatData from xml
	//set current data for the start of the combat
	combatState->HPs.push_back(combatData->players_id);
	combatState->HPs.push_back(combatData->enemies_id);
	combatState->player_id_selected = combatData->players_id[0]; //hardcoded. if we have 2 players at the same time, get the id from the players themselves
	
	showing_continue = false;
	ShowOptions(combatState->player_id_selected);
	return true;
}

bool CombatManager::ShowOptions(int player_ID) {
	LOG("ShowOptions called");
	UnloadCombatUI(); //if needed, create a function to only delete the buttons we choose
	SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX()*2/4-65, Engine::GetInstance().window->GetWindowSize().getY()*2/4-15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, combatData->players_attacks[player_ID-1][0].name, bt1Pos, this));

	SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 + 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, combatData->players_attacks[player_ID - 1][1].name, bt2Pos, this));

	SDL_Rect bt3Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 35, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, combatData->players_attacks[player_ID - 1][0].name, bt3Pos, this));

	SDL_Rect bt4Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 + 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 35, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, combatData->players_attacks[player_ID - 1][1].name, bt4Pos, this));

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

void CombatManager::GetTreeAttributes()
{
	//players data
	std::vector<Attack> newVec;
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("player"); combat_tree_node != NULL; combat_tree_node = combat_tree_node.next_sibling("player"))
	{
		combatData->players_id.push_back(combat_tree_node.attribute("id").as_int());
		for (pugi::xml_node current_node = combat_tree_node.child("attack"); current_node != NULL; current_node = current_node.next_sibling("attack"))
		{
			
			Attack attack;
			attack.name = (const char*)current_node.attribute("name").as_string();
			attack.dmg = current_node.attribute("damage").as_int();
			newVec.push_back(attack);
			
		}
		combatData->players_attacks.push_back(newVec);
		newVec.clear();
	}

	//enemies data
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("enemy"); combat_tree_node != NULL; combat_tree_node = combat_tree_node.next_sibling("player"))
	{
		combatData->enemies_id.push_back(combat_tree_node.attribute("id").as_int());
		for (pugi::xml_node current_node = combat_tree_node.child("attack"); current_node != NULL; current_node = current_node.next_sibling("attack"))
		{

			Attack attack;
			attack.name = (const char*)current_node.attribute("name").as_string();
			attack.dmg = current_node.attribute("damage").as_int();
			newVec.push_back(attack);

		}
		combatData->enemies_attacks.push_back(newVec);
		newVec.clear();
	}
}