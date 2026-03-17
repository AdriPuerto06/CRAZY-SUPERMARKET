#pragma once
#include "UIButton.h"
#include <vector>

struct DialogTree {
	std::vector<const char*> nodes_text;
	std::vector<int> nodes_id;
	std::vector<std::vector<int>> choices_id;
	std::vector<std::vector<const char*>> choices_text;
	std::vector<std::vector<int>> choices_next_node;
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

	// Called before quitting
	bool CleanUp();

	bool LoadDialogs(std::string path, std::string fileName);

	bool OnUIMouseClickEvent(UIElement* uiElement);
	bool StartDialog(int dialogue_tree_ID, int npc_id);
	bool ShowOptions(int dialogue_tree_ID, int node_value);
	
	const char* GetTextFromNode(int dialogue_tree_ID, int node_value);
	void GetTreeAttributes(DialogTree* node, int dialogue_tree_ID, int npc_id);

	std::string dialogsFileName;
	std::string dialogsPath;

private:
	pugi::xml_document dialogsFileXML;

	
};