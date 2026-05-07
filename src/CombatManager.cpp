#include "CombatManager.h"
#include "Engine.h"
#include "Log.h"
#include "UIManager.h"
#include "Window.h"
#include "Scene.h"
#include<iostream>
#include <cstdlib>
#include <random>
#include "Player.h"
#include "Map.h"
#include "ItemManager.h"
#include "EntityManager.h"

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
			num = num * 10 + (str.at(l) - '0');
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

bool CanAttack(int probability) {
	static std::random_device rd;          // seed
	static std::mt19937 gen(rd());         // Mersenne Twister RNG
	std::uniform_int_distribution<> dist(1, 100);

	int roll = dist(gen);
	return roll <= probability;
}

void CombatManager::MakeAttack(Combatant& target, Combatant& attacker, Attack attack)
{

	//effects that affect the attacker (heal itself, buff itself...)
	if (attack.effect == "none")
	{
	}
	else if (attack.effect == "heal")
	{
		attacker.hp += HEAL_HITPOINTS; if (attacker.type == EntityType::PLAYER) LOG("Player ID: %i healed for %i.", attacker.id, HEAL_HITPOINTS); 
						   else { LOG("Enemy ID: %i healed for %i. Now has %i HP.", attacker.id, HEAL_HITPOINTS, attacker.hp); };
	}
	else if (attack.effect == "selfKO")
	{
		attacker.hp -= attacker.hp; if (attacker.type == EntityType::PLAYER) LOG("Player ID: %i selfKOed.", attacker.id); 
									else { LOG("Enemy ID: %i selfKOed.", attacker.id); };
	}
	else if (attack.effect == "ragebait")
	{
		target.hp -= target.hp; if (attacker.type == EntityType::PLAYER) LOG("Player ID: %i falls for the ragebait.", attacker.id);
		else { LOG("Enemy ID: %i falls for the ragebait.", attacker.id); }
	}
	else if (attack.effect == "shield")
	{
		if (attacker.type == EntityType::PLAYER) { LOG("Player ID: %i activates shield.", attacker.id); /*attacker.status = "shield"*/ attacker.shield_and_buff.first = true; }
		else { LOG("Enemy ID: %i activates shield.", attacker.id); /*attacker.status = "shield";*/ attacker.shield_and_buff.first = true;};
	}
	else if (attack.effect == "buff") 
	{
		if (attacker.type == EntityType::PLAYER) { LOG("Player ID: %i buffs its dmg by %i.", attacker.id, BUFF_DMG_INCREASE); /*attacker.status = "Buff";*/ attacker.shield_and_buff.second = true; }
		else { LOG("Enemy ID: %i buffs its dmg by %i.", attacker.id, BUFF_DMG_INCREASE); /*attacker.status = "Buff";*/ attacker.shield_and_buff.second = true; }
	}
	else
	{
		target.status = attack.effect;
	}
	//apply items
	int dmg_increase = 0;
	int dmg_reduction = 0;
	int confused_probability = 0;
	Engine::GetInstance().itemManager->ApplyCombatItems(dmg_increase, dmg_reduction, confused_probability);
	//effects of the target that affect the attacker
	
	if (target.shield_and_buff.first)
	{
		dmg_reduction += SHIELD_DMG_REDUCTION;
		if (target.type == EntityType::PLAYER) { LOG("Player ID: %i reduces %i dmg thanks to the shield.", target.id, SHIELD_DMG_REDUCTION);}
		else { LOG("Enemy ID: %i reduces %i dmg thanks to the shield.", target.id, SHIELD_DMG_REDUCTION);}
	}
	if (attacker.shield_and_buff.second)
	{
		dmg_increase += BUFF_DMG_INCREASE;
		if (attacker.type == EntityType::PLAYER) { LOG("Player ID: %i increases %i dmg thanks to the buff.", attacker.id, BUFF_DMG_INCREASE); }
		else { LOG("Enemy ID: %i increases %i dmg thanks to the buff.", attacker.id, BUFF_DMG_INCREASE); }
	}

	int dmg_applied = attack.dmg + dmg_increase - dmg_reduction;
	if (dmg_applied < 0) dmg_applied = 0; //clamp

	if (confused_probability != 0 && attacker.type == EntityType::BASEENEMY)
	{
		if (CanAttack(100 - confused_probability)) {}
		else { LOG("Enemy couldn't attack because of the item 'Disturbing Picture'."); return; }
	}
	else target.hp -= dmg_applied;
	
	if (attacker.type == EntityType::PLAYER) { LOG("Player ID: %i makes attack: %s, dmg: %i", attacker.id, attack.name, dmg_applied); LOG("Enemy ID: %i now has %i HP.", target.id, target.hp); }
	else { LOG("Enemy ID: %i makes attack: %s, dmg: %i", attacker.id, attack.name, dmg_applied); LOG("Player ID: %i now has %i HP.", target.id, target.hp); }
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
	GetTreeAttributes(-1, true);
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

	if (showInventory == true) {
		Engine::GetInstance().itemManager->ShowInventory();
	}
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && showInventory == true) {
		showInventory = false;
		Engine::GetInstance().itemManager->UnShowInventory();
	}

	return true;
}

