#pragma once

#include "Entity.h"
#include <vector>
#include "Animation.h"

class BaseNPC : public Entity {
public:
	BaseNPC();
	~BaseNPC();
	BaseNPC(EntityType type) : type(type), active(true) {};

	void Init(EntityType type, bool active, Vector2D position, const char* texturePath, const char* anim_tsxpath, int ID, int currentDialogueTree);

	bool Awake();

	bool Start();

	bool Update(float dt);

	void Draw(float dt);

	bool CleanUp();

	bool Destroy();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

public:
	EntityType type;
	bool active;
	bool renderable = true;

	PhysBody* pbody;
	Vector2D position;
	SDL_Texture* texture = NULL;
	const char* texturePath;
	const char* anim_tsxpath;
	int texW, texH;
	AnimationSet anims;

	int ID;
	int currentDialogueTree;
	bool showingButton;

	// hitbox
	int hitW = 0;
	int hitH = 0;
};