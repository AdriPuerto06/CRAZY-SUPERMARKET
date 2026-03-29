#include "NPC_Vagabundo.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Physics.h"
#include "DialogueManager.h"
#include "Window.h"
#include "Log.h"

NPC_Vagabundo::NPC_Vagabundo() {};
NPC_Vagabundo::NPC_Vagabundo(int ID) { this->ID = ID; }
NPC_Vagabundo::~NPC_Vagabundo() {};

bool NPC_Vagabundo::Awake() {

	return true;
}

bool NPC_Vagabundo::Start() {
	//texture
	texture = Engine::GetInstance().textures->Load("Assets/Textures/goldCoin.png");
	texH = texture->h;
	texW = texture->w;
	//sensor
	pbody = Engine::GetInstance().physics->CreateRectangle(position.getX()+texW/2, position.getY() + texH / 2, texH*1.25 , texW*1.25 , bodyType::STATIC);
	pbody->ctype = ColliderType::NPC;
	pbody->listener = this;
	//bools
	showingButton = false;

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

void NPC_Vagabundo::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (Engine::GetInstance().dialogueManager->in_conversation) return;
	if (!(physB->ctype == ColliderType::PLAYER) && showingButton) return;

	float WindowW = Engine::GetInstance().window->GetWindowSize().getX();
	float WindowY = Engine::GetInstance().window->GetWindowSize().getY();

	float scaleX = WindowW / Engine::GetInstance().window->GetBaseWidth();
	float scaleY = WindowY / Engine::GetInstance().window->GetBaseHeight();

	Vector2D buttonPos = Vector2D((position.getX() + texW / 2) / scaleX, (position.getY() + texH * 1.5)/scaleY);
	Engine::GetInstance().dialogueManager->ShowButtonStart(buttonPos, 0, ID);
	Engine::GetInstance().dialogueManager->showingButtonStart = true;
}

void NPC_Vagabundo::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if ((physB->ctype == ColliderType::PLAYER) && Engine::GetInstance().dialogueManager->showingButtonStart && !(Engine::GetInstance().dialogueManager->in_conversation))
	{
		Engine::GetInstance().dialogueManager->UnloadDialogueUI();
		Engine::GetInstance().dialogueManager->in_conversation = false;
	}
}