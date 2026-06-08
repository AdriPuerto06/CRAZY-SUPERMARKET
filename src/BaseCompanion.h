#pragma once

#include "Entity.h"
#include "Animation.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"

struct SDL_Texture;

class BaseCompanion : public Entity
{
public:

	BaseCompanion();
	~BaseCompanion();
	
	void Init(EntityType type, bool active, Vector2D position, const char* texturePath, const char* anim_tsxpath, int ID, int Dialogue_ID);
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	float GetDistanceFromPlayer();
	bool Destroy();

private:

	void Move();
	void Draw(float dt);
	void PerformPathfinding();

public:
	EntityType type;
	bool active;
	bool renderable = true;

	PhysBody* pbody;
	Vector2D position;
	SDL_Texture* texture = NULL;
	int texW, texH;
	AnimationSet anims;
	const char* texturePath;
	const char* anim_tsxpath;

	b2Vec2 velocity;
	float speed = 4.0f;
	std::shared_ptr<Pathfinding> pathfinding;
	float separationRange = 5.0f;

	int HP;
	int ID;
	int Dialogue_ID;


	int hitW = 0;
	int hitH = 0;
};