bool CombatManager::PostUpdate() {
	can_be_clicked = true;
	return true;
}

bool CombatManager::CleanUp()
{
	in_combat = false;
	combatFileXML.empty();
	combatData->Clear();
	return true;
}

bool CombatManager::LoadCombatData(std::string path, std::string fileName)
{
	combatFileName = fileName;
	combatPath = path;
	std::string mapPathName = combatPath + combatFileName;

	pugi::xml_parse_result result = combatFileXML.load_file(mapPathName.c_str());
	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
		return false;
	}
	LOG("CombatData.xml loaded successfully.");
	return true;
}

bool CombatManager::StartCombat()
{
	if (in_combat) return true;

	Engine::GetInstance().scene->ChangeScene(SceneID::BATTLE);
	in_combat = true;
	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;

	combatState->Init(*combatData);
	combatState->player_index_selected = 0; // first player

	showing_continue = false;

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
	int playerIndex = combatState->player_index_selected;
	std::vector<Attack>& attacks = combatData->players[playerIndex].attacks;

	if (combatState->magicPoints <= 0)
	{
		LOG("No magic points left!");
		return;
	}

	int attackIndex = ID - 1;
	if (attackIndex < 0 || attackIndex >= (int)attacks.size())
		return;

	combatState->player_attack_dmg_selected = attacks[attackIndex].dmg;
	if (godMode) combatState->player_attack_dmg_selected = 999;

	combatState->player_attack_index_selected = attackIndex;

	LOG("Attack ID: %i: Damage: %i, name: %s, effect: %s",
		ID,
		combatState->player_attack_dmg_selected,
		attacks[attackIndex].name,
		attacks[attackIndex].effect.c_str());
		
	combatState->selecting_target = true;
	LOG("Select enemy with LEFT/RIGHT \t Press ENTER to confirm.");

	int enemyIdx = combatState->enemy_index_targeted;
	LOG("Currently targeting Enemy ID: %i", combatData->enemies[enemyIdx].id);
}

void CombatManager::ApplyCombatLogic()
{
	if (combatState->turn == "Player")
	{
		CheckAlive();

		Combatant& player = combatData->players[combatState->player_index_selected];
		Combatant& enemy = combatData->enemies[combatState->enemy_index_targeted];
		Attack& attack = player.attacks[combatState->player_attack_index_selected];

		if (!(player.status == "Paralized"))
		{
			if (player.status == "Confused") //50% chance
			{
				bool can_attack = rand() % 2;
				if (can_attack)
				{
					LOG("Player attacked while being confused.");
					MakeAttack(enemy, combatData->players[combatState->player_index_selected], attack);
				}
				else { LOG("Player didn't attack while being confused."); }
			}
			else { MakeAttack(enemy, combatData->players[combatState->player_index_selected], attack); }
		}
		else { LOG("Player is paralized! Choose another one. Skip turn if all are."); return; }
		
		combatState->magicPoints -= attack.magicPoints;

		LOG("Magic Points: %i.", combatState->magicPoints);

		combatState->turn = "Enemy";
		CheckAlive();
	}

	if (combatState->turn == "Enemy")
	{
		CheckAlive();
		EnemyAI();
		combatState->turn = "Player";
	}

	ApplyEffects();
	CheckAlive();
}

