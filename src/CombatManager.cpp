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
#include "QuestManager.h"
#include "DialogueManager.h"
#include "RewardManager.h"
#include "EntityManager.h"
#include "Audio.h"
#include "BaseEnemy.h"
#include "BaseCompanion.h"
#include "Animation.h"
#include <unordered_map>


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
	playerHealthbar = Engine::GetInstance().textures->Load("Assets/Textures/healthbarplayer.png");


	std::unordered_map<int, std::string> emptyAliases;

	Shield_Texture = Engine::GetInstance().textures->Load("Assets/Textures/Combat/Shield_buff_Sheet.png");
	Shield_Anim.LoadFromTSX("Assets/Textures/Combat/Shield_buff_Sheet.tsx", emptyAliases);

	Poison_Texture = Engine::GetInstance().textures->Load("Assets/Textures/Combat/posion-Sheet.png");
	Poison_Anim.LoadFromTSX("Assets/Textures/Combat/posion-Sheet.tsx", emptyAliases);

	Paralized_Texture = Engine::GetInstance().textures->Load("Assets/Textures/Combat/paralized-Sheet.png");
	Paralized_Anim.LoadFromTSX("Assets/Textures/Combat/paralized-Sheet.tsx", emptyAliases);

	return true;
}

bool CombatManager::Update(float dt)
{

	if (!in_combat) {
		return true;
	}

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


	if (in_combat && Engine::GetInstance().scene->GetCurrentScene() == SceneID::BATTLE)
	{
		RenderCombatants(dt);
		
	}



	for (auto it = floatingTexts.begin(); it != floatingTexts.end();)
	{
		it->timer += dt;

		// movimiento hacia arriba
		it->position.setY(it->position.getY() - 0.05f * dt);

		if (it->timer >= it->duration)
		{
			it = floatingTexts.erase(it);
		}
		else
		{
			++it;
		}
	}

	// COMBAT TIMERS
	

	if (waitingAttack || waitingDamage || waitingEffect || waitingEnemyTurn)
	{
		combatTimer += dt;
	}

	// Esperando efectos pasivos
	if (waitingEffect)
	{
		if (combatTimer >= EFFECT_DELAY)
		{
			waitingEffect = false;

			combatTimer = 0.0f;

			// aplicar efectos SOLO al turno actual
			if (combatState->turn == "Player")
			{
				ApplyPlayerEffects();
			}
			else
			{
				ApplyEnemyEffects();
			}

			CheckAlive();

			// si el enemigo sigue vivo -> IA
			if (combatState->turn == "Enemy")
			{
				EnemyAI();
			}
		}
	}

	// Esperando animación de ataque
	if (waitingAttack)
	{
		if (currentAttacker != nullptr)
		{
			// si ya NO está en attack,
			// significa que terminó

			if (currentAttacker->anims.GetCurrentName() != "attack")
			{
				waitingAttack = false;

				waitingDamage = true;

				combatTimer = 0.0f;
			}
		}
	}

	// Esperando aplicar daño
	if (waitingDamage)
	{
		if (combatTimer >= DAMAGE_DELAY)
		{
			waitingDamage = false;

			combatTimer = 0.0f;

			currentTarget->hp -= pendingDamage;
			SpawnFloatingText( "-" + std::to_string(pendingDamage), currentTarget->position.getX(), currentTarget->position.getY() - 40, { 255,0,0,255 } );

			currentTarget->anims.SetCurrent("hit");

			LOG("Damage applied: %i", pendingDamage);

			CheckAlive();

			waitingEnemyTurn = true;
		}
	}

	// Delay antes siguiente turno
	if (waitingEnemyTurn)
	{
		if (combatTimer >= ENEMY_TURN_DELAY)
		{
			waitingEnemyTurn = false;

			combatTimer = 0.0f;

			// cambiar turno
			if (combatState->turn == "Player")
			{
				combatState->turn = "Enemy";
			}
			else
			{
				combatState->turn = "Player";

				PlayerHasAttacked = false;

				Engine::GetInstance().scene->LoadBattle();
			}

			// empezar fase efectos
			waitingEffect = true;
		}
	}

	//Debug
	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		LOG("Players size: %i", combatData->players.size());
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

	goBack = Engine::GetInstance().scene->GetCurrentScene(); 
	Engine::GetInstance().scene->sceneStack.push(goBack);
	Engine::GetInstance().scene->ChangeScene(SceneID::BATTLE);
	in_combat = true;
	Engine::GetInstance().render->camera.x = 0;
	Engine::GetInstance().render->camera.y = 0;

	combatState->Init(*combatData);
	combatState->player_index_selected = 0; // first player


	// Alineation:
	//		0		   !
	//		 0		  !
	//		  0		 !
	//		   0	!

	//Good guys
	float Xinitialpos = Engine::GetInstance().render->camera.w / 4;
	float Yinitialpos = 2 *Engine::GetInstance().render->camera.h / 3;
	float Xincrement = 0.0f;
	float Yincrement = 0.0f;
	
	for (int i = 0; i < combatData->players.size(); i++) {
		combatData->players[i].position = { Xinitialpos + Xincrement, Yinitialpos + Yincrement };
		Xincrement += 80.0f;
		Yincrement += 70.0f;
	}

	//Bad guys
	float Xpos = 3 * Engine::GetInstance().render->camera.w / 4;
	float Ypos = 2 *Engine::GetInstance().render->camera.h / 3;
	float Xinc = 0.0f;
	float Yinc = 0.0f;
 
	for (int i = 0; i < combatData->enemies.size(); i++) {
		combatData->enemies[i].position = { Xpos - Xinc, Ypos + Yinc };
		Xinc += 70.0f;
		Yinc += 90.0f;
	}


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
	case 1:
		if (can_be_clicked) {
			ButtonAction(1);
		}
		break;
	case 2:
		if (can_be_clicked) {
			ButtonAction(2);
		}
		break;
	case 3:
		if (can_be_clicked) {
			ButtonAction(3);
		}
		break;
	case 4:
		if (can_be_clicked) {
			ButtonAction(4);
		}
		break;
	case 5:
		UnloadCombatUI();
		Engine::GetInstance().render->StartTextDisplay("", 0.0f);
		in_combat = false;
		LOG("Cleaned combat UI.");
		Engine::GetInstance().scene->ChangeScene(SceneID::LEVEL1);
		break;
	case 6:
		UnloadCombatUI();
		showingButtonStart = false;
		StartCombat(/*combatData->players_id, combatData->enemies_id*/);
		LOG("Combat starts.");
		break;
	case 10:
		//Back
		UnloadCombatUI();
		choosingAtk = false;
		combatState->selecting_target = false;
		Engine::GetInstance().scene->LoadBattle();
		LOG("Returned from attack options to battle UI");
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

	if (combatState->magicPoints <= 0)
	{
		LOG("No magic points left!");
		return;
	}
	if (combatState->magicPoints-attacks[attackIndex].magicPoints <= 0)
	{
		LOG("Not enough Magic Points to use the attack!");
		return;
	}
		
	combatState->selecting_target = true;
	LOG("Select enemy with LEFT/RIGHT \t Press ENTER to confirm.");

	int enemyIdx = combatState->enemy_index_targeted;
	LOG("Currently targeting Enemy ID: %i", combatData->enemies[enemyIdx].id);
}

