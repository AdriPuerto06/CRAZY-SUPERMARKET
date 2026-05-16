#pragma once

#include "Entity.h"
#include <vector>

//enum class EventType {
//	DOOR,
//	ROCK,
//	MESSAGE,
//	HEALER,
//	KEY
//};

class Event : public Entity {
public:
	Event();
	~Event();
	Event(EntityType type) : type(type), active(true) {};

	void Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID, const char* event_Name, bool activated);
	bool Awake();
	bool Start();
	bool Update(float dt);
	void Draw(float dt);
	bool CleanUp();
	bool Destroy();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	bool IsEventActive(const char* name);
	void PossibleActivate(const char* name);

public:
	EntityType type;
	const char* event_Name;
	bool active; //is the event not "dead"
	bool activated; //is activated by the player in-game (is the door open)
	bool renderable = true;

	PhysBody* pbody;
	Vector2D position;
	SDL_Texture* texture = NULL;
	const char* texturePath;
	int texW, texH;

	int ID;

	std::vector<int> active_Events;
};