void CombatManager::ApplyEffects()
{
	for (auto& enemy : combatData->enemies)
	{
		if (enemy.alive)
		{
			std::string effect = enemy.status;

			if (effect == "none") {}
			if (effect == "poisoned") { enemy.hp -= POISON_DAMAGE; LOG("Enemy ID: %i takes poison damage. HP: %i", enemy.id, enemy.hp); }
			if (effect == "paralized")
			{
				if (enemy.status_duration == 2)
				{
					enemy.status = "none";
					LOG("Enemy ID: %i is no longer paralized.", enemy.id);
					enemy.status_duration = 0;
				}
				else { enemy.status_duration++; LOG("Enemy ID: %i remains paralized. Remaining turns: %i", enemy.id, 3-enemy.status_duration); /*When status_duration is 0, the next turn will not attack.*/ } 
			}
			if (effect == "heal") {}
			if (enemy.shield_and_buff.first)
			{
				if (enemy.status_duration == 1)
				{
					enemy.shield_and_buff.first = false;
					LOG("Enemy ID: %i has no longer a shield.", enemy.id);
					enemy.status_duration = 0;
				}
				else{ enemy.status_duration++; }
			}
			if (enemy.shield_and_buff.second)
			{
				if (enemy.status_duration == 1)
				{
					enemy.shield_and_buff.second = false;
					LOG("Enemy ID: %i has no longer a buff.", enemy.id);
					enemy.status_duration = 0;
				}
				else { enemy.status_duration++; }
			}
		}
	}
	for (auto& player : combatData->players)
	{
		if (player.alive)
		{
			std::string effect = player.status;

			if (effect == "none") {}
			if (effect == "poisoned") { player.hp -= POISON_DAMAGE; LOG("Player ID: %i takes poison damage. HP: %i", player.id, player.hp); }
			if (effect == "paralized")
			{
				if (player.status_duration == 2)
				{
					player.status = "none";
					LOG("Player ID: %i is no longer paralized.", player.id);
					player.status_duration = 0;
				}
				else { player.status_duration++; LOG("Player ID: %i remains paralized. Remaining turns: %i", player.id, 3 - player.status_duration);}
			}
			if (effect == "heal") {}
			if (player.shield_and_buff.first) 
			{
				if (player.status_duration == 1)
				{
					player.shield_and_buff.first = false;
					LOG("Player ID: %i has no longer a shield.", player.id);
					player.status_duration = 0;
				}
				else { player.status_duration++; }
			}
			if (player.shield_and_buff.second)
			{
				if (player.status_duration == 1)
				{
					player.shield_and_buff.second = false;
					LOG("Player ID: %i has no longer a buff.", player.id);
					player.status_duration = 0;
				}
				else { player.status_duration++; }
			}
		}
	}
}

void CombatManager::HandleTargetSelection()
{
	int maxEnemies = (int)combatData->enemies.size();

	// RIGHT -->
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		int next = combatState->enemy_index_targeted;

		for (int i = 0; i < maxEnemies; ++i)
		{
			next = (next + 1) % maxEnemies;

			if (combatData->enemies[next].alive)
			{
				combatState->enemy_index_targeted = next;
				LOG("Targeting Enemy ID: %i", combatData->enemies[next].id);
				break;
			}
		}
	}

	// LEFT <--
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		int prev = combatState->enemy_index_targeted;

		for (int i = 0; i < maxEnemies; ++i)
		{
			prev = (prev - 1 + maxEnemies) % maxEnemies;

			if (combatData->enemies[prev].alive)
			{
				combatState->enemy_index_targeted = prev;
				LOG("Targeting Enemy ID: %i", combatData->enemies[prev].id);
				break;
			}
		}
	}

	// ENTER
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		int idx = combatState->enemy_index_targeted;
		LOG("Confirmed attack on Enemy ID: %i", combatData->enemies[idx].id);

		combatState->selecting_target = false;
		ApplyCombatLogic();
	}
}