void CombatManager::ApplyCombatLogic()
{
	if (waitingAttack || waitingDamage || waitingEffect || waitingEnemyTurn)
		return;

	if (combatState->turn == "Player")
	{
		Combatant& player = combatData->players[combatState->player_index_selected];
		Combatant& enemy = combatData->enemies[combatState->enemy_index_targeted];
		Attack& attack = player.attacks[combatState->player_attack_index_selected];

		// comprobar mana
		if (combatState->magicPoints < attack.magicPoints)
		{
			LOG("Not enough magic points!");
			return;
		}

		// consumir mana AQUÍ
		combatState->magicPoints -= attack.magicPoints;

		MakeAttack(enemy, player, attack);
	}
}

void CombatManager::MakeAttack(Combatant& target, Combatant& attacker, Attack attack)
{
	if (std::string(attack.name) == "Jet Punch")
	{
		Engine::GetInstance().audio->PlayFx(s_punch, 0);
	}
	else if (std::string(attack.name) == "Low kick chill")
	{
		Engine::GetInstance().audio->PlayFx(s_kick, 0);
	}

	attacker.anims.SetLoopFor("attack", false);
	attacker.anims.SetCurrent("attack");

	target.anims.SetLoopFor("hit", false);

	// NO poner hit todavía
	// porque el daño llegará después
	if (attack.effect == "heal")
	{
		attacker.hp += HEAL_HITPOINTS;
		SpawnFloatingText( "+" + std::to_string(HEAL_HITPOINTS), attacker.position.getX(), attacker.position.getY() - 40, { 0,255,255,255 } );
		if (attacker.type == EntityType::PLAYER)
			LOG("Player ID: %i healed for %i.", attacker.id, HEAL_HITPOINTS);
		else
			LOG("Enemy ID: %i healed for %i.", attacker.id, HEAL_HITPOINTS);
	}
	else if (attack.effect == "selfKO")
	{
		attacker.hp = 0;

		if (attacker.type == EntityType::PLAYER)
			LOG("Player ID: %i selfKOed.", attacker.id);
		else
			LOG("Enemy ID: %i selfKOed.", attacker.id);
	}
	else if (attack.effect == "ragebait")
	{
		target.hp = 0;

		if (target.type == EntityType::PLAYER)
			LOG("Player ID: %i falls for the ragebait.", target.id);
		else
			LOG("Enemy ID: %i falls for the ragebait.", target.id);
	}
	else if (attack.effect == "shield")
	{
		attacker.shield_and_buff.first = true;
		SpawnFloatingText( "SHIELDED!", attacker.position.getX(), attacker.position.getY() - 60, { 0,255,255,255 } );
	}
	else if (attack.effect == "buff")
	{
		attacker.shield_and_buff.second = true;
	}
	else if (attack.effect == "poisoned")
	{
		target.status = attack.effect;
		SpawnFloatingText( "POISONED!", target.position.getX(), target.position.getY() - 60, { 0,255,0,255 } );
	}
	else if (attack.effect == "paralized")
	{
		target.status = attack.effect;
		SpawnFloatingText("PARALIZED!", target.position.getX(), target.position.getY() - 60, { 255,255,255,255 });
	}
	else if (attack.effect != "none")
	{
		target.status = attack.effect;
	}


	int dmg_increase = 0;
	int dmg_reduction = 0;
	int confused_probability = 0;

	Engine::GetInstance().itemManager->ApplyCombatItems(
		dmg_increase,
		dmg_reduction,
		confused_probability
	);

	// shield
	if (target.shield_and_buff.first)
	{
		dmg_reduction += SHIELD_DMG_REDUCTION;
	}

	// buff
	if (attacker.shield_and_buff.second)
	{
		dmg_increase += BUFF_DMG_INCREASE;
	}

	// calcular daño

	int dmg_applied = attack.dmg + dmg_increase - dmg_reduction;

	if (dmg_applied < 0)
		dmg_applied = 0;

	// CONFUSED ITEM


	if (confused_probability != 0 &&
		attacker.type == EntityType::BASEENEMY)
	{
		if (!CanAttack(100 - confused_probability))
		{
			LOG("Enemy couldn't attack because of the item.");
			return;
		}
	}


	// guardar ataque pendiente

	currentAttacker = &attacker;
	currentTarget = &target;
	currentAttack = attack;

	pendingDamage = dmg_applied;

	waitingAttack = true;
	combatTimer = 0.0f;

	// LOGS

	if (attacker.type == EntityType::PLAYER)
	{
		LOG("Player ID: %i attacks %i for %i dmg",
			attacker.id,
			target.id,
			dmg_applied);
	}
	else
	{
		LOG("Enemy ID: %i attacks %i for %i dmg",
			attacker.id,
			target.id,
			dmg_applied);
	}
}
void CombatManager::ApplyPlayerEffects()
{
	for (auto& player : combatData->players)
	{
		if (!player.alive)
			continue;

		std::string effect = player.status;

		if (effect == "poisoned")
		{
			player.hp -= POISON_DAMAGE;

			LOG("Player ID: %i takes poison damage. HP: %i",
				player.id,
				player.hp);
		}

		if (effect == "paralized")
		{
			if (player.status_duration == 2)
			{
				player.status = "none";

				LOG("Player ID: %i is no longer paralized.",
					player.id);

				player.status_duration = 0;
			}
			else
			{
				player.status_duration++;

				LOG("Player ID: %i remains paralized. Remaining turns: %i",
					player.id,
					3 - player.status_duration);
			}
		}

		// shield
		if (player.shield_and_buff.first)
		{
			if (player.status_duration == 1)
			{
				player.shield_and_buff.first = false;

				LOG("Player ID: %i has no longer a shield.",
					player.id);

				player.status_duration = 0;
			}
			else
			{
				player.status_duration++;
			}
		}

		// buff
		if (player.shield_and_buff.second)
		{
			if (player.status_duration == 1)
			{
				player.shield_and_buff.second = false;

				LOG("Player ID: %i has no longer a buff.",
					player.id);

				player.status_duration = 0;
			}
			else
			{
				player.status_duration++;
			}
		}
	}
}

