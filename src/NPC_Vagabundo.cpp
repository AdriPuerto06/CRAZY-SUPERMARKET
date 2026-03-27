#include "NPC_Vagabundo.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"

NPC_Vagabundo::NPC_Vagabundo() {};
NPC_Vagabundo::NPC_Vagabundo(int ID) { this->ID = ID; }
NPC_Vagabundo::~NPC_Vagabundo() {};

bool NPC_Vagabundo::Awake() {

	return true;
}

bool NPC_Vagabundo::Start() {
	texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");
	return true;
}

bool NPC_Vagabundo::Update(float dt) {
	Draw(dt);
	return true;
}

bool NPC_Vagabundo::CleanUp() {

	return true;
}

bool NPC_Vagabundo::Destroy() {

	return true;
}

void NPC_Vagabundo::Draw(float dt)
{
	Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY());
}