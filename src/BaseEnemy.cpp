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

BaseEnemy::BaseEnemy(){}

void BaseEnemy::Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID)
{
	this->type = type;
	this->position = position;
	this->HP = HP;
	this->ID = ID;
	this->texturePath = texturePath;
}

BaseEnemy::~BaseEnemy() {

}

bool BaseEnemy::Awake() {
	return true;
}

bool BaseEnemy::Start() {

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

	// Draw pathfinding debug
	pathfinding->DrawPath();

	//Draw the player using the texture and the current animation frame
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2, &animFrame);
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
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

Vector2D BaseEnemy::GetPosition() {
	int x, y;
	pbody->GetPosition(x, y);
	// Adjust for center
	return Vector2D((float)x-texW/2,(float)y-texH/2);
}

//Define OnCollision function for the enemy. 
void BaseEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {

}

void BaseEnemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{

}
