#include "BaseEnemy.h"
#include "Entity.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Map.h"
#include "CombatManager.h"
#include <unordered_map>

BaseEnemy::BaseEnemy(){}

void BaseEnemy::Init(EntityType type, bool active, Vector2D position, const char* texturePath, const char* anim_tsxpath, int ID, int fight_ID)
{
	this->type = type;
	this->position = position;
	this->active = active;
	this->HP = HP;
	this->ID = ID;
	this->texturePath = texturePath;
	this->anim_tsxpath = anim_tsxpath;
	this->fight_ID = fight_ID;
}

BaseEnemy::~BaseEnemy() {

}

bool BaseEnemy::Awake() {
	return true;
}

bool BaseEnemy::Start() {

	//texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	Engine::GetInstance().textures->GetSize(texture, texW, texH);

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

	//sensor: creamos la hitbox centrada en la posición usando el tamaño del frame
	pbody = Engine::GetInstance().physics->CreateRectangle((int)(position.getX() + hitW / 2),(int)(position.getY() + hitH / 2),hitW,hitH,bodyType::STATIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;
	//bools
	showingButton = false;

	return true;
}

bool BaseEnemy::Update(float dt)
{

	Move();
	Draw(dt);

	return true;
}

void BaseEnemy::Move() {

	// Move 
}

void BaseEnemy::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
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

bool BaseEnemy::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool BaseEnemy::Destroy()
{
	LOG("Destroying Enemy");
	active = false;
	pendingToDelete = true;
	return true;
}

void BaseEnemy::SetPosition(Vector2D pos) {
	
	anims.Update(0);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	int w = texW;
	if (animFrame.w != 0)
	{
		w = animFrame.w;
	}
	int h = texH;
	if (animFrame.h != 0)
	{
		h = animFrame.h;
	}
	pbody->SetPosition((int)(pos.getX() + w / 2.0f), (int)(pos.getY() + h / 2.0f));
}

Vector2D BaseEnemy::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	anims.Update(0);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();
	int w = texW;
	if (animFrame.w != 0)
	{
		w = animFrame.w;
	}
	int h = texH;
	if (animFrame.h != 0)
	{
		h = animFrame.h;
	}
	return Vector2D((float)x - w / 2.0f, (float)y - h / 2.0f);
}

//Define OnCollision function for the enemy. 
void BaseEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	if (Engine::GetInstance().combatManager->in_combat) return;
	if (!(physB->ctype == ColliderType::PLAYER) && showingButton) return;

	Vector2D buttonPos = Vector2D{ 500,500 };
	Engine::GetInstance().combatManager->SaveScene();

	bool WJ = Engine::GetInstance().scene->GetPlayer()->WizardJoined;;
	bool CJ = Engine::GetInstance().scene->GetPlayer()->CorneliusJoined;
	Engine::GetInstance().combatManager->isWizardActive = WJ;
	Engine::GetInstance().combatManager->isCorneliusActive = CJ;

	Engine::GetInstance().combatManager->ShowButtonStart(buttonPos, this->ID, fight_ID);
	Engine::GetInstance().combatManager->showingButtonStart = true;
}

void BaseEnemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	if ((physB->ctype == ColliderType::PLAYER) && Engine::GetInstance().combatManager->showingButtonStart && !(Engine::GetInstance().combatManager->in_combat))
	{
		Engine::GetInstance().combatManager->UnloadCombatUI();
		Engine::GetInstance().combatManager->in_combat = false;
		Engine::GetInstance().combatManager->showingButtonStart = false;
	}
}



