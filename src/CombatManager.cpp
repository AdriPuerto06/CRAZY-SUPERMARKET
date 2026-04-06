#include "CombatManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "Scene.h"
#include<iostream>
#include<cstdlib>


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
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1);
		break;
	case 6: // Button MyButton
		UnloadCombatUI();
		showingButtonStart = false;
		//load the ids
		/*combatData->enemies_id.push_back(combatData->possible_enemy_ID);
		combatData->players_id.push_back(1);*/
		StartCombat(combatData->players_id, combatData->enemies_id);
		LOG("Combat starts.");
		break;
	default:
		break;
	}

	return true;
}

void CombatManager::ButtonAction(int ID)
{
	/*LOG("player_id_selected: %d", combatState->player_id_selected);
	LOG("players_attacks size: %d", combatData->players_attacks.size());*/

	int playerIndex = combatState->player_id_selected;
	std::vector<Attack>& attacks = combatData->players_attacks[playerIndex];

	//get damage from the attack id and apply it to the enemy selected
	switch (ID) {
	case 1:
		combatState->player_attack_dmg_selected = attacks[ID - 1].dmg;
		combatState->enemy_id_targeted = 1; //need to make an option to choose the enemy targeted
		LOG("Attack 1: Damage: %i, name: %s", combatState->player_attack_dmg_selected, attacks[ID - 1].name);
		ApplyCombatLogic();
		break;
	case 4:
		LOG("Attack 4");
		UnloadCombatUI();
		Engine::GetInstance().render->StartTextDisplay("", 0.0f);
		in_combat = false;
		LOG("Cleaned combat UI.");
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1);
		break;
	default:
		break;
	}
}

void CombatManager::ApplyCombatLogic()
{
	if (combatState->turn == "Player")
	{
		combatState->current_enemies_HP[combatState->enemy_id_targeted] -= combatState->player_attack_dmg_selected;
		CheckAlive();
		LOG("Enemy ID: %i now has %i HP.", combatData->enemies_id[combatState->enemy_id_targeted], combatState->current_enemies_HP[combatState->enemy_id_targeted]);
		//UpdateCombatUI(): we need visual info (numbers, bars...)
		combatState->turn = "Enemy";
	}
	if (combatState->turn == "Enemy")
	{
		EnemyAI();
		combatState->current_players_HP[0/*combatState->player_id_targeted*/] -= combatState->enemy_attack_dmg_selected;
		LOG("Player ID: %i now has %i HP.", combatData->players_id[0], combatState->current_players_HP[0]);
		CheckAlive();
		combatState->turn = "Player";
	}
}

void CombatManager::EnemyAI() {

	srand((unsigned)time(NULL));
	int random = 0 + (rand() % 4);
	int random_ID = rand() % combatData->enemies_id.size(); //get a random ID of an enemy. Will need to check if the enemy is alive
	if (!combatState->enemies_alive[random_ID]) random_ID= combatState->enemy_id_targeted; //if enemy random is not alive, make the one you currently target attack you

	combatState->enemy_attack_dmg_selected = combatData->enemies_attacks[random_ID][random - 1].dmg;
	LOG("Enemy ID: %i Does %i dmg to Player ID: %i", random_ID, combatData->enemies_attacks[random_ID][random - 1].dmg, combatState->player_id_selected); //we can set the targeted player to be the one you have selected to make things easier
}

void CombatManager::ShowButtonStart(Vector2D position, int enemy_ID)
{
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Start combat", { (int)position.getX(), (int)position.getY(), 120, 20 }, this));
	LOG("Start dialogue button created at %i, %i.", (int)position.getX(), (int)position.getY());
	
	GetTreeAttributes(); //get dialogue_tree from xml
	combatData->possible_enemy_ID = enemy_ID;
	
	showingButtonStart = true;
}

bool CombatManager::StartCombat(std::vector<int> player_IDs, std::vector<int> enemies_IDs)
{
	if (in_combat) return true;
	Engine::GetInstance().scene->ChangeScene(SceneID::BATTLE);

	in_combat = true;
	GetTreeAttributes(); //get combatData from xml
	//set current data for the start of the combat
	combatState->current_players_HP = combatData->players_HP;
	combatState->current_enemies_HP = combatData->enemies_HP;
	
	combatState->Init(); //prepares its data to be used
	
	combatState->player_id_selected = combatData->players_id[0]; //hardcoded. if we have 2 players at the same time, get the id from the players themselves
	
	showing_continue = false;
	
	return true;
}