void CombatManager::EnemyAI()
{
    std::vector<int> possibleIndices;
    for (int i = 0; i < (int)combatData->enemies.size(); ++i)
    {
        if (combatData->enemies[i].alive && !(combatData->enemies[i].status == "paralized"))
			possibleIndices.push_back(i);
    }

	if (possibleIndices.empty())
	{
		LOG("All enemies are paralized! Player's turn.");
		return;
	}

    int random_index_ID = possibleIndices[rand() % possibleIndices.size()];
    Combatant& enemy = combatData->enemies[random_index_ID];
	Combatant& player = combatData->players[combatState->player_index_selected];

    if (enemy.attacks.empty()) return;

    int random_index_attack = rand() % enemy.attacks.size();
    Attack& attack = enemy.attacks[random_index_attack];

	if (enemy.status == "Confused")
	{
		bool can_attack = rand() % 2;
		if (can_attack)
		{
			LOG("Enemy attack while being confused.");
			MakeAttack(player, enemy, attack);
		}
		else { LOG("Enemy didn't attack while being confused."); return; }
	}
	else
	{
		MakeAttack(player, enemy, attack);
	}
}

void CombatManager::ShowButtonStart(Vector2D position, int enemy_ID, int fight_ID)
{
	std::dynamic_pointer_cast<UIButton>(Engine::GetInstance().uiManager->CreateUIElement(UIElementType::BUTTON, 6, "Start combat", { (int)position.getX(), (int)position.getY(), 120, 20 }, this));
	LOG("Start dialogue button created at %i, %i.", (int)position.getX(), (int)position.getY());

	GetTreeAttributes(fight_ID, false); //get dialogue_tree from xml
	combatData->possible_enemy_ID = enemy_ID;

	showingButtonStart = true;
}


bool CombatManager::ShowAttackOptions(int player_ID)
{
	LOG("ShowAttackOptions called");
	UnloadCombatUI();
	choosingAtk = true;

	// find player index by ID
	int playerIndex = 0;
	for (int i = 0; i < (int)combatData->players.size(); ++i)
	{
		if (combatData->players[i].id == player_ID)
		{
			playerIndex = i;
			break;
		}
	}

	auto& attacks = combatData->players[playerIndex].attacks;
	int size = attacks.size();
	std::vector<std::pair<SDL_Rect, const char*>> buttonsAttack;

	SDL_Rect bt1Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 - 65, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 100, 300,50 };

	SDL_Rect bt2Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 365, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 100, 300,50 };

	SDL_Rect bt3Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 - 65, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 200, 300,50 };

	SDL_Rect bt4Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 365, Engine::GetInstance().window->GetWindowSize().getY() / 4 + 200, 300,50 };

	std::vector<SDL_Rect> bPos;
	bPos.push_back(bt1Pos); bPos.push_back(bt2Pos); bPos.push_back(bt3Pos); bPos.push_back(bt4Pos);
	//create attack buttons
	int bPosCount = 0;
	for (int i = 0; i < size; ++i) 
	{
		std::pair<SDL_Rect, const char*> p(bPos[bPosCount], attacks[i].name);
		if (attacks[i].unlocked) { 
			buttonsAttack.push_back(p); 
			bPosCount++;
		}
	}
	int id = 1;
	for (auto a : buttonsAttack)
	{
		std::dynamic_pointer_cast<UIButton>(
			Engine::GetInstance().uiManager->CreateUIElement(
				UIElementType::BUTTON, id, a.second, a.first, this));
		id++;
	}

	//create "back" button
	SDL_Rect bt5Pos = { Engine::GetInstance().window->GetWindowSize().getX() / 8 + 700,
						Engine::GetInstance().window->GetWindowSize().getY() / 4 + 300, 120,20 };
	std::dynamic_pointer_cast<UIButton>(
		Engine::GetInstance().uiManager->CreateUIElement(
			UIElementType::BUTTON, 10, "Back", bt5Pos, Engine::GetInstance().scene.get()));

	return true;
}

