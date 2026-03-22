#include "Entity.h"
#include <vector>

class BaseEnemy : Entity {
public:
	BaseEnemy() {}
	BaseEnemy(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool Destroy()
	{
		return true;
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};

	virtual void OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	};

public:
	EntityType type;
	bool active;
	bool renderable = true;

	PhysBody* pbody;
	Vector2D position;
	SDL_Texture* texture = NULL;
	int texW, texH;

	int HP;
	std::vector<const char*> attack_names;
	std::vector<int> attack_damage;
	
	
};