bool CombatManager::ShowAttackOptions(int player_ID) {
	LOG("ShowAttackOptions called");
	UnloadCombatUI(); //if needed, create a function to only delete the buttons we choose
	SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() /8 - 65, Engine::GetInstance().window->GetWindowSize().getY()/4 + 100, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, combatData->players_attacks[player_ID-1][0].name, bt1Pos, this));

	SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 365, Engine::GetInstance().window->GetWindowSize().getY()/4 + 100, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, combatData->players_attacks[player_ID - 1][1].name, bt2Pos, this));

	SDL_Rect bt3Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 - 65, Engine::GetInstance().window->GetWindowSize().getY()/4 + 200, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, combatData->players_attacks[player_ID - 1][2].name, bt3Pos, this));

	SDL_Rect bt4Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 365, Engine::GetInstance().window->GetWindowSize().getY()/4 + 200, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, combatData->players_attacks[player_ID - 1][3].name, bt4Pos, this));

	return true;
}

bool CombatManager::ShowItemOptions(int player_ID) {
	LOG("ShowItemOptions called");
	UnloadCombatUI(); //if needed, create a function to only delete the buttons we choose
	SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, combatData->players_attacks[player_ID - 1][0].name, bt1Pos, this));

	SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 + 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, combatData->players_attacks[player_ID - 1][1].name, bt2Pos, this));

	return true;
}

bool CombatManager::ShowCrazyOptions(int player_ID) {
	LOG("ShowCrazyOptions called");
	UnloadCombatUI(); //if needed, create a function to only delete the buttons we choose
	SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, combatData->players_attacks[player_ID - 1][0].name, bt1Pos, this));

	SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 + 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 - 15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, combatData->players_attacks[player_ID - 1][1].name, bt2Pos, this));

	SDL_Rect bt3Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 - 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 + 15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, combatData->players_attacks[player_ID - 1][2].name, bt3Pos, this));

	SDL_Rect bt4Pos = { Engine::GetInstance().window->GetWindowSize().getX() * 2 / 4 + 65, Engine::GetInstance().window->GetWindowSize().getY() * 2 / 4 +15, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, combatData->players_attacks[player_ID - 1][3].name, bt4Pos, this));

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
	if (in_combat) return;
	//players data
	std::vector<Attack> newVec;
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("player"); combat_tree_node != NULL; combat_tree_node = combat_tree_node.next_sibling("player"))
	{
		combatData->players_id.push_back(combat_tree_node.attribute("id").as_int());
		combatData->players_HP.push_back(combat_tree_node.attribute("HP").as_int());
		for (pugi::xml_node current_node = combat_tree_node.child("attack"); current_node != NULL; current_node = current_node.next_sibling("attack"))
		{
			
			Attack attack;
			attack.name = (const char*)current_node.attribute("name").as_string();
			attack.dmg = current_node.attribute("dmg").as_int();
			newVec.push_back(attack);
			
		}
		combatData->players_attacks.push_back(newVec);
		newVec.clear();
	}

	//enemies data
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("enemy"); combat_tree_node != NULL; combat_tree_node = combat_tree_node.next_sibling("enemy"))
	{
		combatData->enemies_id.push_back(combat_tree_node.attribute("id").as_int());
		combatData->enemies_HP.push_back(combat_tree_node.attribute("HP").as_int());
		for (pugi::xml_node current_node = combat_tree_node.child("attack"); current_node != NULL; current_node = current_node.next_sibling("attack"))
		{

			Attack attack;
			attack.name = (const char*)current_node.attribute("name").as_string();
			attack.dmg = current_node.attribute("dmg").as_int();
			newVec.push_back(attack);

		}
		combatData->enemies_attacks.push_back(newVec);
		newVec.clear();
	}
}

void CombatManager::CheckAlive() // if hp >= 0, alive -> false
{
	for (int i = 0; i < combatState->current_enemies_HP.size(); ++i)
	{
		if (combatState->current_enemies_HP[i] <= 0)
		{
			combatState->enemies_alive[i] = false;
		}
	}

	for (int i = 0; i < combatState->current_players_HP.size(); ++i)
	{
		if (combatState->current_players_HP[i] <= 0)
		{
			combatState->players_alive[i] = false;
		}
	}

	//check if enemy wins
	int deadCounter = 0;
	for (int i = 0; i < combatState->players_alive.size(); ++i)
	{
		deadCounter += combatState->players_alive[i];
	}
	if (deadCounter == 0) combatState->enemy_Wins = true;

	//check if player wins
	deadCounter = 0;
	for (int i = 0; i < combatState->enemies_alive.size(); ++i)
	{
		deadCounter += combatState->enemies_alive[i];
	}
	if (deadCounter == 0) combatState->player_Wins = true;

	//if(combatState->player_Wins) end combat
	//if (combatState->enemy_Wins) end combat and player dies (and goes back in file save?)
}