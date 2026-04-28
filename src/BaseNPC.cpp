#include "BaseNPC.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Physics.h"
#include "DialogueManager.h"
#include "Window.h"
#include "Log.h"

BaseNPC::BaseNPC() {};
BaseNPC::~BaseNPC() {};

void BaseNPC::Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID)
{
	this->type = type;
	this->active = active;
	this->position = position;
	this->texturePath = texturePath;
	this->ID = ID;
}

bool BaseNPC::Awake() {

	return true;
}

bool BaseNPC::Start() {
	//texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	texH = texture->h;
	texW = texture->w;
	//sensor
	pbody = Engine::GetInstance().physics->CreateRectangle(position.getX() + texW / 2, position.getY() + texH / 2, texH * 1.25, texW * 1.25, bodyType::STATIC);
	pbody->ctype = ColliderType::NPC;
	pbody->listener = this;
	//bools
	showingButton = false;

	return true;
}

bool BaseNPC::Update(float dt) {
	Draw(dt);
	return true;
}

bool BaseNPC::CleanUp() {
	LOG("Cleanup NPC");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool BaseNPC::Destroy() {
	LOG("Destroying NPC");
	active = false;
	pendingToDelete = true;
	return true;
}

void BaseNPC::Draw(float dt)
{
	Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY());
}

void BaseNPC::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (Engine::GetInstance().dialogueManager->in_conversation) return;
	if (!(physB->ctype == ColliderType::PLAYER) && showingButton) return;

	/*Vector2D buttonPos = Vector2D((position.getX() + texW / 2), (position.getY() + texH * 1.5));*/
	Vector2D buttonPos = Vector2D(500,500);
	Engine::GetInstance().dialogueManager->ShowButtonStart(buttonPos, 0, ID);
	Engine::GetInstance().dialogueManager->showingButtonStart = true;
}

void BaseNPC::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if ((physB->ctype == ColliderType::PLAYER) && Engine::GetInstance().dialogueManager->showingButtonStart && !(Engine::GetInstance().dialogueManager->in_conversation))
	{
		Engine::GetInstance().dialogueManager->UnloadDialogueUI();
		Engine::GetInstance().dialogueManager->in_conversation = false;
		Engine::GetInstance().dialogueManager->showingButtonStart = false;
	}
}