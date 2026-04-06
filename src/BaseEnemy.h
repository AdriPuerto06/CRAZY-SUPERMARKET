#pragma once

#include "Entity.h"
#include "Animation.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"

struct SDL_Texture;

class BaseEnemy : public Entity
{
public:

	BaseEnemy();
	~BaseEnemy();
	void Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID);
	bool Awake();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void SetPosition(Vector2D pos);
	Vector2D GetPosition();
	bool Destroy();

private:

	void Move();
	void Draw(float dt);

protected:
	EntityType type;
	bool active;
	bool renderable = true;

	PhysBody* pbody;
	Vector2D position;
	SDL_Texture* texture = NULL;
	int texW, texH;
	AnimationSet anims;
	const char* texturePath;

	b2Vec2 velocity;
	float speed;
	std::shared_ptr<Pathfinding> pathfinding;

	int HP;
	std::vector<std::string> attack_names;
	std::vector<int> attack_damage;

	int ID;
	bool showingButton;
};