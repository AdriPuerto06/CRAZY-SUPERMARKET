#include "CombatManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "Scene.h"
#include<iostream>
#include<cstdlib>
#include "Player.h"
#include "Map.h"

//helpers
std::vector<int> GetIDs(std::string str)
{
	std::vector<int> IDs;
	bool lastValNum = true;
	int num = str.at(0) - '0';
	
	for (int l = 1; l < str.size(); ++l)
	{
		if (!(str[l] == ','))
		{
			/*if (lastValNum) */num = num * 10 + (str.at(l) - '0');
			lastValNum = true;
		}
		else 
		{ 
			lastValNum = false; 
			IDs.push_back(num);
			num = 0;
		}
			
	}

	IDs.push_back(num);
	return IDs;
}

bool Contains(std::vector<int> vec, int val)
{
	for (int i = 0; i < vec.size(); ++i)
	{
		if (vec[i] == val)
		{
			return true;
		}
	}

	return false;
}

CombatManager::CombatManager() : Module()
{
	name = "CombatManagerManager";
}

CombatManager::~CombatManager() {}

bool CombatManager::Awake()
{
	combatData = new CombatData;
	combatState = new CombatState;
	/*itemVector.push_back(false);*/
	return true;
}

bool CombatManager::Start()
{
	srand((unsigned)time(NULL));
	return true;
}

bool CombatManager::Update(float dt)
{
	//choose the enemy to focus
	if (combatState->turn == "Player" && combatState->selecting_target)
	{
		HandleTargetSelection();
	}

	return true;
}

bool CombatManager::PostUpdate() {
	can_be_clicked = true;
	return true;
}

bool CombatManager::CleanUp()
{
	combatFileXML.empty();
	combatData->Clear();
	combatState->Clear();
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
		StartCombat(/*combatData->players_id, combatData->enemies_id*/);
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

	int playerIndex = combatState->player_id_selected - 1;
	std::vector<Attack>& attacks = combatData->players_attacks[playerIndex];
	
	if (combatState->magicPoints <= 0)
	{
		LOG("No magic points left!");
		return;
	}
	combatState->player_attack_dmg_selected = attacks[ID - 1].dmg;
	if (godMode) combatState->player_attack_dmg_selected = 999;
		
	combatState->player_attack_id_selected = ID;
	LOG("Attack %i: Damage: %i, name: %s", ID, combatState->player_attack_dmg_selected, attacks[ID - 1].name);
	//break;
	//else {
	//	//get damage from the attack id and apply it to the enemy selected
	//	switch (ID) {
	//	case 1:
	//		combatState->player_attack_dmg_selected = attacks[ID - 1].dmg;
	//		combatState->player_attack_id_selected = ID - 1;
	//		LOG("Attack 1: Damage: %i, name: %s", combatState->player_attack_dmg_selected, attacks[ID - 1].name);
	//		break;
	//	case 2:
	//		combatState->player_attack_dmg_selected = attacks[ID - 1].dmg;
	//		LOG("Attack 2: Damage: %i, name: %s", combatState->player_attack_dmg_selected, attacks[ID - 1].name);
	//		break;
	//	case 3:
	//		combatState->player_attack_dmg_selected = attacks[ID - 1].dmg;
	//		LOG("Attack 3: Damage: %i, name: %s", combatState->player_attack_dmg_selected, attacks[ID - 1].name);
	//		break;
	//	case 4:
	//		combatState->player_attack_dmg_selected = attacks[ID - 1].dmg;
	//		LOG("Attack 4: Damage: %i, name: %s", combatState->player_attack_dmg_selected, attacks[ID - 1].name);
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//ApplyCombatLogic();
	combatState->selecting_target = true;
	LOG("Select enemy with LEFT/RIGHT \t Press ENTER to confirm.");
	LOG("Currently targeting Enemy ID: %i", combatData->enemies_id[combatState->enemy_id_targeted - 1]);
}