bool CombatManager::ShowItemOptions(int player_ID) {
	LOG("ShowItemOptions called");

	showInventory = true;

	return true;
}

bool CombatManager::ChangePlayer()
{
	int current = combatState->player_index_selected;
	int total = (int)combatData->players.size();

	for (int i = current + 1; i < total; ++i)
	{
		if (combatData->players[i].alive)
		{
			combatState->player_index_selected = i;
			return true;
		}
	}
	for (int i = 0; i < current; ++i)
	{
		if (combatData->players[i].alive)
		{
			combatState->player_index_selected = i;
			return true;
		}
	}
	return true;
}


void CombatManager::GetTreeAttributes(int fight_ID, bool all)
{
		if (in_combat) return;
		combatData->Clear();

		combatData->fight_ID = fight_ID;
		combatState->magicPoints = Engine::GetInstance().map->magicPoints;

		std::vector<int> players_id;
		std::vector<int> enemies_id;

		for (pugi::xml_node fight_tree_node = combatFileXML.child("combat").child("fight");
			fight_tree_node != NULL;
			fight_tree_node = fight_tree_node.next_sibling("fight"))
		{
			if(all) {
				
			}
			else if (fight_tree_node.attribute("id").as_int() == fight_ID)
			{
				std::string players_id_str = fight_tree_node.attribute("players_id").as_string();
				std::string enemies_id_str = fight_tree_node.attribute("enemies_id").as_string();
				players_id = GetIDs(players_id_str);
				enemies_id = GetIDs(enemies_id_str);
				break;
			}
		}

		// players
		for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("player");
			combat_tree_node != NULL;
			combat_tree_node = combat_tree_node.next_sibling("player"))
		{	
			int id = combat_tree_node.attribute("id").as_int();
			if (!all)
			if (!Contains(players_id, id)) continue;

			Combatant player;
			player.id = id;
			player.hp = combat_tree_node.attribute("HP").as_int();
			player.type = EntityType::PLAYER;
			player.status_duration = 0;

			for (pugi::xml_node current_node = combat_tree_node.child("attack_stats");
				current_node != NULL;
				current_node = current_node.next_sibling("attack_stats"))
			{
				Attack attack;
				attack.name = current_node.attribute("name").as_string();
				attack.dmg = current_node.attribute("dmg").as_int();
				attack.magicPoints = current_node.attribute("magicPoints").as_int();
				attack.effect = current_node.attribute("effect").as_string();
				attack.unlocked = current_node.attribute("unlocked").as_bool();
				player.attacks.push_back(attack);
			}

			combatData->players.push_back(player);
		}

		// enemies
		for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("enemy");
			combat_tree_node != NULL;
			combat_tree_node = combat_tree_node.next_sibling("enemy"))
		{
			int id = combat_tree_node.attribute("id").as_int();
			if (!all)
			if (!Contains(enemies_id, id)) continue;

			Combatant enemy;
			enemy.id = id;
			enemy.hp = combat_tree_node.attribute("HP").as_int();
			enemy.type = EntityType::BASEENEMY;
			enemy.status_duration = 0;

			for (pugi::xml_node current_node = combat_tree_node.child("attack");
				current_node != NULL;
				current_node = current_node.next_sibling("attack"))
			{
				Attack attack;
				attack.name = current_node.attribute("name").as_string();
				attack.dmg = current_node.attribute("dmg").as_int();
				attack.effect = current_node.attribute("effect").as_string();
				enemy.attacks.push_back(attack);
			}

			combatData->enemies.push_back(enemy);
		}
}

