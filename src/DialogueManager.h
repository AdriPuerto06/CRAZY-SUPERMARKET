#pragma once
#include "UIButton.h"

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
	bool ShowOptions(int node_value);
	
	const char* GetTextFromNode(int node_value);

	std::string dialogsFileName;
	std::string dialogsPath;

private:
	pugi::xml_document dialogsFileXML;

};