#pragma once
#include "UIButton.h"
#include <vector>

enum class RewardType {
	QUEST,
	ITEM,
	COMPANION,
	COMPLETEQUEST,
	ATTACK,
	DIALOGUE,
	NONE
};

struct Reward {
	RewardType type;
	std::string reward_value;

	Reward() {}

	Reward(RewardType type, std::string str)
	{
		this->type = type;
		this->reward_value = str;
	}
};

struct DialogueTree {
	std::vector<const char*> nodes_text;
	std::vector<int> nodes_id;
	std::vector<std::vector<int>> choices_id;
	std::vector<std::vector<const char*>> choices_text;
	std::vector<std::vector<int>> choices_next_node;
	std::vector<std::vector<Reward>> rewards;

	void Clear()
	{
		nodes_text.clear();
		nodes_id.clear();
		choices_id.clear();
		choices_text.clear();
		choices_next_node.clear();
		rewards.clear();
	}
};

struct CurrentDialogue {
	int dialogue_tree_ID;
	int dialogue_tree_NPC;
	int node_id;
	int choice;
	int next_node;
};

class DialogueManager : public Module {
public:
	DialogueManager();

	// Destructor
	virtual ~DialogueManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool LoadDialogs(std::string path, std::string fileName);
	void UnloadDialogueUI();

	bool OnUIMouseClickEvent(UIElement* uiElement);
	void ButtonAction(int ID);
	void ShowButtonStart(Vector2D position, int dialogue_tree_ID, int npc_id);
	bool StartDialogue(int dialogue_tree_ID, int npc_id);
	bool ShowOptions(int node_value);
	
	/*const char* GetTextFromNode(int dialogue_tree_ID, int node_value);*/
	void GetTreeAttributes(int dialogue_tree_ID, int npc_id);
	void GetPosibleReward(Reward reward);

	void UnlockNewDialogueTree(int NPC_ID);

	std::string dialogsFileName;
	std::string dialogsPath;


	CurrentDialogue* dialogue;
	DialogueTree* tree;

	bool showing_continue;
	bool can_be_clicked = true;
	bool showingButtonStart = false;
	bool in_conversation = false;

	/*std::vector<int> currentDialogueTreesNPC;*/

private:
	pugi::xml_document dialogsFileXML;

};