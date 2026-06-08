#include "BaseNPC.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Physics.h"
#include "DialogueManager.h"
#include "Window.h"
#include "Log.h"
#include <unordered_map>

BaseNPC::BaseNPC() {};
BaseNPC::~BaseNPC() {};

void BaseNPC::Init(EntityType type, bool active, Vector2D position, const char* texturePath, const char* anim_tsxpath, int ID, int currentDialogueTree)
{
	this->type = type;
	this->active = active;
	this->position = position;
	this->texturePath = texturePath;
	this->anim_tsxpath = anim_tsxpath;
	this->ID = ID;
	this->currentDialogueTree = currentDialogueTree;
}

bool BaseNPC::Awake() {

	return true;
}

bool BaseNPC::Start() {
	// texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

	// cargar animaciones si hay TSX
	if (anim_tsxpath != nullptr && anim_tsxpath[0] != '\0') {
		std::unordered_map<int, std::string> emptyAliases;
		anims.LoadFromTSX(anim_tsxpath, emptyAliases);
	}

	// Determinar tamaño de hitbox basado en el frame actual de la animación.
	// Si no hay animación válida, fallback al tamaño completo de la textura.
	anims.Update(0); // asegurar que el set tenga un frame actual si es posible
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	int frameW = texW;
	if (animFrame.w != 0)
	{
		frameW = animFrame.w;
	}
	int frameH = texH;
	if (animFrame.h != 0)
	{
		frameH = animFrame.h;
	}

	hitW = frameW;
	hitH = frameH;

	// sensor: creamos la hitbox centrada en la posición usando el tamaño del frame
	pbody = Engine::GetInstance().physics->CreateRectangle((int)(position.getX() + hitW / 2),(int)(position.getY() + hitH / 2),hitW,hitH,bodyType::STATIC);
	pbody->ctype = ColliderType::NPC;
	pbody->listener = this;
	// bools
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
	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position usando el PhysBody
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	bool hasAnimFrame = (animFrame.w != 0 && animFrame.h != 0);
	if (texture != nullptr) {
		if (hasAnimFrame) {
			Engine::GetInstance().render->DrawTexture(texture, x - animFrame.w / 2, y - animFrame.h / 2, &animFrame);
		}
		else {
			Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2);
		}
	}
}

void BaseNPC::OnCollision(PhysBody* physA, PhysBody* physB)
{
	if (Engine::GetInstance().dialogueManager->in_conversation) return;
	if (!(physB->ctype == ColliderType::PLAYER) && showingButton) return;

	/*Vector2D buttonPos = Vector2D((position.getX() + texW / 2), (position.getY() + texH * 1.5));*/
	Vector2D buttonPos = Vector2D(500,500);
	Engine::GetInstance().dialogueManager->ShowButtonStart(buttonPos, currentDialogueTree, ID);
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