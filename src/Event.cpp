#include "Event.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Physics.h"
#include "DialogueManager.h"
#include "Window.h"
#include "Log.h"
#include "EventManager.h"
#include "EntityManager.h"

#include "Player.h"

Event::Event() {};
Event::~Event() {};

void Event::Init(EntityType type, bool active, Vector2D position, const char* texturePath, int ID, const char* event_Name, bool activated)
{
	this->type = type;
	this->active = active;
	this->position = position;
	this->texturePath = texturePath;
	this->ID = ID;
	this->event_Name = event_Name;
	this->activated = activated;
}

bool Event::Awake() {

	return true;
}

bool Event::Start() {
	//texture
	texture = Engine::GetInstance().textures->Load(texturePath);
	texH = texture->h;
	texW = texture->w;
	//sensor
	bodyType type;
	if (std::strcmp(event_Name, "Rock1") == 0 || std::strcmp(event_Name, "Rock2") == 0)
	{
		pbody = Engine::GetInstance().physics->CreateRectangle(position.getX() + texW / 2, position.getY() + texH / 2, texH * 1.1, texW * 1.1, bodyType::DYNAMIC);
		/*b2Polygon box = b2MakeBox(texH, texW);
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.density = 0.5f;

		b2CreatePolygonShape(pbody->body, &shapeDef, &box);*/
	}
	else pbody = Engine::GetInstance().physics->CreateRectangle(position.getX() + texW / 2, position.getY() + texH / 2, texH * 1.1, texW * 1.1, bodyType::STATIC);
	pbody->ctype = ColliderType::EVENT;
	pbody->listener = this;
	return true;
}

bool Event::Update(float dt) {
	Draw(dt);
	return true;
}

bool Event::CleanUp() {
	LOG("Cleanup Event");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	active_Events.clear();
	return true;
}

bool Event::Destroy() {
	LOG("Destroying Event");
	active = false;
	pendingToDelete = true;
	return true;
}

void Event::Draw(float dt)
{
	Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY());
}

void Event::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().eventManager->PossibleActivate(event_Name);
		break;

	}

}

void Event::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	
}
