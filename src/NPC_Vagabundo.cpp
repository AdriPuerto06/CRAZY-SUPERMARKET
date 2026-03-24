#include "NPC_Vagabundo.h"

NPC_Vagabundo::NPC_Vagabundo() {};
NPC_Vagabundo::NPC_Vagabundo(int ID) { this->ID = ID; }
NPC_Vagabundo::~NPC_Vagabundo() {};

bool NPC_Vagabundo::Awake() {

	return true;
}

bool NPC_Vagabundo::Start() {

	return true;
}

bool NPC_Vagabundo::Update(float dt) {

	return true;
}

bool NPC_Vagabundo::CleanUp() {

	return true;
}

bool NPC_Vagabundo::Destroy() {

	return true;
}