void CombatManager::CheckAlive()
{
	// enemies
	for (int i = 0; i < (int)combatData->enemies.size(); ++i)
	{
		Combatant& e = combatData->enemies[i];
		if (e.hp <= 0 && e.alive)
		{
			e.alive = false;
			LOG("Enemy ID: %i has been killed.", e.id);

			//std::shared_ptr<Entity> entity = Engine::GetInstance().entityManager->GetEnemy(e.id); //get enemy from the id
			//enemies_to_destroy.push_back(entity); //entities is empty because we delete all from the previous scene, I need to make a function that changes the 
			// combat file values

			for (int j = 0; j < (int)combatData->enemies.size(); ++j)
			{
				if (combatData->enemies[j].alive)
				{
					combatState->enemy_index_targeted = j;
					LOG("Now targeting enemy ID: %i", combatData->enemies[j].id);
					break;
				}
			}
		}
	}

	// players
	for (int i = 0; i < (int)combatData->players.size(); ++i)
	{
		Combatant& p = combatData->players[i];
		if (p.hp <= 0 && p.alive)
		{
			p.alive = false;
			for (int j = 0; j < (int)combatData->players.size(); ++j)
			{
				if (combatData->players[j].alive)
				{
					combatState->player_index_selected = j;
					break;
				}
			}
		}
	}
	// check enemy wins
	int alivePlayers = 0;
	for (auto& p : combatData->players)
		if (p.alive) ++alivePlayers;
	if (alivePlayers == 0) combatState->enemy_Wins = true;

	// check player wins
	int aliveEnemies = 0;
	for (auto& e : combatData->enemies)
		if (e.alive) ++aliveEnemies;
	if (aliveEnemies == 0) combatState->player_Wins = true;

	if (combatState->enemy_Wins)
	{
		LOG("Enemies win the combat.");
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1);
		in_combat = false;
		enemies_to_destroy.clear();
	}
	else if (combatState->player_Wins) //delete the enemies you killed
	{
		LOG("Player wins the combat. Destroying the enemies...");
		MarkEnemiesAsDead();
		in_combat = false;
		enemies_to_destroy.clear();
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1);
	}
}

void CombatManager::MarkEnemiesAsDead()
{
	enemies_to_destroy.clear();
	for (auto enemy : combatData->enemies)
	{
		if(!enemy.alive) enemies_to_destroy.push_back(enemy.id);
	}
	Engine::GetInstance().map->UpdateEnemiesData();
}

void CombatManager::UnlockAttack(EntityType type, const char* name)
{
	if (type == EntityType::PLAYER)
	{
		for (auto player : combatData->players)
		{			
			for (auto attack : player.attacks)
			{
				if (std::strcmp(attack.name,name) == 0) { attack.unlocked = true; LOG("Attack: %s from player ID: %i unlocked.", attack.name, player.id); /*Save combat data file*/ return; }
			}
		}
	}
	SaveTreeAttributes();
}

void CombatManager::SaveTreeAttributes()
{
	Engine::GetInstance().map->magicPoints = combatState->magicPoints;
	//for (pugi::xml_node fight_tree_node = combatFileXML.child("combat").child("fight");
	//	fight_tree_node != NULL;
	//	fight_tree_node = fight_tree_node.next_sibling("fight"))
	//{
	//	/*if (fight_tree_node.attribute("id").as_int() == fight_ID)
	//	{
	//		std::string players_id_str = fight_tree_node.attribute("players_id").as_string();
	//		std::string enemies_id_str = fight_tree_node.attribute("enemies_id").as_string();
	//		players_id = GetIDs(players_id_str);
	//		enemies_id = GetIDs(enemies_id_str);
	//		break;
	//	}*/
	//}

	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("player");
		combat_tree_node != NULL;
		combat_tree_node = combat_tree_node.next_sibling("player"))
	{
		int id = combat_tree_node.attribute("id").as_int();
		/*if (!combatData->players[id].alive) continue;*/

		Combatant player = combatData->players[id-1];
		int attack_id = 0;
		for (pugi::xml_node current_node = combat_tree_node.child("attack_stats");
			current_node != NULL;
			current_node = current_node.next_sibling("attack_stats"))
		{
			Attack attack = combatData->players[id-1].attacks[attack_id];
			current_node.attribute("unlocked").set_value(attack.unlocked);
			attack_id++;
		}
	}
}