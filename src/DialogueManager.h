#pragma once
#include "UIButton.h"
#include <vector>

struct TextNode {
	int node_id;
	const char* text;
	std::vector<const char*> choices;
	std::vector<int> id;
	std::vector<int> next;
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
	bool StartDialog(int dialogue_tree_ID, int node_value);
	bool ShowOptions(int dialogue_tree_ID, int node_value);
	
	const char* GetTextFromNode(int dialogue_tree_ID, int node_value);
	void GetNodeAttributes(TextNode* node, int dialogue_tree_ID, int npc_id);

	std::string dialogsFileName;
	std::string dialogsPath;

	TextNode current_text_node;

private:
	pugi::xml_document dialogsFileXML;

	
};