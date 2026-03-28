#pragma once
#include "UIButton.h"
#include <vector>

struct DialogueTree {
	std::vector<const char*> nodes_text;
	std::vector<int> nodes_id;
	std::vector<std::vector<int>> choices_id;
	std::vector<std::vector<const char*>> choices_text;
	std::vector<std::vector<int>> choices_next_node;
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

	std::string dialogsFileName;
	std::string dialogsPath;


	CurrentDialogue* dialogue;
	DialogueTree* tree;

	bool showing_continue;
	bool can_be_clicked = true;
	bool showingButtonStart = false;

private:
	pugi::xml_document dialogsFileXML;

};