void CombatManager::ApplyCombatLogic()
{
	if (combatState->turn == "Player") //add here a switch that depending on the name of the attack does something
	{
		CheckAlive();
		combatState->current_enemies_HP[combatState->enemy_id_targeted - 1] -= combatState->player_attack_dmg_selected;
		combatState->magicPoints -= combatData->players_attacks[combatState->player_id_selected-1][combatState->player_attack_id_selected-1].magicPoints;
		LOG("Enemy ID: %i now has %i HP.", combatData->enemies_id[combatState->enemy_id_targeted - 1], combatState->current_enemies_HP[combatState->enemy_id_targeted - 1]);
		//UpdateCombatUI(): we need visual info (numbers, bars...)
		combatState->turn = "Enemy";
		CheckAlive();
	}
	if (combatState->turn == "Enemy")
	{
		CheckAlive();
		EnemyAI();
		LOG("Player ID: %i now has %i HP.", combatState->player_id_selected - 1, combatState->current_players_HP[combatState->player_id_selected - 1]);
		combatState->turn = "Player";
	}
}

void CombatManager::HandleTargetSelection() {
	int maxEnemies = combatData->enemies_id.size();

	// RIGHT -->
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		int next = combatState->enemy_id_targeted;

		for (int i = 0; i < maxEnemies; i++)
		{
			next++;

			if (next > maxEnemies) {
				next = 1;
			}


			if (combatState->enemies_alive[next - 1])
			{
				combatState->enemy_id_targeted = next;

				LOG("Targeting Enemy ID: %i", combatData->enemies_id[next - 1]);

				break;
			}
		}
	}

	// LEFT <--
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		int prev = combatState->enemy_id_targeted;

		for (int i = 0; i < maxEnemies; i++)
		{
			prev--;

			if (prev < 1) {
				prev = maxEnemies;
			}


			if (combatState->enemies_alive[prev - 1])
			{
				combatState->enemy_id_targeted = prev;

				LOG("Targeting Enemy ID: %i", combatData->enemies_id[prev - 1]);

				break;
			}
		}
	}

	// ENTER
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		LOG("Confirmed attack on Enemy ID: %i", combatData->enemies_id[combatState->enemy_id_targeted - 1]);

		combatState->selecting_target = false;

		ApplyCombatLogic();
	}

}

void CombatManager::EnemyAI()
{
	std::vector<int> alive;
	for (int i = 0; i < combatState->enemies_alive.size(); ++i)
	{
		if (combatState->enemies_alive[i])
			alive.push_back(i);
	}

	if (alive.empty()) return;

	int random_index_ID = alive[rand() % alive.size()];

	int random_index_attack = rand() % combatData->enemies_attacks[random_index_ID].size();

	int dmg = combatData->enemies_attacks[random_index_ID][random_index_attack].dmg;
	combatState->enemy_attack_dmg_selected = dmg;
	combatState->current_players_HP[combatState->player_id_selected - 1] -= dmg;

	LOG("Enemy ID: %i does %i dmg to Player ID: %i",
		combatData->enemies_id[random_index_ID],
		dmg,
		combatState->player_id_selected);
}

void CombatManager::ShowButtonStart(Vector2D position, int enemy_ID, int fight_ID)
{
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Start combat", { (int)position.getX(), (int)position.getY(), 120, 20 }, this));
	LOG("Start dialogue button created at %i, %i.", (int)position.getX(), (int)position.getY());

	GetTreeAttributes(fight_ID); //get dialogue_tree from xml
	combatData->possible_enemy_ID = enemy_ID;

	showingButtonStart = true;
}

bool CombatManager::StartCombat(/*std::vector<int> player_IDs, std::vector<int> enemies_IDs*/)
{
	if (in_combat) return true;
	Engine::GetInstance().scene->ChangeScene(SceneID::BATTLE);

	in_combat = true;
	/*GetTreeAttributes(combatData->fight_ID);*/ //get combatData from xml
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
	choosingAtk = true;

	SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 - 65, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 100, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 1, combatData->players_attacks[player_ID - 1][0].name, bt1Pos, this));

	SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 365, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 100, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 2, combatData->players_attacks[player_ID - 1][1].name, bt2Pos, this));

	SDL_Rect bt3Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 - 65, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 200, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 3, combatData->players_attacks[player_ID - 1][2].name, bt3Pos, this));

	SDL_Rect bt4Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 365, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 200, 300,50 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 4, combatData->players_attacks[player_ID - 1][3].name, bt4Pos, this));

	SDL_Rect bt5Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 700, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 300, 120,20 };
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 10, "Back", bt5Pos, Engine::GetInstance().scene.get()));

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

