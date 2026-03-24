#pragma once

#include "BaseEnemy.h"
#include "Animation.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public BaseEnemy
{
public:

	Enemy();
	virtual ~Enemy();
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
	void PerformPathfinding();
	void GetPhysicsValues();
	void Move();
	void ApplyPhysics();
	void Draw(float dt);

};