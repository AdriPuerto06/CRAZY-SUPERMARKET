#include "BaseCompanion.h"
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

BaseCompanion::BaseCompanion(){}

void BaseCompanion::Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID)
{
	this->type = type;
	this->position = position;
	this->active = active;
	this->HP = HP;
	this->ID = ID;
	this->texturePath = texturePath;
}

BaseCompanion::~BaseCompanion() {

}

bool BaseCompanion::Awake() {
	return true;
}

bool BaseCompanion::Start() {

	//texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	texH = texture->h;
	texW = texture->w;
	//sensor
	pbody = Engine::GetInstance().physics->CreateRectangle(position.getX() + texW / 2, position.getY() + texH / 2, texH * 1.25, texW * 1.25, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::COMPANION;
	pbody->listener = this;

	pathfinding = std::make_shared<Pathfinding>();
	
	return true;
}

bool BaseCompanion::Update(float dt)
{

	Move();
	Draw(dt);
	PerformPathfinding();

	return true;
}

void BaseCompanion::Move() {

	if (!pathfinding->pathTiles.empty()) {
		Vector2D nextTile = pathfinding->GetPenultimateTile(pathfinding->pathTiles);
		// Convert the current position of the enemy from world space to map tiles.
		Vector2D pos = Engine::GetInstance().map->WorldToMap((int)GetPosition().getX(), (int)GetPosition().getY());

		// Calculate the direction to move to the next tile.
		Vector2D direction = nextTile - pos;

		if (direction.getX() != 0 && direction.getY() != 0) {
			float dirX = (direction.getX() > 0) ? 1 : -1;  // Move right if X > 0, left if X < 0.
			float dirY = (direction.getY() < 0) ? 1 : -1;
			velocity.x = dirX * speed;
			velocity.y = dirY * speed;

			// Set animation based on movement direction.
			if (velocity.x < 0) {
				anims.SetCurrent("moveLeft");
			}
			else {
				anims.SetCurrent("moveRight");
			}
			if (velocity.y > 0) {
				anims.SetCurrent("moveDown");
			}
			else {
				anims.SetCurrent("moveUp");
			}

			// Update components
			b2Vec2 currentVel = Engine::GetInstance().physics->GetLinearVelocity(pbody);
			Engine::GetInstance().physics->SetLinearVelocity(pbody, velocity.x, velocity.y);

		}
	}
}

void BaseCompanion::PerformPathfinding() {
	Vector2D pos = Vector2D(GetPosition().getX() + texW / 2, GetPosition().getY() + texH / 2);
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap((int)pos.getX(), (int)pos.getY() + 1);
	Vector2D playerTilePos = Engine::GetInstance().map.get()->WorldToMap((int)Engine::GetInstance().scene->GetPlayerPosition().getX(), (int)Engine::GetInstance().scene->GetPlayerPosition().getY() + 1);

	if (tilePos == playerTilePos) {
		pathfinding->ResetPath(tilePos);
		return;
	}

	if (GetDistanceFromPlayer() < separationRange) {

		pathfinding->ResetPath(tilePos);

		int maxIterations = 20;
		int iterations = 0;
		while (!(GetDistanceFromPlayer() < separationRange) && iterations < maxIterations) {
			pathfinding->PropagateAStar(ASTAR_HEURISTICS::SQUARED);
			iterations++;
		}
	}
}

void BaseCompanion::Draw(float dt) {

	anims.Update(dt);
	const SDL_Rect& animFrame = anims.GetCurrentFrame();

	// Update render position using your PhysBody helper
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);

	// Draw pathfinding debug
	//pathfinding->DrawPath();

	//Draw the player using the texture and the current animation frame
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2);
}

bool BaseCompanion::CleanUp()
{
	LOG("Cleanup enemy");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool BaseCompanion::Destroy()
{
	LOG("Destroying Companion");
	active = false;
	pendingToDelete = true;
	return true;
}

void BaseCompanion::SetPosition(Vector2D pos) {
	pbody->SetPosition((int)(pos.getX()), (int)(pos.getY()));
}

void BaseCompanion::OnCollision(PhysBody* physA, PhysBody* physB) 
{

}

void BaseCompanion::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	
}

Vector2D BaseCompanion::GetPosition()
{
	if (!pbody)
		return position;

	int x, y;
	pbody->GetPosition(x, y);
	return Vector2D((float)x - texW / 2, (float)y - texH / 2);
}

float BaseCompanion::GetDistanceFromPlayer()
{
	Vector2D tilePos = Engine::GetInstance().map->WorldToMap(
		(int)GetPosition().getX(),
		(int)GetPosition().getY() + 1
		);
	Vector2D playerTilePos = Engine::GetInstance().map->WorldToMap(
		(int)Engine::GetInstance().scene->GetPlayerPosition().getX(),
		(int)Engine::GetInstance().scene->GetPlayerPosition().getY());

	return std::pow(playerTilePos.getX() - tilePos.getX(), 2) +
		std::pow(playerTilePos.getY() - tilePos.getY(), 2);
}