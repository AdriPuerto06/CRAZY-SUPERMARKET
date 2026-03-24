#include "Tomato.h"
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

Tomato::Tomato() : BaseEnemy(EntityType::ENEMY) {}

bool Tomato::Awake()
{
	return true;
}

bool Tomato::Start()
{
	active = true;
	HP = 100;

	texture = Engine::GetInstance().textures->Load("Assets/Textures/enemy_spritesheet.png");
	if (texture == nullptr) {
		LOG("Failed to load Tomato Texture");
	}
	texW = 32;
	texH = 32;
	pbody = Engine::GetInstance().physics->CreateCircle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW / 2, bodyType::DYNAMIC);
	pbody->listener = this;
	pbody->ctype = ColliderType::ENEMY;

	return true;
}

bool Tomato::Update(float dt) {
	Draw(dt);

	return true;
}


void Tomato::Draw(float dt) {
	int x, y;
	pbody->GetPosition(x, y);
	position.setX((float)x);
	position.setY((float)y);
	Engine::GetInstance().render->DrawTexture(texture, x - texW / 2, y - texH / 2);

}

bool Tomato::CleanUp() {
	LOG("Cleanup Tomato");
	Engine::GetInstance().textures->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);
	return true;
}

bool Tomato::Destroy() {
	LOG("Destroying Tomato");
	active = false;
	pendingToDelete = true;
	return true;
}