bool CombatManager::ChangePlayer() {
	int current_player_index = combatState->player_id_selected-1;
	for (int i = current_player_index+1; i < combatState->players_alive.size(); ++i)
	{
		if (combatState->players_alive[i])
		{
			combatState->player_id_selected = i + 1;
			return true;
		}
	}
	for (int i = 0; i < current_player_index; ++i)
	{
		if (combatState->players_alive[i])
		{
			combatState->player_id_selected = i + 1;
			return true;
		}
	}
	/*if (combatState->player_id_selected == 1 && combatState->players_alive[combatState->player_id_selected - 1])
	{
		combatState->player_id_selected = 2;
		LOG("Changed player to Player ID: 2");
	}
	else
		if (combatState->player_id_selected == 2 && combatState->players_alive[combatState->player_id_selected - 1])
		{
			combatState->player_id_selected = 1;
			LOG("Changed player to Player ID: 1");
		}*/

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

void CombatManager::GetTreeAttributes(int fight_ID)
{
	if (in_combat) return;
	combatData->Clear();

	combatData->fight_ID = fight_ID;
	combatState->magicPoints = Engine::GetInstance().map->magicPoints; //get magicPoints from map
	//get the ids of players and enemies and add them in combatData
	for (pugi::xml_node fight_tree_node = combatFileXML.child("combat").child("fight"); fight_tree_node != NULL; fight_tree_node = fight_tree_node.next_sibling("fight"))
	{
		if (fight_tree_node.attribute("id").as_int() == fight_ID)
		{
			std::string players_id_str = fight_tree_node.attribute("players_id").as_string();
			std::string enemies_id_str = fight_tree_node.attribute("enemies_id").as_string();
			combatData->players_id = GetIDs(players_id_str);
			combatData->enemies_id = GetIDs(enemies_id_str);
			break;
		}
	}
	//players data
	std::vector<Attack> newVec;
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("player"); combat_tree_node != NULL; combat_tree_node = combat_tree_node.next_sibling("player"))
	{
		if (Contains(combatData->players_id, combat_tree_node.attribute("id").as_int()))
		{

			combatData->players_HP.push_back(combat_tree_node.attribute("HP").as_int());
			for (pugi::xml_node current_node = combat_tree_node.child("attack_stats"); current_node != NULL; current_node = current_node.next_sibling("attack_stats"))
			{

				Attack attack;
				attack.name = (const char*)current_node.attribute("name").as_string();
				attack.dmg = current_node.attribute("dmg").as_int();
				attack.magicPoints = current_node.attribute("magicPoints").as_int();
				attack.effect = current_node.attribute("effect").as_string();
				newVec.push_back(attack);

			}

			combatData->players_attacks.push_back(newVec);
			newVec.clear();

		}
	}

	//enemies data
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("enemy"); combat_tree_node != NULL; combat_tree_node = combat_tree_node.next_sibling("enemy"))
	{
		if (Contains(combatData->enemies_id, combat_tree_node.attribute("id").as_int()))
		{

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
}

void CombatManager::CheckAlive() // if hp >= 0, alive -> false
{
	for (int i = 0; i < combatState->current_enemies_HP.size(); ++i)
	{
		if (combatState->current_enemies_HP[i] <= 0 && combatState->enemies_alive[i])
		{
			combatState->enemies_alive[i] = false;
			LOG("Enemy ID: %i has been killed.", combatData->enemies_id[i]);
			for (int j = 0; j < combatData->enemies_id.size(); ++j) //change the selected enemy to one that is alive
			{
				if (combatState->enemies_alive[j])
				{
					combatState->enemy_id_targeted = j+1;
					LOG("Now targeting enemy ID: %i", combatData->enemies_id[combatState->enemy_id_targeted-1]);
					break;
				}
			}
		}


	}

	for (int i = 0; i < combatState->current_players_HP.size(); ++i)
	{
		if (combatState->current_players_HP[i] <= 0)
		{
			combatState->players_alive[i] = false;
			for (int j = 0; j < combatData->players_id.size(); ++j) //change the selected player to one that is alive
			{
				if (combatState->players_alive[j])
				{
					combatState->player_id_selected = combatData->players_id[j];
					break;
				}
			}

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

	if (combatState->player_Wins || combatState->enemy_Wins)
	{
		if (combatState->player_Wins) LOG("Player wins the combat.");
		else LOG("Enemies win the combat.");
		//UnloadCombatUI();

		//combatData->Clear();

		//combatState->Clear();

		//Engine::GetInstance().render->StartTextDisplay("", 0.0f);
		//in_combat = false;
		//LOG("Cleaned combat UI.");
		//Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1); //current_Level
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1);
		in_combat = false;
	}
	
}