void CombatManager::ApplyEnemyEffects()
{
	for (auto& enemy : combatData->enemies)
	{
		if (!enemy.alive)
			continue;

		std::string effect = enemy.status;

		if (effect == "poisoned")
		{
			enemy.hp -= POISON_DAMAGE;
			SpawnFloatingText("-" + std::to_string(POISON_DAMAGE), enemy.position.getX(), enemy.position.getY() - 40, { 0,255,0,255 } );
			LOG("Enemy ID: %i takes poison damage. HP: %i",
				enemy.id,
				enemy.hp);
		}

		if (effect == "paralized")
		{
			if (enemy.status_duration == 2)
			{
				enemy.status = "none";

				LOG("Enemy ID: %i is no longer paralized.",
					enemy.id);

				enemy.status_duration = 0;
			}
			else
			{
				enemy.status_duration++;

				LOG("Enemy ID: %i remains paralized. Remaining turns: %i",
					enemy.id,
					3 - enemy.status_duration);
			}
		}

		// shield
		if (enemy.shield_and_buff.first)
		{
			if (enemy.status_duration == 1)
			{
				enemy.shield_and_buff.first = false;

				LOG("Enemy ID: %i has no longer a shield.",
					enemy.id);

				enemy.status_duration = 0;
			}
			else
			{
				enemy.status_duration++;
			}
		}

		// buff
		if (enemy.shield_and_buff.second)
		{
			if (enemy.status_duration == 1)
			{
				enemy.shield_and_buff.second = false;

				LOG("Enemy ID: %i has no longer a buff.",
					enemy.id);

				enemy.status_duration = 0;
			}
			else
			{
				enemy.status_duration++;
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
	bool confirmAttack = Engine::GetInstance().input->GetButton(SDL_GAMEPAD_BUTTON_WEST);
	if ((Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) || confirmAttack)
	{
		int idx = combatState->enemy_index_targeted;

		LOG("Confirmed attack on Enemy ID: %i",
			combatData->enemies[idx].id);

		PlayerHasAttacked = true;

		// destruir UI
		UnloadCombatUI();

		choosingAtk = false;
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

		waitingEnemyTurn = true;
		combatTimer = ENEMY_TURN_DELAY;

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
	if (combatState->turn != "Player")
	{
		LOG("Not player's turn.");
		return false;
	}

	if (PlayerHasAttacked)
	{
		LOG("Player already acted this turn.");
		return false;
	}

	UnloadCombatUI();
	choosingAtk = true;

	// find player index by ID
	/*int playerIndex = 0;
	for (int i = 0; i < (int)combatData->players.size(); ++i)
	{
		if (combatData->players[i].id == player_ID)
		{
			playerIndex = i;
			break;
		}
	}*/

	auto& attacks = combatData->players[combatState->player_index_selected].attacks;
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
			UIElementType::BUTTON, 10, "Back", bt5Pos, this));

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
		/*combatState->magicPoints = Engine::GetInstance().map->magicPoints;*/
		combatState->magicPoints = 50;

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

				if (isWizardActive) { players_id.push_back(2); }
				if (isCorneliusActive) { players_id.push_back(3); }
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
			player.maxhp = player.hp;
			if (id == 1)
			{
				player.type = EntityType::PLAYER;
			}
			else {
				player.type = EntityType::BASECOMPANION;
			}
			
			player.status_duration = 0;

			// animation
			player.texturePath = combat_tree_node.attribute("texturePath").as_string();
			player.anim_tsxpath = combat_tree_node.attribute("anim_tsxpath").as_string();

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
			enemy.maxhp = enemy.hp;
			enemy.type = EntityType::BASEENEMY;
			enemy.status_duration = 0;

			// animations
			enemy.texturePath = combat_tree_node.attribute("texturePath").as_string();
			enemy.anim_tsxpath = combat_tree_node.attribute("anim_tsxpath").as_string();

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

		// Cargar recursos (texturas/animaciones) para cada combatant si se especificó la ruta
		std::unordered_map<int, std::string> emptyAliases; // si necesitas alias, cambia esto
		for (auto& p : combatData->players)
		{
			if (!p.texturePath.empty())
			{
				p.texture = Engine::GetInstance().textures->Load(p.texturePath.c_str());
				if (p.texture) Engine::GetInstance().textures->GetSize(p.texture, p.texW, p.texH);
			}
			// Hardcodear los aliases para cada player
			std::unordered_map<int, std::string> aliases = emptyAliases;
			if (p.id == 1)
			{
				aliases = { {0, "idle"}, {0, "attack"}, {0, "hit"}, {0, "hit"} };
			}
			if (!p.anim_tsxpath.empty())
			{
				if (p.anims.LoadFromTSX(p.anim_tsxpath.c_str(), emptyAliases))
				{
					// intenta usar "idle" si existe, sino queda la primera anim por defecto
					if (p.anims.Has("idle")) 
					{ 
						p.anims.SetCurrent("idle"); 
					}
					// idle = loop
					// attack/hit  = no loop
					if (p.anims.Has("idle")) { p.anims.SetLoopFor("idle", true); }
					if (p.anims.Has("attack")) { p.anims.SetLoopFor("attack", false); }
					if (p.anims.Has("hit")) { p.anims.SetLoopFor("hit", false); }
				}
			}
		}
		for (auto& e : combatData->enemies)
		{
			if (!e.texturePath.empty())
			{
				e.texture = Engine::GetInstance().textures->Load(e.texturePath.c_str());
				if (e.texture) Engine::GetInstance().textures->GetSize(e.texture, e.texW, e.texH);
			}
			// Hardcodear los aliases para cada enemigo
			std::unordered_map<int, std::string> aliases = emptyAliases;
			if (e.id == 1)
			{
				aliases = { {0, "idle"}, {4, "attack"}, {11, "hit"}, {16, "hit"} };
			}
			if (!e.anim_tsxpath.empty())
			{
				if (e.anims.LoadFromTSX(e.anim_tsxpath.c_str(), aliases))
				{
					if (e.anims.Has("idle")) e.anims.SetCurrent("idle");
					// idle = loop
					// attack/hit = no loop
					if (e.anims.Has("idle")) { e.anims.SetLoopFor("idle", true); }
					if (e.anims.Has("attack")) { e.anims.SetLoopFor("attack", false); }
					if (e.anims.Has("hit")) { e.anims.SetLoopFor("hit", false); }
				}
			}
		}
}

void CombatManager::CheckAlive()
{
	if (combatState->player_Wins || combatState->enemy_Wins) return;
	// enemies
	for (int i = 0; i < (int)combatData->enemies.size(); ++i)
	{
		Combatant& e = combatData->enemies[i];
		if (e.hp <= 0 && e.alive)
		{
			e.alive = false;
			LOG("Enemy ID: %i has been killed.", e.id);

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

	if (combatState->magicPoints <= 0) combatState->enemy_Wins = true;

	if (combatState->enemy_Wins)
	{
		Engine::GetInstance().scene->ChangeScene(SceneID::LOSE);
		LOG("Enemies win the combat.");
		/*Engine::GetInstance().scene->ChangeScene(goBack);*/
		ResetCombatState();

		in_combat = false;

		enemies_to_destroy.clear();
	}
	else if (combatState->player_Wins) //delete the enemies you killed
	{
		PendingChange change;
		change.entityType = EntityType::PLAYER;
		change.type = Component::MAGICPOINTS;
		change.new_value = combatState->magicPoints;

		Engine::GetInstance().map->pendingChanges.emplace_back(change);
		LOG("Player wins the combat. Destroying the enemies...");
		MarkEnemiesAsDead();
		in_combat = false;
		enemies_to_destroy.clear();
		ResetCombatState();

		if (combatData->fight_ID == 104) //final boss
		{
			Engine::GetInstance().scene->ChangeScene(SceneID::WIN);
		}
		Engine::GetInstance().scene->ChangeScene(goBack);
		CanCombatQuestBeCompleted(combatData->fight_ID, true);
	}
}

void CombatManager::MarkEnemiesAsDead()
{
	enemies_to_destroy.clear();
	for (auto& enemy : combatData->enemies)
	{
		if(!enemy.alive) enemies_to_destroy.push_back(enemy.id);
	}
	Engine::GetInstance().map->UpdateEnemiesData();
}

void CombatManager::UnlockAttack(EntityType type, const char* name)
{
	if (type == EntityType::PLAYER)
	{
		for (auto& player : combatData->players)
		{			
			for (auto& attack : player.attacks)
			{
				if (std::strcmp(attack.name,name) == 0) { attack.unlocked = true; LOG("Attack: %s from player ID: %i unlocked.", attack.name, player.id); return; }
			}
		}
	}
	SaveTreeAttributes();
}

void CombatManager::SaveTreeAttributes()
{
	Engine::GetInstance().map->magicPoints = combatState->magicPoints;

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

void CombatManager::SaveScene()
{
	goBack = Engine::GetInstance().scene->GetCurrentScene();
	Engine::GetInstance().map->SaveEntities(Engine::GetInstance().scene->GetPlayer(), goBack);
}

void CombatManager::CanCombatQuestBeCompleted(int fight_ID, bool victory)
{
	if (!(victory)) return;
	switch (fight_ID)
	{
	case 2:
		if (Engine::GetInstance().questManager->IsQuestActive("Beat those guys!") && !Engine::GetInstance().questManager->IsQuestCompleted("Beat those guys!"))
		{
			Engine::GetInstance().questManager->CompleteQuest("Beat those guys!");
			Engine::GetInstance().dialogueManager->UnlockNewDialogueTree(4); //unlock next dialogue for NPC with ID = 4
		}
		break;
	case 102:
		if (Engine::GetInstance().questManager->IsQuestActive("Kill the boss") && !Engine::GetInstance().questManager->IsQuestCompleted("Kill the boss"))
		{
			Engine::GetInstance().questManager->CompleteQuest("Kill the boss");
			Engine::GetInstance().dialogueManager->UnlockNewDialogueTree(1); //unlock next dialogue for NPC with ID = 1
			Engine::GetInstance().questManager->ActivateQuest("Talk with the homeless guy");
		}
		break;
	}
}

std::vector<Attack> CombatManager::GetPlayerAttacks(int& HP)
{
	std::vector<Attack> attacks;

	// player
	for (pugi::xml_node combat_tree_node = combatFileXML.child("combat").child("player");
		combat_tree_node != NULL;
		combat_tree_node = combat_tree_node.next_sibling("player"))
	{
		HP = combat_tree_node.attribute("HP").as_int();
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
			attacks.push_back(attack);
		}
		break;
	}
	return attacks;
}

void CombatManager::RenderCombatants(float dt)
{
    // players
    for (int i = 0; i < combatData->players.size(); ++i)
    {
		auto& player = combatData->players[i];
        int x = (int)player.position.getX();
        int y = (int)player.position.getY();

		if(player.alive){

			//Draw
			if (player.anims.GetCurrentFrame().w > 0 && player.texture)
			{
				player.anims.Update(dt);
				const SDL_Rect& frame = player.anims.GetCurrentFrame();

				// center sprite
				int drawX = x - frame.w / 2;
				int drawY = y - frame.h / 2;

				Engine::GetInstance().render->DrawTexture(player.texture, drawX, drawY, &frame);

				// volver a idle
				if (player.anims.HasFinishedOnce() && player.anims.GetCurrentName() != "idle" && player.anims.Has("idle"))
				{
					player.anims.SetCurrent("idle");
				}

				//effects

				//shield
				if (player.shield_and_buff.first == true) {
					Shield_Anim.Update(dt);
					const SDL_Rect& shieldframe = Shield_Anim.GetCurrentFrame();
					int shieldX = drawX + (frame.w / 2) - (shieldframe.w / 2);
					int shieldY = drawY + (frame.h / 2) - (shieldframe.h / 2);
					Engine::GetInstance().render->DrawTexture(Shield_Texture, shieldX, shieldY, &shieldframe);
				}
				if (player.status == "poisoned") {
					Poison_Anim.Update(dt);
					const SDL_Rect& poisonframe = Poison_Anim.GetCurrentFrame();
					int px = drawX + (frame.w / 2) - (poisonframe.w / 2);
					int py = drawY - poisonframe.h / 2;
					Engine::GetInstance().render->DrawTexture(Poison_Texture, px, py, &poisonframe);
				}
				if (player.status == "paralized") {
					Paralized_Anim.Update(dt);
					const SDL_Rect& paralizedframe = Paralized_Anim.GetCurrentFrame();
					int px = drawX + (frame.w / 2) - (paralizedframe.w / 2);
					int py = drawY - paralizedframe.h / 2;
					Engine::GetInstance().render->DrawTexture(Paralized_Texture, px, py, &paralizedframe);
				}

				// healthbar

				// prota
				if (player.id == 1) {
			
					//healthbar
					int player_hpbar_posX = 64;
					int player_hpbar_posY = 64;

					int Wmax = 330; // maximum width
					int unity = Wmax / player.maxhp;
					player.hp_Interior.w = player.hp * unity;
					player.hp_Interior.x = player_hpbar_posX + 117;
					player.hp_Interior.y = player_hpbar_posY + 27;
					player.hp_Interior.h = 37; // maximum height (this doesn't change)
					Engine::GetInstance().render->DrawRectangle(player.hp_Interior, 255, 0, 0, 255, true);

					// print hp
					std::string s = std::to_string(player.hp);
					const char* pchar = s.c_str();

					Engine::GetInstance().render->DrawText(pchar, player.hp_Interior.x + 45 ,player.hp_Interior.y , 32, 32, { 255, 255, 255, 255 });

					//mana bar
					int player_manabar_posX = 64;
					int player_manabar_posY = 64;

					int Wmax_mana = 250; // maximum width
					int Munity = Wmax_mana / 50;
					SDL_Rect mana_rect = { player_manabar_posX + 110, player_manabar_posY + 65, combatState->magicPoints * Munity, 30};
					Engine::GetInstance().render->DrawRectangle(mana_rect, 0, 0, 255, 255, true);


					// print mana
					std::string s2 = std::to_string(combatState->magicPoints);
					const char* pchar2 = s2.c_str();

					Engine::GetInstance().render->DrawText(pchar2, player_manabar_posX + 110 + 110, player_manabar_posY + 70, 26, 26, { 255, 255, 255, 255 });




					Engine::GetInstance().render->DrawTexture(playerHealthbar, player_hpbar_posX, player_hpbar_posY, nullptr, 0.0f, 0.0, 0, 0, true);
				}
				else {

					//outline (position relative to sprite top-left)
					player.hp_outline.x = drawX;
					player.hp_outline.y = drawY;
					Engine::GetInstance().render->DrawRectangle(player.hp_outline, 0, 0, 0, 255, true);

					//inner part
					float Wmax = player.hp_outline.w;

					float unity = Wmax / player.maxhp;

					player.hp_Interior.w = player.hp * unity;

					player.hp_Interior.x = drawX - 2;
					player.hp_Interior.y = drawY - 2;

					player.hp_Interior.h = player.hp_outline.h;

					if (player.hp > 0) {
						//green
						Engine::GetInstance().render->DrawRectangle(player.hp_Interior, 0, 255, 0, 255, true);

						// print hp
						std::string s = std::to_string(player.hp);
						const char* pchar = s.c_str();

						Engine::GetInstance().render->DrawText(pchar, player.hp_Interior.x + Wmax, player.hp_Interior.y - 16, 32, 32, { 255, 255, 255, 255 });
					}
				}
			}
			else {
				// fallback texture (centered)
				SDL_Rect rect = { x - 32, y - 32, 64, 64 };
				Engine::GetInstance().render->DrawRectangle(rect, 0, 180, 255, 200, true);
			}
		}
    }

    // enemies
    for (int i = 0; i < combatData->enemies.size(); ++i)
    {
        auto& enemy = combatData->enemies[i];
        int x = (int)enemy.position.getX();
        int y = (int)enemy.position.getY();

		if (enemy.alive) {
			//draw
			if (enemy.anims.GetCurrentFrame().w > 0 && enemy.texture)
			{
				enemy.anims.Update(dt);
				const SDL_Rect& frame = enemy.anims.GetCurrentFrame();

				// center sprite on logical position
				int drawX = x - frame.w / 2;
				int drawY = y - frame.h / 2;

				Engine::GetInstance().render->DrawTexture(enemy.texture, drawX, drawY, &frame);

				// volver a idle
				if (enemy.anims.HasFinishedOnce() && enemy.anims.GetCurrentName() != "idle" && enemy.anims.Has("idle"))
				{
					enemy.anims.SetCurrent("idle");
				}

				//effects

				//shield (centered)
				if (enemy.shield_and_buff.first == true) {
					Shield_Anim.Update(dt);
					const SDL_Rect& shieldframe = Shield_Anim.GetCurrentFrame();
					int shieldX = drawX + (frame.w / 2) - (shieldframe.w / 2);
					int shieldY = drawY + (frame.h / 2) - (shieldframe.h / 2);
					Engine::GetInstance().render->DrawTexture(Shield_Texture, shieldX, shieldY, &shieldframe);
				}
				if (enemy.status == "poisoned") {
					Poison_Anim.Update(dt);
					const SDL_Rect& poisonframe = Poison_Anim.GetCurrentFrame();
					int px = drawX + (frame.w / 2) - (poisonframe.w / 2);
					int py = drawY - poisonframe.h / 2;
					Engine::GetInstance().render->DrawTexture(Poison_Texture, px, py, &poisonframe);
				}
				if (enemy.status == "paralized") {
					Paralized_Anim.Update(dt);
					const SDL_Rect& paralizedframe = Paralized_Anim.GetCurrentFrame();
					int px = drawX + (frame.w / 2) - (paralizedframe.w / 2);
					int py = drawY - paralizedframe.h / 2;
					Engine::GetInstance().render->DrawTexture(Paralized_Texture, px, py, &paralizedframe);
				}


				// healthbar

				//outline (position relative to sprite)
				enemy.hp_outline.x = drawX;
				enemy.hp_outline.y = drawY - 20;
				Engine::GetInstance().render->DrawRectangle(enemy.hp_outline, 0, 0, 0, 255, true);

				//inner part
				float Wmax = (float)enemy.hp_outline.w;

				float unity = Wmax / enemy.maxhp;

				enemy.hp_Interior.w = enemy.hp * unity;

				enemy.hp_Interior.x = drawX - 2;
				enemy.hp_Interior.y = enemy.hp_outline.y - 2;

				enemy.hp_Interior.h = enemy.hp_outline.h;

				if (enemy.hp > 0) {
					//red
					Engine::GetInstance().render->DrawRectangle(enemy.hp_Interior, 255, 0, 0, 255, true);

					// print hp
					std::string s = std::to_string(enemy.hp);
					const char* pchar = s.c_str();

					Engine::GetInstance().render->DrawText(pchar, enemy.hp_Interior.x - 32, enemy.hp_Interior.y - 16, 32, 32, { 255, 255, 255, 255 });
				
				}
			
				// marcar objetivo + calcular damage
				if (i == combatState->enemy_index_targeted && combatState->selecting_target)
				{

					// output
					int output_x = enemy.hp_Interior.x + enemy.hp_Interior.w - (combatState->player_attack_dmg_selected * unity);
					int output_w = combatState->player_attack_dmg_selected * unity;
					if (output_x < enemy.hp_Interior.x) {
						output_x = enemy.hp_Interior.x;
						output_w = enemy.hp_Interior.w;
					}

					SDL_Rect output = { output_x, enemy.hp_Interior.y, output_w ,  enemy.hp_Interior.h };
					Engine::GetInstance().render->DrawRectangle(output, 255, 255, 0, 255, true);

					// resaltar (outline around sprite)
					SDL_Rect outline = { drawX - 4, drawY - 4, frame.w + 8, frame.h + 8 };
					Engine::GetInstance().render->DrawRectangle(outline, 255, 255, 0, 255, false);
				}
			}
			else
			{
				// fallback (centered)
				SDL_Rect rect = { x - 32, y - 32, 64, 64 };
				Engine::GetInstance().render->DrawRectangle(rect, 200, 40, 40, 200, true);
			}
		}
    }
	for (const auto& text : floatingTexts)
	{
		Engine::GetInstance().render->DrawText(text.text.c_str(), (int)text.position.getX(), (int)text.position.getY(), 64, 64, text.color );
	}
}

void CombatManager::SpawnFloatingText(const std::string& text, float x, float y, SDL_Color color) 
{
	FloatingText ft;

	ft.text = text;
	ft.position = { x, y };
	ft.color = color;

	floatingTexts.push_back(ft);
}

void CombatManager::ResetCombatState()
{
	waitingAttack = false;
	waitingDamage = false;
	waitingEffect = false;
	waitingEnemyTurn = false;

	combatTimer = 0.0f;

	currentAttacker = nullptr;
	currentTarget = nullptr;

	pendingDamage = 0;

	PlayerHasAttacked = false;

	combatState->selecting_target = false;
}