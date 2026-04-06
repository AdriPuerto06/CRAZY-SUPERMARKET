#pragma once
#include "UIButton.h"
#include "Scene.h"
#include <vector>

struct Attack {
	const char* name;
	int dmg;
};

struct CombatData {
	std::vector<int> players_id;
	std::vector<int> players_HP;
	std::vector<std::vector<Attack>> players_attacks;

	std::vector<int> enemies_id;
	std::vector<int> enemies_HP;
	std::vector<std::vector<Attack>> enemies_attacks;

	int possible_enemy_ID;
};

struct CombatState {
	std::string turn;

	std::vector<int> current_players_HP;
	std::vector<int> current_enemies_HP;
	std::vector<bool> players_alive;
	std::vector<bool> enemies_alive;

	int enemy_id_targeted;
	int player_id_targeted;
	int player_id_selected;
	int player_attack_dmg_selected;
	int enemy_attack_dmg_selected;

	bool player_Wins;
	bool enemy_Wins;
	bool selecting_target;

	void Init()
	{
		for (int i = 0; i < current_players_HP.size(); ++i)
		{
			players_alive.push_back(true);
		}
		for (int i = 0; i < current_enemies_HP.size(); ++i)
		{
			enemies_alive.push_back(true);
		} //set all to alive
		turn = "Player";
		player_Wins = false;
		enemy_Wins = false;
		bool selecting_target = false;
	}

};

class CombatManager : public Module {
public:
	CombatManager();

	// Destructor
	virtual ~CombatManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool LoadCombatData(std::string path, std::string fileName);
	void UnloadCombatUI();

	bool OnUIMouseClickEvent(UIElement* uiElement);
	void ButtonAction(int ID);
	void ShowButtonStart(Vector2D position, int enemy_ID);
	bool StartCombat(std::vector<int> player_IDs, std::vector<int> enemies_IDs);
	bool ShowAttackOptions(int player_ID);
	bool ShowItemOptions(int player_ID);
	bool ChangePlayer();
	bool ShowOptions(int player_ID);

	/*const char* GetTextFromNode(int dialogue_tree_ID, int node_value);*/
	void GetTreeAttributes();
	//right now this is just a choose randomn
	void EnemyAI();

	void HandleTargetSelection();
	void ApplyCombatLogic();
	void CheckAlive();

	std::string combatFileName;
	std::string combatPath;

	CombatData* combatData;
	CombatState* combatState;

	bool showing_continue;
	bool can_be_clicked = true;
	bool showingButtonStart = false;
	bool in_combat = false;

	bool godMode;
	bool choosingAtk;

private:
	pugi::xml_document combatFileXML;

	SceneID timeScene;
	SceneID currentScene;

};
