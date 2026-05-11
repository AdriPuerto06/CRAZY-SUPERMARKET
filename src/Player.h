#pragma once

#include "Entity.h"
#include "Animation.h"
#include <box2d/box2d.h>
#include <SDL3/SDL.h>

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool Destroy();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	Vector2D GetPosition();
	void SetPosition(Vector2D pos);

private:

	void GetPhysicsValues();
	void Move();
	void Teleport();
	void ApplyPhysics();
	void Draw(float dt);
	void GodMode();
	void CenterCamera();
	void CheckDialogueAndCombatLogic();
	void ShowMenu();

public:

	//Declare player parameters
	float speed = 4.0f;
	bool can_Move = true;
	SDL_Texture* texture = nullptr;

	int texW, texH;

	//Audio fx
	int pickCoinFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;

	int HP;
	bool inCombat = false;

	std::string pendingMapLoad;

private:
	b2Vec2 velocity;
	AnimationSet anims;
	int teleportCooldown = 120;
	bool showingMenu = false;

	bool godMode = false;
	
};