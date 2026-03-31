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

void BaseEnemy::Init(EntityType type, Vector2D position, int HP, int ID, SDL_Texture* texture) 
{
	this->type = type;
	this->position = position;
	this->HP = HP;
	this->ID = ID;
	this->texture = texture;
}

BaseEnemy::~BaseEnemy() {

}

bool BaseEnemy::Awake() {
	return true;
}

bool BaseEnemy::Start() {

	// load
	std::unordered_map<int, std::string> aliases = { {0,"idle"} };
	anims.LoadFromTSX("Assets/Textures/enemy_Spritesheet.tsx", aliases);
	anims.SetCurrent("idle");

	//Initialize Player parameters
	texture = Engine::GetInstance().textures->Load("Assets/Textures/enemy_spritesheet.png");
	
	//Add physics to the enemy - initialize physics body
	texW = 32;
	texH = 32;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX()+texW/2, (int)position.getY()+texH/2, texW / 2, bodyType::DYNAMIC);

	//Assign enemy class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	//ssign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Initialize pathfinding
	pathfinding = std::make_shared<Pathfinding>();
	//Get the position of the enemy
	Vector2D pos = GetPosition();
	//Convert to tile coordinates
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap((int)pos.getX(), (int)pos.getY()+1);
	//Reset pathfinding
	pathfinding->ResetPath(tilePos);

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
