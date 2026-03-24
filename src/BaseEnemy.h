#include "Entity.h"
#include "Pathfinding.h"
#include <vector>

class BaseEnemy : public Entity {
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

	virtual void PerformPathfinding() = 0;
	virtual void GetPhysicsValues() = 0;
	virtual void Move() = 0;
	virtual void ApplyPhysics() = 0;
	virtual void Draw(float dt) = 0;

protected:
	EntityType type;
	bool active;
	bool renderable = true;

	PhysBody* pbody;
	Vector2D position;
	SDL_Texture* texture = NULL;
	int texW, texH;
	AnimationSet anims;

	b2Vec2 velocity;
	float speed;
	std::shared_ptr<Pathfinding> pathfinding;

	int HP;
	std::vector<std::string> attack_names;
	std::vector<int> attack_damage;

	int ID;
	
};