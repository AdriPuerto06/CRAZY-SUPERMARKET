#pragma once
#include "UIButton.h"
#include <vector>

struct Attack {
	const char* name;
	int dmg;
};

struct CombatData {
	std::vector<int> players_id;
	std::vector<std::vector<Attack>> players_attacks;

	std::vector<int> enemies_id;
	std::vector<std::vector<Attack>> enemies_attacks;
};

struct CombatState {
	std::vector<std::vector<int>> HPs;
	int enemy_id_targeted;
	int player_id_targeted;
	int player_id_selected;
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
	void ShowButtonStart(Vector2D position);
	bool StartCombat(std::vector<int> player_IDs, std::vector<int> enemies_IDs);
	bool ShowOptions(int player_ID);
	
	/*const char* GetTextFromNode(int dialogue_tree_ID, int node_value);*/
	void GetTreeAttributes();

	std::string combatFileName;
	std::string combatPath;

	CombatData* combatData;
	CombatState* combatState;

	bool showing_continue;
	bool can_be_clicked = true;
	bool showingButtonStart = false;
	bool in_conversation = false;

private:
	pugi::xml_document combatFileXML;

};